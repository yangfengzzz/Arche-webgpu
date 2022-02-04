//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef scene_animation_clip_hpp
#define scene_animation_clip_hpp

#include <string>
#include "entity.h"

namespace vox {
class SceneAnimationClip {
public:
    struct AnimationChannel {
        enum PathType {
            TRANSLATION, ROTATION, SCALE
        };
        PathType path;
        EntityPtr node;
        uint32_t samplerIndex;
    };
    
    struct AnimationSampler {
        enum InterpolationType {
            LINEAR, STEP, CUBICSPLINE
        };
        InterpolationType interpolation;
        std::vector<float> inputs;
        std::vector<Vector4F> outputsVec4;
    };
    
public:
    SceneAnimationClip(const std::string &name);
    
    void update(float deltaTime);
    
    const std::string &name() const;
    
    float start() const;
    
    void setStart(float time);
    
    float end() const;
    
    void setEnd(float time);
    
    void addSampler(const AnimationSampler &sampler);
    
    void addChannel(const AnimationChannel &channel);
    
private:
    std::string _name;
    std::vector<AnimationSampler> _samplers;
    std::vector<AnimationChannel> _channels;
    float _start = std::numeric_limits<float>::max();
    float _end = std::numeric_limits<float>::min();
    
    float _currentTime = 0.0f;
};

}

#endif /* scene_animation_clip_hpp */
