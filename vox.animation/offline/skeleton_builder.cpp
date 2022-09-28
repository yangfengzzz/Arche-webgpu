//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/skeleton_builder.h"

#include <cstring>

#include "vox.animation/offline/raw_skeleton.h"
#include "vox.animation/runtime/skeleton.h"
#include "vox.simd_math/soa_transform.h"

namespace vox::animation::offline {

namespace {
// Stores each traversed joint in a vector.
struct JointLister {
    explicit JointLister(int _num_joints) { linear_joints.reserve(_num_joints); }
    void operator()(const RawSkeleton::Joint& _current, const RawSkeleton::Joint* _parent) {
        // Looks for the "lister" parent.
        int16_t parent = Skeleton::kNoParent;
        if (_parent) {
            // Start searching from the last joint.
            int16_t j = static_cast<int16_t>(linear_joints.size()) - 1;
            for (; j >= 0; --j) {
                if (linear_joints[j].joint == _parent) {
                    parent = j;
                    break;
                }
            }
            assert(parent >= 0);
        }
        const Joint listed = {&_current, parent};
        linear_joints.push_back(listed);
    }
    struct Joint {
        const RawSkeleton::Joint* joint;
        int16_t parent;
    };
    // Array of joints in the traversed DAG order.
    vox::vector<Joint> linear_joints;
};
}  // namespace

// Validates the RawSkeleton and fills a Skeleton.
// Uses RawSkeleton::IterateJointsDF to traverse in DAG depth-first order.
// Building skeleton hierarchy in depth first order make it easier to iterate a
// skeleton sub-hierarchy.
unique_ptr<vox::animation::Skeleton> SkeletonBuilder::operator()(const RawSkeleton& _raw_skeleton) const {
    // Tests _raw_skeleton validity.
    if (!_raw_skeleton.Validate()) {
        return nullptr;
    }

    // Everything is fine, allocates and fills the skeleton.
    // Will not fail.
    unique_ptr<vox::animation::Skeleton> skeleton = make_unique<Skeleton>();
    const int num_joints = _raw_skeleton.num_joints();

    // Iterates through all the joint of the raw skeleton and fills a sorted joint
    // list.
    // Iteration order defines runtime skeleton joint ordering.
    JointLister lister(num_joints);
    IterateJointsDF<JointLister&>(_raw_skeleton, lister);
    assert(static_cast<int>(lister.linear_joints.size()) == num_joints);

    // Computes name's buffer size.
    size_t chars_size = 0;
    for (int i = 0; i < num_joints; ++i) {
        const RawSkeleton::Joint& current = *lister.linear_joints[i].joint;
        chars_size += (current.name.size() + 1) * sizeof(char);
    }

    // Allocates all skeleton members.
    char* cursor = skeleton->Allocate(chars_size, num_joints);

    // Copy names. All names are allocated in a single buffer. Only the first name
    // is set, all other names array entries must be initialized.
    for (int i = 0; i < num_joints; ++i) {
        const RawSkeleton::Joint& current = *lister.linear_joints[i].joint;
        skeleton->joint_names_[i] = cursor;
        strcpy(cursor, current.name.c_str());
        cursor += (current.name.size() + 1) * sizeof(char);
    }

    // Transfers sorted joints hierarchy to the new skeleton.
    for (int i = 0; i < num_joints; ++i) {
        skeleton->joint_parents_[i] = lister.linear_joints[i].parent;
    }

    // Transfers t-poses.
    const simd_math::SimdFloat4 w_axis = simd_math::simd_float4::w_axis();
    const simd_math::SimdFloat4 zero = simd_math::simd_float4::zero();
    const simd_math::SimdFloat4 one = simd_math::simd_float4::one();

    for (int i = 0; i < skeleton->num_soa_joints(); ++i) {
        simd_math::SimdFloat4 translations[4];
        simd_math::SimdFloat4 scales[4];
        simd_math::SimdFloat4 rotations[4];
        for (int j = 0; j < 4; ++j) {
            if (i * 4 + j < num_joints) {
                const RawSkeleton::Joint& src_joint = *lister.linear_joints[i * 4 + j].joint;
                translations[j] = simd_math::simd_float4::Load3PtrU(&src_joint.transform.translation.x);
                rotations[j] = simd_math::NormalizeSafe4(
                        simd_math::simd_float4::LoadPtrU(&src_joint.transform.rotation.x), w_axis);
                scales[j] = simd_math::simd_float4::Load3PtrU(&src_joint.transform.scale.x);
            } else {
                translations[j] = zero;
                rotations[j] = w_axis;
                scales[j] = one;
            }
        }
        // Fills the SoaTransform structure.
        simd_math::Transpose4x3(translations, &skeleton->joint_rest_poses_[i].translation.x);
        simd_math::Transpose4x4(rotations, &skeleton->joint_rest_poses_[i].rotation.x);
        simd_math::Transpose4x3(scales, &skeleton->joint_rest_poses_[i].scale.x);
    }

    return skeleton;  // Success.
}
}  // namespace vox::animation::offline
