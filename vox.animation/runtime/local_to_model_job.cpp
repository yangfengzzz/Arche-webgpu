//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/runtime/local_to_model_job.h"

#include <cassert>

#include "vox.animation/runtime/skeleton.h"
#include "vox.math/math_utils.h"
#include "vox.simd_math/simd_math.h"
#include "vox.simd_math/soa_float4x4.h"
#include "vox.simd_math/soa_transform.h"

namespace vox::animation {

LocalToModelJob::LocalToModelJob()
    : skeleton(nullptr), root(nullptr), from(Skeleton::kNoParent), to(Skeleton::kMaxJoints), from_excluded(false) {}

bool LocalToModelJob::Validate() const {
    // Don't need any early out, as jobs are valid in most of the performance
    // critical cases.
    // Tests are written in multiple lines in order to avoid branches.
    bool valid = true;

    // Test for nullptr begin pointers.
    if (!skeleton) {
        return false;
    }

    const auto num_joints = static_cast<size_t>(skeleton->num_joints());
    const size_t num_soa_joints = (num_joints + 3) / 4;

    // Test input and output ranges, implicitly tests for nullptr end pointers.
    valid &= input.size() >= num_soa_joints;
    valid &= output.size() >= num_joints;

    return valid;
}

bool LocalToModelJob::Run() const {
    if (!Validate()) {
        return false;
    }

    const span<const int16_t>& parents = skeleton->joint_parents();

    // Initializes an identity matrix that will be used to compute roots model
    // matrices without requiring a branch.
    const simd_math::Float4x4 identity = simd_math::Float4x4::identity();
    const simd_math::Float4x4* root_matrix = (root == nullptr) ? &identity : root;

    // Applies hierarchical transformation.
    // Loop ends after "to".
    const int end = std::min(to + 1, skeleton->num_joints());
    // Begins iteration from "from", or the next joint if "from" is excluded.
    // Process next joint if end is not reach. parents[begin] >= from is true as
    // long as "begin" is a child of "from".
    for (int i = std::max(from + from_excluded, 0), process = i < end && (!from_excluded || parents[i] >= from);
         process;) {
        // Builds soa matrices from soa transforms.
        const simd_math::SoaTransform& transform = input[i / 4];
        const simd_math::SoaFloat4x4 local_soa_matrices =
                simd_math::SoaFloat4x4::FromAffine(transform.translation, transform.rotation, transform.scale);

        // Converts to aos matrices.
        simd_math::Float4x4 local_aos_matrices[4];
        simd_math::Transpose16x16(&local_soa_matrices.cols[0].x, local_aos_matrices->cols);

        // parents[i] >= from is true as long as "i" is a child of "from".
        for (const int soa_end = (i + 4) & ~3; i < soa_end && process; ++i, process = i < end && parents[i] >= from) {
            const int parent = parents[i];
            const simd_math::Float4x4* parent_matrix = parent == Skeleton::kNoParent ? root_matrix : &output[parent];
            output[i] = *parent_matrix * local_aos_matrices[i & 3];
        }
    }
    return true;
}
}  // namespace vox::animation
