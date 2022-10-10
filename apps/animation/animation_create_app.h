//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
#include "vox.animation/offline/raw_animation.h"
#include "vox.animation/offline/raw_skeleton.h"
#include "vox.animation/runtime/animation.h"
#include "vox.animation/runtime/skeleton.h"

namespace vox {
class AnimationClip;

class AnimationCreateApp : public ForwardApplication {
public:
    void loadScene() override;

private:
    // Millipede skeleton number of slices. 7 joints per slice.
    int slice_count_ = 26;

    // Procedurally builds millipede skeleton and walk animation
    void _build(Animator* animator, AnimationClip* animationClip);

    void _createSkeleton(animation::offline::RawSkeleton* _skeleton) const;

    void _createAnimation(animation::Skeleton& skeleton, animation::offline::RawAnimation* _animation) const;
};
}  // namespace vox