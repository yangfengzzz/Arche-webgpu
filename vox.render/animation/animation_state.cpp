//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animation_state.h"

#include "vox.animation/runtime/skeleton_utils.h"
#include "vox.base/logging.h"

namespace vox {
void AnimationState::addChild(const std::shared_ptr<AnimationState>& state) {
    auto iter = std::find(_states.begin(), _states.end(), state);
    if (iter == _states.end()) {
        _states.push_back(state);
    } else {
        LOGE("State already attached.")
    }
}

void AnimationState::removeChild(const std::shared_ptr<AnimationState>& state) {
    auto iter = std::find(_states.begin(), _states.end(), state);
    if (iter != _states.end()) {
        _states.erase(iter);
    }
}

vox::vector<vox::simd_math::SimdFloat4>& AnimationState::jointMasks() { return _joint_masks; }

void AnimationState::setJointMasks(const animation::Skeleton& skeleton, float mask, const char* root) {
    simd_math::SimdFloat4 simdMask = simd_math::simd_float4::Load1(mask);
    if (root == nullptr) {
        for (int i = 0; i < skeleton.num_soa_joints(); ++i) {
            _joint_masks[i] = simdMask;
        }
    } else {
        const auto set_joint = [this, simdMask](int _joint, int) {
            simd_math::SimdFloat4& soa_weight = _joint_masks[_joint / 4];
            soa_weight = simd_math::SetI(soa_weight, simdMask, _joint % 4);
        };

        const int joint = FindJoint(skeleton, root);
        if (joint >= 0) {
            animation::IterateJointsDF(skeleton, set_joint, joint);
        }
    }
}
}  // namespace vox