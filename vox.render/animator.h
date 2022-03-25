//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef animator_hpp
#define animator_hpp

#include "component.h"
#include "animator_controller.h"
#include <ozz/animation/runtime/animation.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/animation/runtime/blending_job.h>
#include <ozz/base/containers/vector.h>
#include <ozz/base/memory/unique_ptr.h>
#include <ozz/base/maths/soa_transform.h>
#include <string>

namespace vox {
class Animator : public Component {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    // Sampler structure contains all the data required to sample a single
    // animation.
    struct AnimationClip {
        // Constructor, default initialization.
        AnimationClip() : weight(1.f) {
        }
        
        // Playback animation controller. This is a utility class that helps with
        // controlling animation playback time.
        vox::AnimatorController controller;
        
        // Blending weight for the layer.
        float weight;
        
        // Runtime animation.
        ozz::animation::Animation animation;
        
        // Sampling context.
        ozz::animation::SamplingJob::Context context;
        
        // Buffer of local transforms as sampled from animation_.
        ozz::vector<ozz::math::SoaTransform> locals;
    };
    
    Animator(Entity *entity);
    
    bool addAnimationClip(const std::string &filename);
    
    bool addAnimationClip(const std::string &filename, int num_joints, int num_soa_joints);
    
    void update(float deltaTime);
    
    ozz::span<ozz::animation::BlendingJob::Layer> layers();
    
public:
    /**
     * Serialize the component
     */
    void onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Deserialize the component
     */
    void onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer& p_root) override;
    
private:
    void _onEnable() override;
    
    void _onDisable() override;
    
private:
    ozz::vector<ozz::unique_ptr<AnimationClip>> _clips;
    ozz::vector<ozz::animation::BlendingJob::Layer> _layers;
};

}

#endif /* animator_hpp */
