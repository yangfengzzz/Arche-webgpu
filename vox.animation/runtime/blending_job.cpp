//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/runtime/blending_job.h"

#include <cassert>

#include "vox.animation/runtime/skeleton.h"
#include "vox.simd_math/soa_transform.h"

namespace vox::animation {

BlendingJob::Layer::Layer() : weight(0.f) {}

BlendingJob::BlendingJob() : threshold(.1f) {}

namespace {
bool ValidateLayer(const BlendingJob::Layer& _layer, size_t _min_range) {
    bool valid = true;

    // Tests transforms validity.
    valid &= _layer.transform.size() >= _min_range;

    // Joint weights are optional.
    if (!_layer.joint_weights.empty()) {
        valid &= _layer.joint_weights.size() >= _min_range;
    } else {
        valid &= _layer.joint_weights.empty();
    }
    return valid;
}
}  // namespace

bool BlendingJob::Validate() const {
    // Don't need any early out, as jobs are valid in most of the performance
    // critical cases.
    // Tests are written in multiple lines in order to avoid branches.
    bool valid = true;

    // Test for valid threshold).
    valid &= threshold > 0.f;

    // Test for nullptr begin pointers.
    // Blending layers are mandatory, additive aren't.
    valid &= !rest_pose.empty();
    valid &= !output.empty();

    // The rest pose size defines the ranges of transforms to blend, so all
    // other buffers should be bigger.
    const size_t min_range = rest_pose.size();
    valid &= output.size() >= min_range;

    // Validates layers.
    for (const Layer& layer : layers) {
        valid &= ValidateLayer(layer, min_range);
    }

    // Validates additive layers.
    for (const Layer& layer : additive_layers) {
        valid &= ValidateLayer(layer, min_range);
    }

    return valid;
}

namespace {

// Macro that defines the process of blending the 1st pass.
#define VOX_BLEND_1ST_PASS(_in, _simd_weight, _out)         \
    do {                                                    \
        _out->translation = _in.translation * _simd_weight; \
        _out->rotation = _in.rotation * _simd_weight;       \
        _out->scale = _in.scale * _simd_weight;             \
    } while (void(0), 0)

// Macro that defines the process of blending any pass but the first.
#define VOX_BLEND_N_PASS(_in, _simd_weight, _out)                                            \
    do {                                                                                     \
        /* Blends translation. */                                                            \
        _out->translation = _out->translation + _in.translation * _simd_weight;              \
        /* Blends rotations, negates opposed quaternions to be sure to choose*/              \
        /* the shortest path between the two.*/                                              \
        const simd_math::SimdInt4 sign = simd_math::Sign(Dot(_out->rotation, _in.rotation)); \
        const simd_math::SoaQuaternion rotation = {                                          \
                simd_math::Xor(_in.rotation.x, sign), simd_math::Xor(_in.rotation.y, sign),  \
                simd_math::Xor(_in.rotation.z, sign), simd_math::Xor(_in.rotation.w, sign)}; \
        _out->rotation = _out->rotation + rotation * _simd_weight;                           \
        /* Blends scales.*/                                                                  \
        _out->scale = _out->scale + _in.scale * _simd_weight;                                \
    } while (void(0), 0)

// Macro that defines the process of adding a pass.
#define VOX_ADD_PASS(_in, _simd_weight, _out)                                                               \
    do {                                                                                                    \
        _out.translation = _out.translation + _in.translation * _simd_weight;                               \
        /* Interpolate quaternion between identity and src.rotation.*/                                      \
        /* Quaternion sign is fixed up, so that lerp takes the shortest path.*/                             \
        const simd_math::SimdInt4 sign = simd_math::Sign(_in.rotation.w);                                   \
        const simd_math::SoaQuaternion rotation = {                                                         \
                simd_math::Xor(_in.rotation.x, sign), simd_math::Xor(_in.rotation.y, sign),                 \
                simd_math::Xor(_in.rotation.z, sign), simd_math::Xor(_in.rotation.w, sign)};                \
        const simd_math::SoaQuaternion interp_quat = {rotation.x * _simd_weight, rotation.y * _simd_weight, \
                                                      rotation.z * _simd_weight,                            \
                                                      (rotation.w - one) * _simd_weight + one};             \
        _out.rotation = NormalizeEst(interp_quat) * _out.rotation;                                          \
        _out.scale = _out.scale * (one_minus_weight_f3 + (_in.scale * _simd_weight));                       \
    } while (void(0), 0)

// Macro that defines the process of subtracting a pass.
#define VOX_SUB_PASS(_in, _simd_weight, _out)                                                               \
    do {                                                                                                    \
        _out.translation = _out.translation - _in.translation * _simd_weight;                               \
        /* Interpolate quaternion between identity and src.rotation.*/                                      \
        /* Quaternion sign is fixed up, so that lerp takes the shortest path.*/                             \
        const simd_math::SimdInt4 sign = simd_math::Sign(_in.rotation.w);                                   \
        const simd_math::SoaQuaternion rotation = {                                                         \
                simd_math::Xor(_in.rotation.x, sign), simd_math::Xor(_in.rotation.y, sign),                 \
                simd_math::Xor(_in.rotation.z, sign), simd_math::Xor(_in.rotation.w, sign)};                \
        const simd_math::SoaQuaternion interp_quat = {rotation.x * _simd_weight, rotation.y * _simd_weight, \
                                                      rotation.z * _simd_weight,                            \
                                                      (rotation.w - one) * _simd_weight + one};             \
        _out.rotation = Conjugate(NormalizeEst(interp_quat)) * _out.rotation;                               \
        const simd_math::SoaFloat3 rcp_scale = {                                                            \
                simd_math::RcpEst(simd_math::MAdd(_in.scale.x, _simd_weight, one_minus_weight)),            \
                simd_math::RcpEst(simd_math::MAdd(_in.scale.y, _simd_weight, one_minus_weight)),            \
                simd_math::RcpEst(simd_math::MAdd(_in.scale.z, _simd_weight, one_minus_weight))};           \
        _out.scale = _out.scale * rcp_scale;                                                                \
    } while (void(0), 0)

// Defines parameters that are passed through blending stages.
struct ProcessArgs {
    ProcessArgs(const BlendingJob& _job)
        : job(_job),
          num_soa_joints(_job.rest_pose.size()),
          num_passes(0),
          num_partial_passes(0),
          accumulated_weight(0.f) {
        // The range of all buffers has already been validated.
        assert(job.output.size() >= num_soa_joints);
        assert(VOX_ARRAY_SIZE(accumulated_weights) >= num_soa_joints);
    }

    // Allocates enough space to store a accumulated weights per-joint.
    // It will be initialized by the first pass processed, if any.
    // This is quite big for a stack allocation (4 byte * maximum number of
    // joints). This is one of the reasons why the number of joints is limited
    // by the API.
    // Note that this array is used with SoA data.
    // This is the first argument in order to avoid wasting too much space with
    // alignment padding.
    simd_math::SimdFloat4 accumulated_weights[Skeleton::kMaxSoAJoints]{};

    // The job to process.
    const BlendingJob& job;

    // The number of transforms to process as defined by the size of the rest
    // pose.
    size_t num_soa_joints;

    // Number of processed blended passes (excluding passes with a weight <= 0.f),
    // including partial passes.
    int num_passes;

    // Number of processed partial blending passes (aka with a weight per-joint).
    int num_partial_passes;

    // The accumulated weight of all layers.
    float accumulated_weight;

private:
    // Disables assignment operators.
    ProcessArgs(const ProcessArgs&);
    void operator=(const ProcessArgs&);
};

// Blends all layers of the job to its output.
void BlendLayers(ProcessArgs* _args) {
    assert(_args);

    // Iterates through all layers and blend them to the output.
    for (const BlendingJob::Layer& layer : _args->job.layers) {
        // Asserts buffer sizes, which must never fail as it has been validated.
        assert(layer.transform.size() >= _args->num_soa_joints);
        assert(layer.joint_weights.empty() || (layer.joint_weights.size() >= _args->num_soa_joints));

        // Skip irrelevant layers.
        if (layer.weight <= 0.f) {
            continue;
        }

        // Accumulates global weights.
        _args->accumulated_weight += layer.weight;
        const simd_math::SimdFloat4 layer_weight = simd_math::simd_float4::Load1(layer.weight);

        if (!layer.joint_weights.empty()) {
            // This layer has per-joint weights.
            ++_args->num_partial_passes;

            if (_args->num_passes == 0) {
                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    const simd_math::SoaTransform& src = layer.transform[i];
                    simd_math::SoaTransform* dest = _args->job.output.begin() + i;
                    const simd_math::SimdFloat4 weight = layer_weight * simd_math::Max0(layer.joint_weights[i]);
                    _args->accumulated_weights[i] = weight;
                    VOX_BLEND_1ST_PASS(src, weight, dest);
                }
            } else {
                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    const simd_math::SoaTransform& src = layer.transform[i];
                    simd_math::SoaTransform* dest = _args->job.output.begin() + i;
                    const simd_math::SimdFloat4 weight = layer_weight * simd_math::Max0(layer.joint_weights[i]);
                    _args->accumulated_weights[i] = _args->accumulated_weights[i] + weight;
                    VOX_BLEND_N_PASS(src, weight, dest);
                }
            }
        } else {
            // This is a full layer.
            if (_args->num_passes == 0) {
                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    const simd_math::SoaTransform& src = layer.transform[i];
                    simd_math::SoaTransform* dest = _args->job.output.begin() + i;
                    _args->accumulated_weights[i] = layer_weight;
                    VOX_BLEND_1ST_PASS(src, layer_weight, dest);
                }
            } else {
                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    const simd_math::SoaTransform& src = layer.transform[i];
                    simd_math::SoaTransform* dest = _args->job.output.begin() + i;
                    _args->accumulated_weights[i] = _args->accumulated_weights[i] + layer_weight;
                    VOX_BLEND_N_PASS(src, layer_weight, dest);
                }
            }
        }
        // One more pass blended.
        ++_args->num_passes;
    }
}

// Blends rest pose to the output if accumulated weight is less than the
// threshold value.
void BlendRestPose(ProcessArgs* _args) {
    assert(_args);

    // Asserts buffer sizes, which must never fail as it has been validated.
    assert(_args->job.rest_pose.size() >= _args->num_soa_joints);

    if (_args->num_partial_passes == 0) {
        // No partial blending pass detected, threshold can be tested globally.
        const float bp_weight = _args->job.threshold - _args->accumulated_weight;

        if (bp_weight > 0.f) {  // The rest-pose is needed if it has a weight.
            if (_args->num_passes == 0) {
                // Strictly copying rest-pose.
                _args->accumulated_weight = 1.f;
                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    _args->job.output[i] = _args->job.rest_pose[i];
                }
            } else {
                // Updates global accumulated weight, but not per-joint weight any more
                // because normalization stage will be global also.
                _args->accumulated_weight = _args->job.threshold;

                const simd_math::SimdFloat4 simd_bp_weight = simd_math::simd_float4::Load1(bp_weight);

                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    const simd_math::SoaTransform& src = _args->job.rest_pose[i];
                    simd_math::SoaTransform* dest = _args->job.output.begin() + i;
                    VOX_BLEND_N_PASS(src, simd_bp_weight, dest);
                }
            }
        }
    } else {
        // Blending passes contain partial blending, threshold must be tested for
        // each joint.
        const simd_math::SimdFloat4 threshold = simd_math::simd_float4::Load1(_args->job.threshold);

        // There's been at least 1 pass as num_partial_passes != 0.
        assert(_args->num_passes != 0);

        for (size_t i = 0; i < _args->num_soa_joints; ++i) {
            const simd_math::SoaTransform& src = _args->job.rest_pose[i];
            simd_math::SoaTransform* dest = _args->job.output.begin() + i;
            const simd_math::SimdFloat4 bp_weight = simd_math::Max0(threshold - _args->accumulated_weights[i]);
            _args->accumulated_weights[i] = simd_math::Max(threshold, _args->accumulated_weights[i]);
            VOX_BLEND_N_PASS(src, bp_weight, dest);
        }
    }
}

// Normalizes output rotations. Quaternion length cannot be zero as opposed
// quaternions have been fixed up during blending passes.
// Translations and scales are already normalized because weights were
// pre-multiplied by the normalization ratio.
void Normalize(ProcessArgs* _args) {
    assert(_args);

    if (_args->num_partial_passes == 0) {
        // Normalization of a non-partial blending requires to apply the same
        // division to all joints.
        const simd_math::SimdFloat4 ratio = simd_math::simd_float4::Load1(1.f / _args->accumulated_weight);
        for (size_t i = 0; i < _args->num_soa_joints; ++i) {
            simd_math::SoaTransform& dest = _args->job.output[i];
            dest.rotation = NormalizeEst(dest.rotation);
            dest.translation = dest.translation * ratio;
            dest.scale = dest.scale * ratio;
        }
    } else {
        // Partial blending normalization requires to compute the divider per-joint.
        const simd_math::SimdFloat4 one = simd_math::simd_float4::one();
        for (size_t i = 0; i < _args->num_soa_joints; ++i) {
            const simd_math::SimdFloat4 ratio = one / _args->accumulated_weights[i];
            simd_math::SoaTransform& dest = _args->job.output[i];
            dest.rotation = NormalizeEst(dest.rotation);
            dest.translation = dest.translation * ratio;
            dest.scale = dest.scale * ratio;
        }
    }
}

// Process additive blending pass.
void AddLayers(ProcessArgs* _args) {
    assert(_args);

    // Iterates through all layers and blend them to the output.
    for (const BlendingJob::Layer& layer : _args->job.additive_layers) {
        // Asserts buffer sizes, which must never fail as it has been validated.
        assert(layer.transform.size() >= _args->num_soa_joints);
        assert(layer.joint_weights.empty() || (layer.joint_weights.size() >= _args->num_soa_joints));

        // Prepares constants.
        const simd_math::SimdFloat4 one = simd_math::simd_float4::one();

        if (layer.weight > 0.f) {
            // Weight is positive, need to perform additive blending.
            const simd_math::SimdFloat4 layer_weight = simd_math::simd_float4::Load1(layer.weight);

            if (!layer.joint_weights.empty()) {
                // This layer has per-joint weights.
                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    const simd_math::SoaTransform& src = layer.transform[i];
                    simd_math::SoaTransform& dest = _args->job.output[i];
                    const simd_math::SimdFloat4 weight = layer_weight * simd_math::Max0(layer.joint_weights[i]);
                    const simd_math::SimdFloat4 one_minus_weight = one - weight;
                    const simd_math::SoaFloat3 one_minus_weight_f3 = {one_minus_weight, one_minus_weight,
                                                                      one_minus_weight};
                    VOX_ADD_PASS(src, weight, dest);
                }
            } else {
                // This is a full layer.
                const simd_math::SimdFloat4 one_minus_weight = one - layer_weight;
                const simd_math::SoaFloat3 one_minus_weight_f3 = {one_minus_weight, one_minus_weight, one_minus_weight};

                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    const simd_math::SoaTransform& src = layer.transform[i];
                    simd_math::SoaTransform& dest = _args->job.output[i];
                    VOX_ADD_PASS(src, layer_weight, dest);
                }
            }
        } else if (layer.weight < 0.f) {
            // Weight is negative, need to perform subtractive blending.
            const simd_math::SimdFloat4 layer_weight = simd_math::simd_float4::Load1(-layer.weight);

            if (!layer.joint_weights.empty()) {
                // This layer has per-joint weights.
                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    const simd_math::SoaTransform& src = layer.transform[i];
                    simd_math::SoaTransform& dest = _args->job.output[i];
                    const simd_math::SimdFloat4 weight = layer_weight * simd_math::Max0(layer.joint_weights[i]);
                    const simd_math::SimdFloat4 one_minus_weight = one - weight;
                    VOX_SUB_PASS(src, weight, dest);
                }
            } else {
                // This is a full layer.
                const simd_math::SimdFloat4 one_minus_weight = one - layer_weight;
                for (size_t i = 0; i < _args->num_soa_joints; ++i) {
                    const simd_math::SoaTransform& src = layer.transform[i];
                    simd_math::SoaTransform& dest = _args->job.output[i];
                    VOX_SUB_PASS(src, layer_weight, dest);
                }
            }
        } else {
            // Skip layer as its weight is 0.
        }
    }
}
}  // namespace

bool BlendingJob::Run() const {
    if (!Validate()) {
        return false;
    }

    // Initializes blended parameters that are exchanged across blend stages.
    ProcessArgs process_args(*this);

    // Blends all layers to the job output buffers.
    BlendLayers(&process_args);

    // Applies rest pose.
    BlendRestPose(&process_args);

    // Normalizes output.
    Normalize(&process_args);

    // Process additive blending.
    AddLayers(&process_args);

    return true;
}
}  // namespace vox::animation