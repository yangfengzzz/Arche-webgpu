//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/export.h"
#include "vox.base/macros.h"
#include "vox.simd_math/simd_math.h"

namespace vox {
// Forward declaration of math structures.
namespace simd_math {
struct SimdQuaternion;
}

namespace animation {

// vox::animation::IKTwoBoneJob performs inverse kinematic on a three joints
// chain (two bones).
// The job computes the transformations (rotations) that needs to be applied to
// the first two joints of the chain (named start and middle joints) such that
// the third joint (named end) reaches the provided target position (if
// possible). The job outputs start and middle joint rotation corrections as
// quaternions.
// The three joints must be ancestors, but don't need to be direct
// ancestors (joints in-between will simply remain fixed).
// Implementation is inspired by Autodesk Maya 2 bone IK, improved stability
// wise and extended with Soften IK.
struct VOX_ANIMATION_DLL IKTwoBoneJob {
    // Constructor, initializes default values.
    IKTwoBoneJob();

    // Validates job parameters. Returns true for a valid job, or false otherwise:
    // -if any input pointer is nullptr
    // -if mid_axis isn't normalized.
    [[nodiscard]] bool Validate() const;

    // Runs job's execution task.
    // The job is validated before any operation is performed, see Validate() for
    // more details.
    // Returns false if *this job is not valid.
    [[nodiscard]] bool Run() const;

    // Job input.

    // Target IK position, in model-space. This is the position the end of the
    // joint chain will try to reach.
    simd_math::SimdFloat4 target;

    // Normalized middle joint rotation axis, in middle joint local-space. Default
    // value is z axis. This axis is usually fixed for a given skeleton (as it's
    // in middle joint space). Its direction is defined like this: a positive
    // rotation around this axis will open the angle between the two bones. This
    // in turn also to define which side the two joints must bend. Job validation
    // will fail if mid_axis isn't normalized.
    simd_math::SimdFloat4 mid_axis;

    // Pole vector, in model-space. The pole vector defines the direction the
    // middle joint should point to, allowing to control IK chain orientation.
    // Note that IK chain orientation will flip when target vector and the pole
    // vector are aligned/crossing each other. Its caller responsibility to
    // ensure that this doesn't happen.
    simd_math::SimdFloat4 pole_vector;

    // Twist_angle rotates IK chain around the vector define by start-to-target
    // vector. Default is 0.
    float twist_angle;

    // Soften ratio allows the chain to gradually fall behind the target
    // position. This prevents the joint chain from snapping into the final
    // position, softening the final degrees before the joint chain becomes flat.
    // This ratio represents the distance to the end, from which softening is
    // starting.
    float soften;

    // Weight given to the IK correction clamped in range [0,1]. This allows to
    // blend / interpolate from no IK applied (0 weight) to full IK (1).
    float weight;

    // Model-space matrices of the start, middle and end joints of the chain.
    // The 3 joints should be ancestors. They don't need to be direct
    // ancestors though.
    const simd_math::Float4x4* start_joint;
    const simd_math::Float4x4* mid_joint;
    const simd_math::Float4x4* end_joint;

    // Job output.

    // Local-space corrections to apply to start and middle joints in order for
    // end joint to reach target position.
    // These quaternions must be multiplied to the local-space quaternion of their
    // respective joints.
    simd_math::SimdQuaternion* start_joint_correction;
    simd_math::SimdQuaternion* mid_joint_correction;

    // Optional boolean output value, set to true if target can be reached with IK
    // computations. Reachability is driven by bone chain length, soften ratio and
    // target distance. Target is considered unreached if weight is less than 1.
    bool* reached;
};
}  // namespace animation
}  // namespace vox