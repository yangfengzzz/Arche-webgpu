//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#ifndef scene_animator_hpp
#define scene_animator_hpp

#include "component.h"
#include "scene_animation_clip.h"
#include <vector>
#include <string>

namespace vox {
class SceneAnimator : public Component {
public:
    SceneAnimator(Entity *entity);
    
    void update(float deltaTime);
    
    void addAnimationClip(std::unique_ptr<SceneAnimationClip> &&clip);
    
    void play(const std::string &name);
    
private:
    void _onEnable() override;
    
    void _onDisable() override;
    
private:
    friend class ComponentsManager;
    
    ssize_t _onUpdateIndex = -1;
    ssize_t _activeAnimation = -1;
    std::vector<std::unique_ptr<SceneAnimationClip>> _animationClips;
};

}

#endif /* scene_animator_hpp */
