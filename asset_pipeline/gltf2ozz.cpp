//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cassert>
#include <cstring>

#include "asset_pipeline/importer/import2ozz.h"
#include "vox.animation/offline/raw_animation_utils.h"
#include "vox.animation/runtime/skeleton.h"
#include "vox.base/containers/map.h"
#include "vox.base/containers/set.h"
#include "vox.base/logging.h"
#include "vox.simd_math/simd_math.h"

#define TINYGLTF_IMPLEMENTATION

// No support for image loading or writing
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)  // unreachable code
#pragma warning(disable : 4267)  // conversion from 'size_t' to 'type'
#endif                           // _MSC_VER

#include <tiny_gltf.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

namespace {

template <typename VectorType>
bool FixupNames(VectorType& _data, const char* _pretty_name, const char* _prefix_name) {
    vox::set<std::string> names;
    for (size_t i = 0; i < _data.size(); ++i) {
        bool renamed = false;
        typename VectorType::const_reference data = _data[i];

        std::string name(data.name);

        // Fixes unnamed animations.
        if (name.length() == 0) {
            renamed = true;
            name = _prefix_name;
            name += std::to_string(i);
        }

        // Fixes duplicated names, while it has duplicates
        for (auto it = names.find(name); it != names.end(); it = names.find(name)) {
            renamed = true;
            name += "_";
            name += std::to_string(i);
        }

        // Update names index.
        if (!names.insert(name).second) {
            assert(false && "Algorithm must ensure no duplicated animation names.");
        }

        if (renamed) {
            LOGI("{}#{} with name {} was renamed to {} in order to avoid duplicates.", _pretty_name, i, data.name, name)

            // Actually renames tinygltf data.
            _data[i].name = name;
        }
    }

    return true;
}

// Returns the address of a gltf buffer given an accessor.
// Performs basic checks to ensure the data is in the correct format
template <typename T>
vox::span<const T> BufferView(const tinygltf::Model& _model, const tinygltf::Accessor& _accessor) {
    const int32_t component_size = tinygltf::GetComponentSizeInBytes(_accessor.componentType);
    const int32_t element_size = component_size * tinygltf::GetNumComponentsInType(_accessor.type);
    if (element_size != sizeof(T)) {
        LOGE("Invalid buffer view access. Expected element size '{} got {} instead.", sizeof(T), element_size)
        return vox::span<const T>();
    }

    const tinygltf::BufferView& bufferView = _model.bufferViews[_accessor.bufferView];
    const tinygltf::Buffer& buffer = _model.buffers[bufferView.buffer];
    const T* begin = reinterpret_cast<const T*>(buffer.data.data() + bufferView.byteOffset + _accessor.byteOffset);
    return vox::span<const T>(begin, _accessor.count);
}

// Samples a linear animation channel
// There is an exact mapping between gltf and vox keyframes, so we just copy
// everything over.
template <typename KeyframesType>
bool SampleLinearChannel(const tinygltf::Model& _model,
                         const tinygltf::Accessor& _output,
                         const vox::span<const float>& _timestamps,
                         KeyframesType* _keyframes) {
    const size_t gltf_keys_count = _output.count;

    if (gltf_keys_count == 0) {
        _keyframes->clear();
        return true;
    }

    typedef typename KeyframesType::value_type::Value ValueType;
    const vox::span<const ValueType> values = BufferView<ValueType>(_model, _output);
    if (values.size_bytes() / sizeof(ValueType) != gltf_keys_count || _timestamps.size() != gltf_keys_count) {
        LOGE("gltf format error, inconsistent number of keys.")
        return false;
    }

    _keyframes->reserve(_output.count);
    for (size_t i = 0; i < _output.count; ++i) {
        const typename KeyframesType::value_type key{_timestamps[i], values[i]};
        _keyframes->push_back(key);
    }

    return true;
}

// Samples a step animation channel
// There are twice-1 as many vox keyframes as gltf keyframes
template <typename KeyframesType>
bool SampleStepChannel(const tinygltf::Model& _model,
                       const tinygltf::Accessor& _output,
                       const vox::span<const float>& _timestamps,
                       KeyframesType* _keyframes) {
    const size_t gltf_keys_count = _output.count;

    if (gltf_keys_count == 0) {
        _keyframes->clear();
        return true;
    }

    typedef typename KeyframesType::value_type::Value ValueType;
    const vox::span<const ValueType> values = BufferView<ValueType>(_model, _output);
    if (values.size_bytes() / sizeof(ValueType) != gltf_keys_count || _timestamps.size() != gltf_keys_count) {
        LOGE("gltf format error, inconsistent number of keys.")
        return false;
    }

    // A step is created with 2 consecutive keys. Last step is a single key.
    size_t numKeyframes = gltf_keys_count * 2 - 1;
    _keyframes->resize(numKeyframes);

    for (size_t i = 0; i < _output.count; i++) {
        typename KeyframesType::reference key = _keyframes->at(i * 2);
        key.time = _timestamps[i];
        key.value = values[i];

        if (i < _output.count - 1) {
            typename KeyframesType::reference next_key = _keyframes->at(i * 2 + 1);
            next_key.time = nexttowardf(_timestamps[i + 1], 0.f);
            next_key.value = values[i];
        }
    }

    return true;
}

// Samples a hermite spline in the form
// p(t) = (2t^3 - 3t^2 + 1)p0 + (t^3 - 2t^2 + t)m0 + (-2t^3 + 3t^2)p1 + (t^3 -
// t^2)m1 where t is a value between 0 and 1 p0 is the starting point at t = 0
// m0 is the scaled starting tangent at t = 0
// p1 is the ending point at t = 1
// m1 is the scaled ending tangent at t = 1
// p(t) is the resulting point value
template <typename T>
T SampleHermiteSpline(float _alpha, const T& p0, const T& m0, const T& p1, const T& m1) {
    assert(_alpha >= 0.f && _alpha <= 1.f);

    const float t1 = _alpha;
    const float t2 = _alpha * _alpha;
    const float t3 = t2 * _alpha;

    // a = 2t^3 - 3t^2 + 1
    const float a = 2.0f * t3 - 3.0f * t2 + 1.0f;
    // b = t^3 - 2t^2 + t
    const float b = t3 - 2.0f * t2 + t1;
    // c = -2t^3 + 3t^2
    const float c = -2.0f * t3 + 3.0f * t2;
    // d = t^3 - t^2
    const float d = t3 - t2;

    // p(t) = a * p0 + b * m0 + c * p1 + d * m1
    T pt = p0 * a + m0 * b + p1 * c + m1 * d;
    return pt;
}

// Samples a cubic-spline channel
// the number of keyframes is determined from the animation duration and given
// sample rate
template <typename KeyframesType>
bool SampleCubicSplineChannel(const tinygltf::Model& _model,
                              const tinygltf::Accessor& _output,
                              const vox::span<const float>& _timestamps,
                              float _sampling_rate,
                              float _duration,
                              KeyframesType* _keyframes) {
    (void)_duration;

    assert(_output.count % 3 == 0);
    size_t gltf_keys_count = _output.count / 3;

    if (gltf_keys_count == 0) {
        _keyframes->clear();
        return true;
    }

    typedef typename KeyframesType::value_type::Value ValueType;
    const vox::span<const ValueType> values = BufferView<ValueType>(_model, _output);
    if (values.size_bytes() / (sizeof(ValueType) * 3) != gltf_keys_count || _timestamps.size() != gltf_keys_count) {
        LOGE("gltf format error, inconsistent number of keys.")
        return false;
    }

    // Iterate keyframes at _sampling_rate steps, between first and last time
    // stamps.
    vox::animation::offline::FixedRateSamplingTime fixed_it(_timestamps[gltf_keys_count - 1] - _timestamps[0],
                                                            _sampling_rate);
    _keyframes->resize(fixed_it.num_keys());
    size_t cubic_key0 = 0;
    for (size_t k = 0; k < fixed_it.num_keys(); ++k) {
        const float time = fixed_it.time(k) + _timestamps[0];

        // Creates output key.
        typename KeyframesType::value_type key;
        key.time = time;

        // Makes sure time is in between the correct cubic keyframes.
        while (_timestamps[cubic_key0 + 1] < time) {
            cubic_key0++;
        }
        assert(_timestamps[cubic_key0] <= time && time <= _timestamps[cubic_key0 + 1]);

        // Interpolate cubic key
        const float t0 = _timestamps[cubic_key0];      // keyframe before time
        const float t1 = _timestamps[cubic_key0 + 1];  // keyframe after time
        const float alpha = (time - t0) / (t1 - t0);
        const ValueType& p0 = values[cubic_key0 * 3 + 1];
        const ValueType m0 = values[cubic_key0 * 3 + 2] * (t1 - t0);
        const ValueType& p1 = values[(cubic_key0 + 1) * 3 + 1];
        const ValueType m1 = values[(cubic_key0 + 1) * 3] * (t1 - t0);
        key.value = SampleHermiteSpline(alpha, p0, m0, p1, m1);

        // Pushes interpolated key.
        _keyframes->at(k) = key;
    }

    return true;
}

template <typename KeyframesType>
bool SampleChannel(const tinygltf::Model& _model,
                   const std::string& _interpolation,
                   const tinygltf::Accessor& _output,
                   const vox::span<const float>& _timestamps,
                   float _sampling_rate,
                   float _duration,
                   KeyframesType* _keyframes) {
    bool valid = false;
    if (_interpolation == "LINEAR") {
        valid = SampleLinearChannel(_model, _output, _timestamps, _keyframes);
    } else if (_interpolation == "STEP") {
        valid = SampleStepChannel(_model, _output, _timestamps, _keyframes);
    } else if (_interpolation == "CUBICSPLINE") {
        valid = SampleCubicSplineChannel(_model, _output, _timestamps, _sampling_rate, _duration, _keyframes);
    } else {
        LOGE("Invalid or unknown interpolation type {}.", _interpolation)
        valid = false;
    }

    // Check if sorted (increasing time, might not be stricly increasing).
    if (valid) {
        valid = std::is_sorted(_keyframes->begin(), _keyframes->end(),
                               [](typename KeyframesType::const_reference _a,
                                  typename KeyframesType::const_reference _b) { return _a.time < _b.time; });
        if (!valid) {
            LOGI("gltf format error, keyframes are not sorted in increasing order.")
        }
    }

    // Remove keyframes with strictly equal times, keeping the first one.
    if (valid) {
        auto new_end = std::unique(_keyframes->begin(), _keyframes->end(),
                                   [](typename KeyframesType::const_reference _a,
                                      typename KeyframesType::const_reference _b) { return _a.time == _b.time; });
        if (new_end != _keyframes->end()) {
            _keyframes->erase(new_end, _keyframes->end());

            LOGI("gltf format error, keyframe times are not unique. "
                 "Imported data were modified to remove keyframes at "
                 "consecutive equivalent times.")
        }
    }
    return valid;
}

vox::animation::offline::RawAnimation::TranslationKey CreateTranslationRestPoseKey(const tinygltf::Node& _node) {
    vox::animation::offline::RawAnimation::TranslationKey key;
    key.time = 0.0f;

    if (_node.translation.empty()) {
        key.value = vox::Vector3F::zero();
    } else {
        key.value = vox::Vector3F(static_cast<float>(_node.translation[0]), static_cast<float>(_node.translation[1]),
                                  static_cast<float>(_node.translation[2]));
    }

    return key;
}

vox::animation::offline::RawAnimation::RotationKey CreateRotationRestPoseKey(const tinygltf::Node& _node) {
    vox::animation::offline::RawAnimation::RotationKey key;
    key.time = 0.0f;

    if (_node.rotation.empty()) {
        key.value = vox::QuaternionF::makeIdentity();
    } else {
        key.value = vox::QuaternionF(static_cast<float>(_node.rotation[0]), static_cast<float>(_node.rotation[1]),
                                     static_cast<float>(_node.rotation[2]), static_cast<float>(_node.rotation[3]));
    }
    return key;
}

vox::animation::offline::RawAnimation::ScaleKey CreateScaleRestPoseKey(const tinygltf::Node& _node) {
    vox::animation::offline::RawAnimation::ScaleKey key;
    key.time = 0.0f;

    if (_node.scale.empty()) {
        key.value = vox::Vector3F::one();
    } else {
        key.value = vox::Vector3F(static_cast<float>(_node.scale[0]), static_cast<float>(_node.scale[1]),
                                  static_cast<float>(_node.scale[2]));
    }
    return key;
}

// Creates the default transform for a gltf node
bool CreateNodeTransform(const tinygltf::Node& _node, vox::ScalableTransform* _transform) {
    *_transform = vox::ScalableTransform::identity();

    if (!_node.matrix.empty()) {
        const vox::simd_math::Float4x4 matrix = {
                {vox::simd_math::simd_float4::Load(
                         static_cast<float>(_node.matrix[0]), static_cast<float>(_node.matrix[1]),
                         static_cast<float>(_node.matrix[2]), static_cast<float>(_node.matrix[3])),
                 vox::simd_math::simd_float4::Load(
                         static_cast<float>(_node.matrix[4]), static_cast<float>(_node.matrix[5]),
                         static_cast<float>(_node.matrix[6]), static_cast<float>(_node.matrix[7])),
                 vox::simd_math::simd_float4::Load(
                         static_cast<float>(_node.matrix[8]), static_cast<float>(_node.matrix[9]),
                         static_cast<float>(_node.matrix[10]), static_cast<float>(_node.matrix[11])),
                 vox::simd_math::simd_float4::Load(
                         static_cast<float>(_node.matrix[12]), static_cast<float>(_node.matrix[13]),
                         static_cast<float>(_node.matrix[14]), static_cast<float>(_node.matrix[15]))}};
        vox::simd_math::SimdFloat4 translation, rotation, scale;
        if (ToAffine(matrix, &translation, &rotation, &scale)) {
            vox::simd_math::Store3PtrU(translation, &_transform->translation.x);
            vox::simd_math::StorePtrU(rotation, &_transform->rotation.x);
            vox::simd_math::Store3PtrU(scale, &_transform->scale.x);
            return true;
        }

        LOGE("Failed to extract transformation from node {}.", _node.name)
        return false;
    }

    if (!_node.translation.empty()) {
        _transform->translation =
                vox::Vector3F(static_cast<float>(_node.translation[0]), static_cast<float>(_node.translation[1]),
                              static_cast<float>(_node.translation[2]));
    }
    if (!_node.rotation.empty()) {
        _transform->rotation =
                vox::QuaternionF(static_cast<float>(_node.rotation[0]), static_cast<float>(_node.rotation[1]),
                                 static_cast<float>(_node.rotation[2]), static_cast<float>(_node.rotation[3]));
    }
    if (!_node.scale.empty()) {
        _transform->scale = vox::Vector3F(static_cast<float>(_node.scale[0]), static_cast<float>(_node.scale[1]),
                                          static_cast<float>(_node.scale[2]));
    }

    return true;
}
}  // namespace

class GltfImporter : public vox::animation::offline::OzzImporter {
public:
    GltfImporter() {
        // We don't care about image data, but we have to provide this callback
        // because we're not loading the stb library
        auto image_loader = [](tinygltf::Image*, const int, std::string*, std::string*, int, int, const unsigned char*,
                               int, void*) { return true; };
        m_loader.SetImageLoader(image_loader, nullptr);
    }

private:
    bool Load(const char* _filename) override {
        bool success = false;
        std::string errors;
        std::string warnings;

        // Finds file extension.
        const char* separator = std::strrchr(_filename, '.');
        const char* ext = separator != nullptr ? separator + 1 : "";

        // Tries to guess whether the input is a gltf json or a glb binary based on
        // the file extension
        if (std::strcmp(ext, "glb") == 0) {
            success = m_loader.LoadBinaryFromFile(&m_model, &errors, &warnings, _filename);
        } else {
            if (std::strcmp(ext, "gltf") != 0) {
                LOGI("Unknown file extension {}, assuming a JSON-formatted gltf.", ext)
            }

            success = m_loader.LoadASCIIFromFile(&m_model, &errors, &warnings, _filename);
        }

        // Prints any errors or warnings emitted by the loader
        if (!warnings.empty()) {
            LOGW("glTF parsing warnings: {}", warnings)
        }

        if (!errors.empty()) {
            LOGE("glTF parsing errors: {}", errors)
        }

        if (success) {
            LOGI("glTF parsed successfully.")
        }

        if (success) {
            success &= FixupNames(m_model.scenes, "Scene", "scene_");
            success &= FixupNames(m_model.nodes, "Node", "node_");
            success &= FixupNames(m_model.animations, "Animation", "animation_");
        }

        return success;
    }

    // Find all unique root joints of skeletons used by given skins and add them
    // to `roots`
    void FindSkinRootJointIndices(const vox::vector<tinygltf::Skin>& skins, vox::vector<int>& roots) {
        static constexpr int no_parent = -1;
        static constexpr int visited = -2;
        vox::vector<int> parents(m_model.nodes.size(), no_parent);
        for (int node = 0; node < static_cast<int>(m_model.nodes.size()); node++) {
            for (int child : m_model.nodes[node].children) {
                parents[child] = node;
            }
        }

        for (const tinygltf::Skin& skin : skins) {
            if (skin.joints.empty()) {
                continue;
            }

            if (skin.skeleton != -1) {
                parents[skin.skeleton] = visited;
                roots.push_back(skin.skeleton);
                continue;
            }

            int root = skin.joints[0];
            while (root != visited && parents[root] != no_parent) {
                root = parents[root];
            }
            if (root != visited) {
                roots.push_back(root);
            }
        }
    }

    bool Import(vox::animation::offline::RawSkeleton* _skeleton, const NodeType& _types) override {
        (void)_types;

        if (m_model.scenes.empty()) {
            LOGE("No scenes found.")
            return false;
        }

        // If no default scene has been set then take the first one spec does not
        // disallow gltfs without a default scene, but it makes more sense to keep
        // going instead of throwing an error here
        int defaultScene = m_model.defaultScene;
        if (defaultScene == -1) {
            defaultScene = 0;
        }

        tinygltf::Scene& scene = m_model.scenes[defaultScene];
        LOGI("Importing from default scene #{} with name {}.", defaultScene, scene.name)

        if (scene.nodes.empty()) {
            LOGE("Scene has no node.")
            return false;
        }

        // Get all the skins belonging to this scene
        vox::vector<int> roots;
        vox::vector<tinygltf::Skin> skins = GetSkinsForScene(scene);
        if (skins.empty()) {
            LOGI("No skin exists in the scene, the whole scene graph "
                 "will be considered as a skeleton.")
            // Uses all scene nodes.
            for (auto& node : scene.nodes) {
                roots.push_back(node);
            }
        } else {
            if (skins.size() > 1) {
                LOGI("Multiple skins exist in the scene, they will all "
                     "be exported to a single skeleton.")
            }

            // Uses all skins roots.
            FindSkinRootJointIndices(skins, roots);
        }

        // Remove nodes listed multiple times.
        std::sort(roots.begin(), roots.end());
        roots.erase(std::unique(roots.begin(), roots.end()), roots.end());

        // Traverses the scene graph and record all joints starting from the roots.
        _skeleton->roots.resize(roots.size());
        for (size_t i = 0; i < roots.size(); ++i) {
            const tinygltf::Node& root_node = m_model.nodes[roots[i]];
            vox::animation::offline::RawSkeleton::Joint& root_joint = _skeleton->roots[i];
            if (!ImportNode(root_node, &root_joint)) {
                return false;
            }
        }

        if (!_skeleton->Validate()) {
            LOGE("Output skeleton failed validation. This is likely an implementation issue.")
            return false;
        }

        return true;
    }

    // Recursively import a node's children
    bool ImportNode(const tinygltf::Node& _node, vox::animation::offline::RawSkeleton::Joint* _joint) {
        // Names joint.
        _joint->name = _node.name;

        // Fills transform.
        if (!CreateNodeTransform(_node, &_joint->transform)) {
            return false;
        }

        // Allocates all children at once.
        _joint->children.resize(_node.children.size());

        // Fills each child information.
        for (size_t i = 0; i < _node.children.size(); ++i) {
            const tinygltf::Node& child_node = m_model.nodes[_node.children[i]];
            vox::animation::offline::RawSkeleton::Joint& child_joint = _joint->children[i];

            if (!ImportNode(child_node, &child_joint)) {
                return false;
            }
        }

        return true;
    }

    // Returns all animations in the gltf document.
    AnimationNames GetAnimationNames() override {
        AnimationNames animNames;
        for (auto& animation : m_model.animations) {
            assert(animation.name.length() != 0);
            animNames.push_back(animation.name.c_str());
        }

        return animNames;
    }

    bool Import(const char* _animation_name,
                const vox::animation::Skeleton& skeleton,
                float _sampling_rate,
                vox::animation::offline::RawAnimation* _animation) override {
        if (_sampling_rate == 0.0f) {
            _sampling_rate = 30.0f;

            static bool samplingRateWarn = false;
            if (!samplingRateWarn) {
                LOGI("The animation sampling rate is set to 0 "
                     "(automatic) but glTF does not carry scene frame "
                     "rate information. Assuming a sampling rate of {} hz.",
                     _sampling_rate)

                samplingRateWarn = true;
            }
        }

        // Find the corresponding gltf animation
        auto gltf_animation = std::find_if(begin(m_model.animations), end(m_model.animations),
                                           [_animation_name](const tinygltf::Animation& _animation) {
                                               return _animation.name == _animation_name;
                                           });
        assert(gltf_animation != end(m_model.animations));

        _animation->name = gltf_animation->name;

        // Animation duration is determined during sampling from the duration of the
        // longest channel
        _animation->duration = 0.0f;

        const int num_joints = skeleton.num_joints();
        _animation->tracks.resize(num_joints);

        // gltf stores animations by splitting them in channels
        // where each channel targets a node's property i.e. translation, rotation
        // or scale. vox expects animations to be stored per joint, so we create a
        // map where we record the associated channels for each joint
        vox::cstring_map<std::vector<const tinygltf::AnimationChannel*>> channels_per_joint;

        for (const tinygltf::AnimationChannel& channel : gltf_animation->channels) {
            // Reject if no node is targetted.
            if (channel.target_node == -1) {
                continue;
            }

            // Reject if path isn't about skeleton animation.
            bool valid_target = false;
            for (const char* path : {"translation", "rotation", "scale"}) {
                valid_target |= channel.target_path == path;
            }
            if (!valid_target) {
                continue;
            }

            const tinygltf::Node& target_node = m_model.nodes[channel.target_node];
            channels_per_joint[target_node.name.c_str()].push_back(&channel);
        }

        // For each joint get all its associated channels, sample them and record
        // the samples in the joint track
        const vox::span<const char* const> joint_names = skeleton.joint_names();
        for (int i = 0; i < num_joints; i++) {
            auto& channels = channels_per_joint[joint_names[i]];
            auto& track = _animation->tracks[i];

            for (auto& channel : channels) {
                auto& sampler = gltf_animation->samplers[channel->sampler];
                if (!SampleAnimationChannel(m_model, sampler, channel->target_path, _sampling_rate,
                                            &_animation->duration, &track)) {
                    return false;
                }
            }

            const tinygltf::Node* node = FindNodeByName(joint_names[i]);
            assert(node != nullptr);

            // Pads the rest pose transform for any joints which do not have an
            // associated channel for this animation
            if (track.translations.empty()) {
                track.translations.push_back(CreateTranslationRestPoseKey(*node));
            }
            if (track.rotations.empty()) {
                track.rotations.push_back(CreateRotationRestPoseKey(*node));
            }
            if (track.scales.empty()) {
                track.scales.push_back(CreateScaleRestPoseKey(*node));
            }
        }

        LOGI("Processed animation '{}' (tracks: {}, duration: {}s).", _animation->name, _animation->tracks.size(),
             _animation->duration)

        if (!_animation->Validate()) {
            LOGE("Animation '{}' failed validation.", _animation->name)
            return false;
        }

        return true;
    }

    bool SampleAnimationChannel(const tinygltf::Model& _model,
                                const tinygltf::AnimationSampler& _sampler,
                                const std::string& _target_path,
                                float _sampling_rate,
                                float* _duration,
                                vox::animation::offline::RawAnimation::JointTrack* _track) {
        // Validate interpolation type.
        if (_sampler.interpolation.empty()) {
            LOGE("Invalid sampler interpolation.")
            return false;
        }

        auto& input = m_model.accessors[_sampler.input];
        assert(input.maxValues.size() == 1);

        // The max[0] property of the input accessor is the animation duration
        // this is required to be present by the spec:
        // "Animation Sampler's input accessor must have min and max properties
        // defined."
        const auto duration = static_cast<float>(input.maxValues[0]);

        // If this channel's duration is larger than the animation's duration
        // then increase the animation duration to match.
        if (duration > *_duration) {
            *_duration = duration;
        }

        assert(input.type == TINYGLTF_TYPE_SCALAR);
        auto& _output = m_model.accessors[_sampler.output];
        assert(_output.type == TINYGLTF_TYPE_VEC3 || _output.type == TINYGLTF_TYPE_VEC4);

        const vox::span<const float> timestamps = BufferView<float>(_model, input);
        if (timestamps.empty()) {
            return true;
        }

        // Builds keyframes.
        bool valid = false;
        if (_target_path == "translation") {
            valid = SampleChannel(m_model, _sampler.interpolation, _output, timestamps, _sampling_rate, duration,
                                  &_track->translations);
        } else if (_target_path == "rotation") {
            valid = SampleChannel(m_model, _sampler.interpolation, _output, timestamps, _sampling_rate, duration,
                                  &_track->rotations);
            if (valid) {
                // Normalize quaternions.
                for (auto& key : _track->rotations) {
                    key.value = key.value.normalized();
                }
            }
        } else if (_target_path == "scale") {
            valid = SampleChannel(m_model, _sampler.interpolation, _output, timestamps, _sampling_rate, duration,
                                  &_track->scales);
        } else {
            assert(false && "Invalid target path");
        }

        return valid;
    }

    // Returns all skins belonging to a given gltf scene
    [[nodiscard]] vox::vector<tinygltf::Skin> GetSkinsForScene(const tinygltf::Scene& _scene) const {
        vox::set<int> open;
        vox::set<int> found;

        for (int nodeIndex : _scene.nodes) {
            open.insert(nodeIndex);
        }

        while (!open.empty()) {
            int nodeIndex = *open.begin();
            found.insert(nodeIndex);
            open.erase(nodeIndex);

            auto& node = m_model.nodes[nodeIndex];
            for (int childIndex : node.children) {
                open.insert(childIndex);
            }
        }

        vox::vector<tinygltf::Skin> skins;
        for (const tinygltf::Skin& skin : m_model.skins) {
            if (!skin.joints.empty() && found.find(skin.joints[0]) != found.end()) {
                skins.push_back(skin);
            }
        }

        return skins;
    }

    [[nodiscard]] const tinygltf::Node* FindNodeByName(const std::string& _name) const {
        for (const tinygltf::Node& node : m_model.nodes) {
            if (node.name == _name) {
                return &node;
            }
        }

        return nullptr;
    }

    // no support for user-defined tracks
    NodeProperties GetNodeProperties(const char*) override { return {}; }
    bool Import(const char*,
                const char*,
                const char*,
                NodeProperty::Type,
                float,
                vox::animation::offline::RawFloatTrack*) override {
        return false;
    }
    bool Import(const char*,
                const char*,
                const char*,
                NodeProperty::Type,
                float,
                vox::animation::offline::RawFloat2Track*) override {
        return false;
    }
    bool Import(const char*,
                const char*,
                const char*,
                NodeProperty::Type,
                float,
                vox::animation::offline::RawFloat3Track*) override {
        return false;
    }
    bool Import(const char*,
                const char*,
                const char*,
                NodeProperty::Type,
                float,
                vox::animation::offline::RawFloat4Track*) override {
        return false;
    }

    tinygltf::TinyGLTF m_loader;
    tinygltf::Model m_model;
};

int main(int _argc, const char** _argv) {
    GltfImporter converter;
    return converter(_argc, _argv);
}
