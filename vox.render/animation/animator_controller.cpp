//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animator_controller.h"

#include "vox.simd_math/soa_transform.h"

namespace vox {
void AnimatorController::_setNumSoaJoints(int value) {
    _num_soa_joints = value;
    for (auto& state : _states) {
        state._setNumSoaJoints(value);
    }
}

void AnimatorController::_setNumJoints(int value) {
    _num_joints = value;
    for (auto& state : _states) {
        state._setNumJoints(value);
    }
}

AnimatorState& AnimatorController::addAnimatorState(const char* _filename) {
    _states.emplace_back(_filename);
    return _states.back();
}

}  // namespace vox