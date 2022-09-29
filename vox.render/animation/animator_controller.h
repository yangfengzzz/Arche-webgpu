//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/animation/animator_state.h"

namespace vox {
class AnimatorController {
public:
    AnimatorState& addAnimatorState(const char* _filename);

private:
    friend class Animator;
    void _setNumSoaJoints(int value);

    void _setNumJoints(int value);

    int _num_soa_joints;
    int _num_joints;
    vox::vector<AnimatorState> _states;
};
}  // namespace vox
