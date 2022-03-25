//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "animator.h"
#include "mesh/skinned_mesh_renderer.h"
#include "loader/animator_loader.h"
#include "scene.h"
#include "entity.h"
#include "components_manager.h"
#include <string>

namespace vox {
std::string Animator::name() {
    return "Animator";
}

Animator::Animator(Entity *entity) :
Component(entity) {
}

bool Animator::addAnimationClip(const std::string &filename) {
    const auto skinnedMesh = entity()->getComponent<SkinnedMeshRenderer>();
    if (skinnedMesh) {
        return addAnimationClip(filename, skinnedMesh->numJoints(), skinnedMesh->numSoaJoints());
    } else {
        assert(false && "found no skeleton");
        throw std::exception();
    }
}

bool Animator::addAnimationClip(const std::string &filename, int num_joints, int num_soa_joints) {
    ozz::unique_ptr<AnimationClip> clip = ozz::make_unique<AnimationClip>();
    
    if (!ozz::loader::LoadAnimation(filename.c_str(), &clip->animation)) {
        return false;
    }
    
    // Allocates sampler runtime buffers.
    clip->locals.resize(num_soa_joints);
    
    // Allocates a context that matches animation requirements.
    clip->context.Resize(num_joints);
    
    _clips.emplace_back(std::move(clip));
    _layers.resize(_clips.size());
    
    return true;
}

void Animator::update(float deltaTime) {
    size_t kNumLayers = _clips.size();
    
    // Updates and samples all animations to their respective local space
    // transform buffers.
    for (int i = 0; i < kNumLayers; ++i) {
        AnimationClip *clip = _clips[i].get();
        
        // Updates animations time.
        clip->controller.update(clip->animation, deltaTime);
        
        // Early out if this sampler weight makes it irrelevant during blending.
        if (_clips[i]->weight <= 0.f) {
            continue;
        }
        
        // Setup sampling job.
        ozz::animation::SamplingJob sampling_job;
        sampling_job.animation = &clip->animation;
        sampling_job.context = &clip->context;
        sampling_job.ratio = clip->controller.timeRatio();
        sampling_job.output = make_span(clip->locals);
        
        // Samples animation.
        if (!sampling_job.Run()) {
            return;
        }
    }
    
    for (int i = 0; i < kNumLayers; ++i) {
        _layers[i].transform = make_span(_clips[i]->locals);
        _layers[i].weight = _clips[i]->weight;
    }
}

ozz::span<ozz::animation::BlendingJob::Layer> Animator::layers() {
    return make_span(_layers);
}

void Animator::_onEnable() {
   ComponentsManager::getSingleton().addOnUpdateAnimators(this);
}

void Animator::_onDisable() {
    ComponentsManager::getSingleton().removeOnUpdateAnimators(this);
}

//MARK: - Reflection
void Animator::onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) {
    
}

void Animator::onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) {
    
}

void Animator::onInspector(ui::WidgetContainer& p_root) {
    
}

}
