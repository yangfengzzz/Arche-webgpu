//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef scene_animator_hpp
#define scene_animator_hpp

#include <string>
#include <vector>

#include "component.h"
#include "scene_animation_clip.h"

namespace vox {
class SceneAnimator : public Component {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    SceneAnimator(Entity *entity);

    void update(float deltaTime);

    void addAnimationClip(std::unique_ptr<SceneAnimationClip> &&clip);

    void play(const std::string &name);

public:
    /**
     * Serialize the component
     */
    void onSerialize(nlohmann::json &data) override;

    /**
     * Deserialize the component
     */
    void onDeserialize(nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer &p_root) override;

private:
    void _onEnable() override;

    void _onDisable() override;

private:
    ssize_t _activeAnimation = -1;
    std::vector<std::unique_ptr<SceneAnimationClip>> _animationClips;
};

}  // namespace vox

#endif /* scene_animator_hpp */
