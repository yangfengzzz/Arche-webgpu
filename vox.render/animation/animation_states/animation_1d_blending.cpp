//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animation_states/animation_1d_blending.h"

namespace vox {
void Animator1DBlending::loadSkeleton(const animation::Skeleton& skeleton) {
    num_soa_joints = skeleton.num_soa_joints();
    num_joints = skeleton.num_joints();
    for (auto& clip : _clips) {
        clip->_setNumSoaJoints(num_soa_joints);
        clip->_setNumJoints(num_joints);
    }

    _blended_locals.resize(num_soa_joints);
    _blend_job.output = make_span(_blended_locals);
    _blend_job.rest_pose = skeleton.joint_rest_poses();
}

std::shared_ptr<AnimationClip> Animator1DBlending::addAnimatorClip(const char* _filename, float location) {
    auto clip = std::make_shared<AnimationClip>(_filename);
    clip->_setNumSoaJoints(num_soa_joints);
    clip->_setNumJoints(num_joints);
    _clips.push_back(clip);
    _locations.emplace_back(location);
    return clip;
}

void Animator1DBlending::update(float dt) {
    _layers.clear();
    _additive_layers.clear();

    const auto kNumIntervals = static_cast<float>(_clips.size() - 1);
    const float kInterval = 1.f / kNumIntervals;
    for (int i = 0; i < _clips.size(); ++i) {
        auto& clip = _clips[i];
        clip->update(dt);

        const float x = blendRatio - _locations[i];
        const float y = ((x < 0.f ? x : -x) + kInterval) * kNumIntervals;

        animation::BlendingJob::Layer layer{};
        layer.transform = make_span(clip->locals());
        layer.weight = std::max(0.f, y);
        if (clip->blendMode == AnimationClip::BlendMode::Normal) {
            _layers.push_back(layer);
        } else {
            _additive_layers.push_back(layer);
        }
    }
    if (!_layers.empty() || !_additive_layers.empty()) {
        _blend_job.layers = make_span(_layers);
        _blend_job.additive_layers = make_span(_additive_layers);
        bool result = _blend_job.Run();
    }
}

const vox::vector<simd_math::SoaTransform>& Animator1DBlending::locals() const { return _blended_locals; }

float Animator1DBlending::threshold() const { return _blend_job.threshold; }

void Animator1DBlending::setThreshold(float value) { _blend_job.threshold = value; }

}  // namespace vox