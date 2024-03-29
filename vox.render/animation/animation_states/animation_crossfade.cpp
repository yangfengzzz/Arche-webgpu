//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animation_states/animation_crossfade.h"

namespace vox {
void AnimationCrossFade::loadSkeleton( animation::Skeleton* skeleton) {
    for (auto& clip : _clips) {
        clip.loadSkeleton(skeleton);
    }

    _blended_locals.resize(skeleton->num_soa_joints());
    _blend_job.output = make_span(_blended_locals);
    _blend_job.rest_pose = skeleton->joint_rest_poses();
}

void AnimationCrossFade::update(float dt) {
    // todo
}

const vox::vector<simd_math::SoaTransform>& AnimationCrossFade::locals() const { return _blended_locals; }
}  // namespace vox