//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/raw_skeleton.h"

#include "vox.animation/runtime/skeleton.h"

namespace vox::animation::offline {

RawSkeleton::RawSkeleton() = default;

RawSkeleton::~RawSkeleton() = default;

bool RawSkeleton::Validate() const {
    if (num_joints() > Skeleton::kMaxJoints) {
        return false;
    }
    return true;
}

namespace {
struct JointCounter {
    JointCounter() : num_joints(0) {}
    void operator()(const RawSkeleton::Joint&, const RawSkeleton::Joint*) { ++num_joints; }
    int num_joints;
};
}  // namespace

// Iterates through all the root children and count them.
int RawSkeleton::num_joints() const { return IterateJointsDF(*this, JointCounter()).num_joints; }
}  // namespace vox::animation::offline
