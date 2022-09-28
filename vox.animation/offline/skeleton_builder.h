//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/export.h"
#include "vox.base/memory/unique_ptr.h"
#include "vox.math/scalable_transform.h"

namespace vox::animation {

// Forward declares the runtime skeleton type.
class Skeleton;

namespace offline {

// Forward declares the offline skeleton type.
struct RawSkeleton;

// Defines the class responsible for building Skeleton instances.
class VOX_ANIMOFFLINE_DLL SkeletonBuilder {
public:
    // Creates a Skeleton based on _raw_skeleton and *this builder parameters.
    // Returns a Skeleton instance on success, an empty unique_ptr on failure. See
    // RawSkeleton::Validate() for more details about failure reasons.
    // The skeleton is returned as a unique_ptr as ownership is given back to the
    // caller.
    vox::unique_ptr<vox::animation::Skeleton> operator()(const RawSkeleton& _raw_skeleton) const;
};
}  // namespace offline
}  // namespace vox::animation