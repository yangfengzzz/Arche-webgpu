//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/export.h"
#include "vox.base/macros.h"
#include "vox.base/span.h"

namespace vox {

// Forward declaration math structures.
namespace simd_math {
struct SoaTransform;
struct Float4x4;
}  // namespace simd_math

namespace animation {

// Forward declares the Skeleton object used to describe joint hierarchy.
class Skeleton;

// Computes model-space joint matrices from local-space SoaTransform.
// This job uses the skeleton to define joints parent-child hierarchy. The job
// iterates through all joints to compute their transform relatively to the
// skeleton root.
// Job inputs is an array of SoaTransform objects (in local-space), ordered like
// skeleton's joints. Job output is an array of matrices (in model-space),
// ordered like skeleton's joints. Output are matrices, because the combination
// of affine transformations can contain shearing or complex transformation
// that cannot be represented as Transform object.
struct VOX_ANIMATION_DLL LocalToModelJob {
    // Default constructor, initializes default values.
    LocalToModelJob();

    // Validates job parameters. Returns true for a valid job, or false otherwise:
    // -if any input pointer, including ranges, is nullptr.
    // -if the size of the input is smaller than the skeleton's number of joints.
    // Note that this input has a SoA format.
    // -if the size of the output is smaller than the skeleton's number of
    // joints.
    [[nodiscard]] bool Validate() const;

    // Runs job's local-to-model task.
    // The job is validated before any operation is performed, see Validate() for
    // more details.
    // Returns false if job is not valid. See Validate() function.
    [[nodiscard]] bool Run() const;

    // Job input.

    // The Skeleton object describing the joint hierarchy used for local to
    // model space conversion.
    const Skeleton* skeleton;

    // The root matrix will multiply to every model space matrices, default nullptr
    // means an identity matrix. This can be used to directly compute world-space
    // transforms for example.
    const vox::simd_math::Float4x4* root;

    // Defines "from" which joint the local-to-model conversion should start.
    // Default value is vox::Skeleton::kNoParent, meaning the whole hierarchy is
    // updated. This parameter can be used to optimize update by limiting
    // conversion to part of the joint hierarchy. Note that "from" parent should
    // be a valid matrix, as it is going to be used as part of "from" joint
    // hierarchy update.
    int from;

    // Defines "to" which joint the local-to-model conversion should go, "to"
    // included. Update will end before "to" joint is reached if "to" is not part
    // of the hierarchy starting from "from". Default value is
    // vox::animation::Skeleton::kMaxJoints, meaning the hierarchy (starting from
    // "from") is updated to the last joint.
    int to;

    // If true, "from" joint is not updated during job execution. Update starts
    // with all children of "from". This can be used to update a model-space
    // transform independently of the local-space one. To do so: set "from"
    // joint model-space transform matrix, and run this Job with "from_excluded"
    // to update all "from" children.
    // Default value is false.
    bool from_excluded;

    // The input range that store local transforms.
    span<const vox::simd_math::SoaTransform> input;

    // Job output.

    // The output range to be filled with model-space matrices.
    span<vox::simd_math::Float4x4> output;
};
}  // namespace animation
}  // namespace vox