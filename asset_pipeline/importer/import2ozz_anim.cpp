//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_pipeline/importer/import2ozz_anim.h"

#include <json/json.h>

#include "asset_pipeline/importer/import2ozz_track.h"
#include "vox.animation/offline/additive_animation_builder.h"
#include "vox.animation/offline/animation_builder.h"
#include "vox.animation/offline/animation_optimizer.h"
#include "vox.animation/offline/skeleton_builder.h"
#include "vox.animation/runtime/animation.h"
#include "vox.animation/runtime/skeleton.h"
#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.base/string_utils.h"
#include "vox.simd_math/soa_transform.h"

namespace vox::animation::offline {
namespace {

void DisplaysOptimizationStatistics(const RawAnimation& _non_optimized, const RawAnimation& _optimized) {
    size_t opt_translations = 0, opt_rotations = 0, opt_scales = 0;
    for (const auto& track : _optimized.tracks) {
        opt_translations += track.translations.size();
        opt_rotations += track.rotations.size();
        opt_scales += track.scales.size();
    }
    size_t non_opt_translations = 0, non_opt_rotations = 0, non_opt_scales = 0;
    for (const auto& track : _non_optimized.tracks) {
        non_opt_translations += track.translations.size();
        non_opt_rotations += track.rotations.size();
        non_opt_scales += track.scales.size();
    }

    // Computes optimization ratios.
    float translation_ratio = opt_translations != 0 ? 1.f * non_opt_translations / opt_translations : 0.f;
    float rotation_ratio = opt_rotations != 0 ? 1.f * non_opt_rotations / opt_rotations : 0.f;
    float scale_ratio = opt_scales != 0 ? 1.f * non_opt_scales / opt_scales : 0.f;

    LOGI("Optimization stage results:")
    LOGI(" - Translations: {}:1", translation_ratio)
    LOGI(" - Rotations: {}:1", rotation_ratio)
    LOGI(" - Scales: {}:1", scale_ratio)
}

unique_ptr<vox::animation::Skeleton> LoadSkeleton(const char* _path) {
    // Reads the skeleton from the binary vox stream.
    unique_ptr<vox::animation::Skeleton> skeleton;
    {
        if (*_path == 0) {
            LOGE("Missing input skeleton file from json config.")
            return nullptr;
        }
        LOGI("Opens input skeleton vox binary file: {}", _path)
        vox::io::File file(_path, "rb");
        if (!file.opened()) {
            LOGE("Failed to open input skeleton vox binary file: {}", _path)
            return nullptr;
        }
        vox::io::IArchive archive(&file);

        // File could contain a RawSkeleton or a Skeleton.
        if (archive.TestTag<RawSkeleton>()) {
            LOGI("Reading RawSkeleton from file.")

            // Reading the skeleton cannot file.
            RawSkeleton raw_skeleton;
            archive >> raw_skeleton;

            // Builds runtime skeleton.
            LOGI("Builds runtime skeleton.")
            SkeletonBuilder builder;
            skeleton = builder(raw_skeleton);
            if (!skeleton) {
                LOGE("Failed to build runtime skeleton.")
                return nullptr;
            }
        } else if (archive.TestTag<Skeleton>()) {
            // Reads input archive to the runtime skeleton.
            // This operation cannot fail.
            skeleton = make_unique<Skeleton>();
            archive >> *skeleton;
        } else {
            LOGE("Failed to read input skeleton from binary file: {}.", _path)
            return nullptr;
        }
    }
    return skeleton;
}

vector<ScalableTransform> SkeletonRestPoseSoAToAoS(const Skeleton& _skeleton) {
    // Copy skeleton rest pose to AoS form.
    vector<ScalableTransform> transforms(_skeleton.num_joints());
    for (int i = 0; i < _skeleton.num_soa_joints(); ++i) {
        const simd_math::SoaTransform& soa_transform = _skeleton.joint_rest_poses()[i];
        simd_math::SimdFloat4 translation[4];
        simd_math::SimdFloat4 rotation[4];
        simd_math::SimdFloat4 scale[4];
        simd_math::Transpose3x4(&soa_transform.translation.x, translation);
        simd_math::Transpose4x4(&soa_transform.rotation.x, rotation);
        simd_math::Transpose3x4(&soa_transform.scale.x, scale);
        for (int j = 0; j < 4 && i * 4 + j < _skeleton.num_joints(); ++j) {
            ScalableTransform& out = transforms[i * 4 + j];
            simd_math::Store3PtrU(translation[j], &out.translation.x);
            simd_math::StorePtrU(rotation[j], &out.rotation.x);
            simd_math::Store3PtrU(scale[j], &out.scale.x);
        }
    }
    return transforms;
}

bool Export(OzzImporter& _importer,
            const RawAnimation& _input_animation,
            const Skeleton& _skeleton,
            const Json::Value& _config,
            const vox::Endianness _endianness) {
    // Raw animation to build and output. Initial setup is just a copy.
    RawAnimation raw_animation = _input_animation;

    // Optimizes animation if option is enabled.
    // Must be done before converting to additive, to be sure hierarchy length is
    // valid when optimizing.
    if (_config["optimize"].asBool()) {
        LOGI("Optimizing animation.")
        AnimationOptimizer optimizer;

        // Setup optimizer from config parameters.
        const Json::Value& tolerances = _config["optimization_settings"];
        optimizer.setting.tolerance = tolerances["tolerance"].asFloat();
        optimizer.setting.distance = tolerances["distance"].asFloat();

        // Builds per joint settings.
        const Json::Value& joints_config = tolerances["override"];
        for (const auto& joint_config : joints_config) {
            // Prepares setting.
            AnimationOptimizer::Setting setting;
            setting.tolerance = joint_config["tolerance"].asFloat();
            setting.distance = joint_config["distance"].asFloat();

            // Push it for all matching joints.
            // Settings are overwritten if one has already been pushed.
            bool found = false;
            const char* name_pattern = joint_config["name"].asCString();
            for (int j = 0; j < _skeleton.num_joints(); ++j) {
                const char* joint_name = _skeleton.joint_names()[j];
                if (strmatch(joint_name, name_pattern)) {
                    found = true;
                    LOGI("Found joint {} matching pattern {} for joint optimization setting override.", joint_name,
                         name_pattern)
                    const AnimationOptimizer::JointsSetting::value_type entry(j, setting);
                    const bool newly = optimizer.joints_setting_override.insert(entry).second;
                    if (!newly) {
                        LOGI("Redundant optimization setting for pattern {}", name_pattern)
                    }
                }
            }

            if (!found) {
                LOGI("No joint found for optimization setting for pattern {}", name_pattern)
            }
        }

        RawAnimation raw_optimized_animation;
        if (!optimizer(raw_animation, _skeleton, &raw_optimized_animation)) {
            LOGE("Failed to optimize animation.")
            return false;
        }

        // Displays optimization statistics.
        DisplaysOptimizationStatistics(raw_animation, raw_optimized_animation);

        // Brings data back to the raw animation.
        raw_animation = raw_optimized_animation;
    } else {
        LOGI("Optimization for animation {} is disabled.", _input_animation.name)
    }

    // Make delta animation if requested.
    if (_config["additive"].asBool()) {
        LOGI("Makes additive animation.")

        AdditiveAnimationBuilder additive_builder;
        RawAnimation raw_additive;

        AdditiveReferenceEnum::Value reference;
        bool enum_found = AdditiveReference::GetEnumFromName(_config["additive_reference"].asCString(), &reference);
        assert(enum_found);  // Already checked on config side.

        bool succeeded = false;
        if (enum_found && reference == AdditiveReferenceEnum::kSkeleton) {
            const vector<ScalableTransform> transforms = SkeletonRestPoseSoAToAoS(_skeleton);
            succeeded = additive_builder(raw_animation, make_span(transforms), &raw_additive);
        } else {
            succeeded = additive_builder(raw_animation, &raw_additive);
        }

        if (!succeeded) {
            LOGE("Failed to make additive animation.")
            return false;
        }

        // Now use additive animation.
        raw_animation = raw_additive;
    }

    // Builds runtime animation.
    unique_ptr<Animation> animation;
    if (!_config["raw"].asBool()) {
        LOGI("Builds runtime animation.")
        AnimationBuilder builder;
        animation = builder(raw_animation);
        if (!animation) {
            LOGE("Failed to build runtime animation.")
            return false;
        }
    }

    {
        // Prepares output stream. File is a RAII, so it will close automatically
        // at the end of this scope. Once the file is opened, nothing should fail
        // as it would leave an invalid file on the disk.

        // Builds output filename.
        vox::string filename = _importer.BuildFilename(_config["filename"].asCString(), raw_animation.name.c_str());

        LOGI("Opens output file: {}", filename)
        vox::io::File file(filename.c_str(), "wb");
        if (!file.opened()) {
            LOGE("Failed to open output file: {}", filename)
            return false;
        }

        // Initializes output archive.
        vox::io::OArchive archive(&file, _endianness);

        // Fills output archive with the animation.
        if (_config["raw"].asBool()) {
            LOGI("Outputs RawAnimation to binary archive.")
            archive << raw_animation;
        } else {
            LOGI("Outputs Animation to binary archive.")
            archive << *animation;
        }
    }

    LOGI("Animation binary archive successfully outputted.")

    return true;
}  // namespace

bool ProcessAnimation(OzzImporter& _importer,
                      const char* _animation_name,
                      const Skeleton& _skeleton,
                      const Json::Value& _config,
                      const vox::Endianness _endianness) {
    RawAnimation animation;

    LOGI("Extracting animation {}", _animation_name)

    if (!_importer.Import(_animation_name, _skeleton, _config["sampling_rate"].asFloat(), &animation)) {
        LOGE("Failed to import animation {}", _animation_name)
        return false;
    } else {
        // Give animation a name
        animation.name = _animation_name;

        return Export(_importer, animation, _skeleton, _config, _endianness);
    }
}
}  // namespace

AdditiveReference::EnumNames AdditiveReference::GetNames() {
    static const char* kNames[] = {"animation", "skeleton"};
    const EnumNames enum_names = {VOX_ARRAY_SIZE(kNames), kNames};
    return enum_names;
}

bool ImportAnimations(const Json::Value& _config, OzzImporter* _importer, const vox::Endianness _endianness) {
    const Json::Value& skeleton_config = _config["skeleton"];
    const Json::Value& animations_config = _config["animations"];

    if (animations_config.empty()) {
        LOGI("Configuration contains no animation import definition, animations import will be skipped.")
        return true;
    }

    // Get all available animation names.
    const OzzImporter::AnimationNames& import_animation_names = _importer->GetAnimationNames();

    // Are their animations available
    if (import_animation_names.empty()) {
        LOGE("No animation found.")
        return true;
    }

    // Iterates all imported animations, build and output them.
    bool success = true;

    // Import skeleton instance.
    unique_ptr<Skeleton> skeleton(LoadSkeleton(skeleton_config["filename"].asCString()));
    success &= skeleton.get() != nullptr;

    if (!success) return false;

    // Loop thought all existing animations, and export those who match
    // configuration.
    for (Json::ArrayIndex i = 0; i < animations_config.size(); ++i) {
        const Json::Value& animation_config = animations_config[i];
        const char* clip_match = animation_config["clip"].asCString();

        if (*clip_match == 0) {
            LOGI("No clip name provided. Animation import will be skipped.")
            continue;
        }

        size_t num_not_clip_animation = 0, num_valid_animation = 0;
        for (const auto& import_animation_name : import_animation_names) {
            const char* animation_name = import_animation_name.c_str();
            if (!strmatch(animation_name, clip_match)) {
                continue;
            }
            ++num_not_clip_animation;
            const bool anisuccess =
                    ProcessAnimation(*_importer, animation_name, *skeleton, animation_config, _endianness);
            if (anisuccess) {
                ++num_valid_animation;
            }

            size_t num_valid_track = 0;
            const Json::Value& tracks_config = animation_config["tracks"];
            for (const auto& t : tracks_config) {
                if (ProcessTracks(*_importer, animation_name, *skeleton, t, _endianness)) {
                    ++num_valid_track;
                }
            }

            if (num_valid_track != tracks_config.size()) {
                LOGI("One of track failed when import: {}", animation_name)
                success = false;
            }
        }
        // Don't display any message if no animation is supposed to be imported.
        if (0 == num_not_clip_animation && *clip_match != 0) {
            LOGI("No matching animation found for {}", clip_match)
        }

        if (num_valid_animation != num_not_clip_animation) {
            LOGI("One of animation failed when import, animation index: {}", i)
            success = false;
        }
    }

    return success;
}
}  // namespace vox::animation::offline
