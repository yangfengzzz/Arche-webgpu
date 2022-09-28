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

// vox::animation::IKAimJob rotates a joint, so it aims at a target. Joint aim
// direction and up vectors can be different from joint axis. The job computes
// the transformation (rotation) that needs to be applied to the joints such
// that a provided forward vector (in joint local-space) aims at the target
// position (in skeleton model-space). Up vector (in joint local-space) is also
// used to keep the joint oriented in the same direction as the pole vector.
// The job also exposes an offset (in joint local-space) from where the forward
// vector should aim the target.
// Result is unstable if joint-to-target direction is parallel to pole vector,
// or if target is too close to joint position.
struct VOX_ANIMATION_DLL IKAimJob {
    // Default constructor, initializes default values.
    IKAimJob();

    // Validates job parameters. Returns true for a valid job, or false otherwise:
    // -if output quaternion pointer is nullptr
    [[nodiscard]] bool Validate() const;

    // Runs job's execution task.
    // The job is validated before any operation is performed, see Validate() for
    // more details.
    // Returns false if *this job is not valid.
    [[nodiscard]] bool Run() const;

    // Job input.

    // Target position to aim at, in model-space
    simd_math::SimdFloat4 target;

    // Joint forward axis, in joint local-space, to be aimed at target position.
    // This vector shall be normalized, otherwise validation will fail.
    // Default is x-axis.
    simd_math::SimdFloat4 forward;

    // Offset position from the joint in local-space, that will aim at target.
    simd_math::SimdFloat4 offset;

    // Joint up axis, in joint local-space, used to keep the joint oriented in the
    // same direction as the pole vector. Default is y-axis.
    simd_math::SimdFloat4 up;

    // Pole vector, in model-space. The pole vector defines the direction
    // the up should point to.  Note that IK chain orientation will flip when
    // target vector and the pole vector are aligned/crossing each other. Its
    // caller responsibility to ensure that this doesn't happen.
    simd_math::SimdFloat4 pole_vector;

    // Twist_angle rotates joint around the target vector.
    // Default is 0.
    float twist_angle;

    // Weight given to the IK correction clamped in range [0,1]. This allows to
    // blend / interpolate from no IK applied (0 weight) to full IK (1).
    float weight;

    // Joint model-space matrix.
    const simd_math::Float4x4* joint;

    // Job output.

    // Output local-space joint correction quaternion. It needs to be multiplied
    // with joint local-space quaternion.
    simd_math::SimdQuaternion* joint_correction;

    // Optional boolean output value, set to true if target can be reached with IK
    // computations. Target is considered not reachable when target is between
    // joint and offset position.
    bool* reached;
};
}  // namespace animation
}  // namespace vox