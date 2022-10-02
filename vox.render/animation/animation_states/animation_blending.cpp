//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animation_states/animation_blending.h"

namespace vox {
void AnimatorBlending::loadSkeleton(animation::Skeleton* skeleton) {
    for (auto& clip : _clips) {
        clip->loadSkeleton(skeleton);
    }

    _blended_locals.resize(skeleton->num_soa_joints());
    _blend_job.output = make_span(_blended_locals);
    _blend_job.rest_pose = skeleton->joint_rest_poses();
}

std::shared_ptr<AnimationClip> AnimatorBlending::addAnimatorClip(const std::string& filename, float location) {
    auto clip = std::make_shared<AnimationClip>(filename);
    clip->loadSkeleton(_skeleton);
    _clips.push_back(clip);
    _locations.emplace_back(location);
    return clip;
}

void AnimatorBlending::update(float dt) {
    _layers.clear();
    _additive_layers.clear();

    for (auto & clip : _clips) {
        clip->update(dt);

        animation::BlendingJob::Layer layer{};
        layer.transform = make_span(clip->locals());
        layer.joint_weights = make_span(clip->jointMasks());
        layer.weight = clip->weight;
        if (clip->blendMode == AnimationClip::BlendMode::Normal) {
            _layers.push_back(layer);
        } else {
            _additive_layers.push_back(layer);
        }
    }
    if (!_layers.empty() || !_additive_layers.empty()) {
        _blend_job.layers = make_span(_layers);
        _blend_job.additive_layers = make_span(_additive_layers);
        (void)_blend_job.Run();
    }
}

const vox::vector<simd_math::SoaTransform>& AnimatorBlending::locals() const { return _blended_locals; }

float AnimatorBlending::threshold() const { return _blend_job.threshold; }

void AnimatorBlending::setThreshold(float value) { _blend_job.threshold = value; }

}  // namespace vox
