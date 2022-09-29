//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/animation/animator_blending.h"
#include "vox.render/component.h"
#include "vox.animation/runtime/local_to_model_job.h"

namespace vox {
class Animator : public Component {
public:
    std::unique_ptr<AnimatorBlending> blending{nullptr};

    bool loadSkeleton(const char* _filename);

    void update(float dt);

private:
    animation::Skeleton _skeleton;
    animation::LocalToModelJob _ltm_job;
};
}  // namespace vox
