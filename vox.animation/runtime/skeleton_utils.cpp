//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/runtime/skeleton_utils.h"

#include <cassert>
#include <cstring>

#include "vox.simd_math/soa_transform.h"

namespace vox::animation {

int FindJoint(const Skeleton& _skeleton, const char* _name) {
    const auto& names = _skeleton.joint_names();
    for (size_t i = 0; i < names.size(); ++i) {
        if (std::strcmp(names[i], _name) == 0) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Unpacks skeleton rest pose stored in soa format by the skeleton.
vox::ScalableTransform GetJointLocalRestPose(const Skeleton& _skeleton, int _joint) {
    assert(_joint >= 0 && _joint < _skeleton.num_joints() && "Joint index out of range.");

    const vox::simd_math::SoaTransform& soa_transform = _skeleton.joint_rest_poses()[_joint / 4];

    // Transpose SoA data to AoS.
    vox::simd_math::SimdFloat4 translations[4];
    vox::simd_math::Transpose3x4(&soa_transform.translation.x, translations);
    vox::simd_math::SimdFloat4 rotations[4];
    vox::simd_math::Transpose4x4(&soa_transform.rotation.x, rotations);
    vox::simd_math::SimdFloat4 scales[4];
    vox::simd_math::Transpose3x4(&soa_transform.scale.x, scales);

    // Stores to the Transform object.
    ScalableTransform rest_pose;
    const int offset = _joint % 4;
    vox::simd_math::Store3PtrU(translations[offset], &rest_pose.translation.x);
    vox::simd_math::StorePtrU(rotations[offset], &rest_pose.rotation.x);
    vox::simd_math::Store3PtrU(scales[offset], &rest_pose.scale.x);

    return rest_pose;
}
}  // namespace vox::animation
