//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/animation/animation_state.h"
#include "vox.render/component.h"
#include "vox.animation/runtime/local_to_model_job.h"

namespace vox {
class Animator : public Component {
public:
    std::shared_ptr<AnimationState> rootState{nullptr};

    bool loadSkeleton(const char* _filename);

    void update(float dt);

    [[nodiscard]] bool localToModelFromExcluded() const;

    void setLocalToModelFromExcluded(bool value);

    [[nodiscard]] int localToModelFrom() const;

    void setLocalToModelFrom(int value);

    [[nodiscard]] int localToModelTo() const;

    void setLocalToModelTo(int value);

    [[nodiscard]] const vox::vector<simd_math::Float4x4>& models() const;

private:
    animation::Skeleton _skeleton;
    animation::LocalToModelJob _ltm_job;
    // Buffer of model space matrices.
    vox::vector<simd_math::Float4x4> _models;
};
}  // namespace vox
