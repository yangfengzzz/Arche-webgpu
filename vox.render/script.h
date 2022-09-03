//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/component.h"
#include "vox.render/platform/input_events.h"

namespace vox {
/**
 * Script class, used for logic writing.
 */
class Script : public Component {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit Script(Entity *entity);

    ~Script() override;

    /**
     * Called when be enabled first time, only once.
     */
    virtual void onAwake() {}

    /**
     * Called when be enabled.
     */
    virtual void onEnable() {}

    /**
     * Called when be disabled.
     */
    virtual void onDisable() {}

    /**
     * Called at the end of the destroyed frame.
     */
    virtual void onDestroy() {}

public:
    void setIsStarted(bool value);

    bool isStarted();

    /**
     * Called before the frame-level loop start for the first time, only once.
     */
    virtual void onStart() {}

    /**
     * The main loop, called frame by frame.
     * @param deltaTime - The deltaTime when the script update.
     */
    virtual void onUpdate(float deltaTime) {}

    /**
     * Called after the onUpdate finished, called frame by frame.
     * @param deltaTime - The deltaTime when the script update.
     */
    virtual void onLateUpdate(float deltaTime) {}

    /**
     * Called before camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void onBeginRender(Camera *camera) {}

    /**
     * Called after camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void onEndRender(Camera *camera) {}

    virtual void inputEvent(const InputEvent &inputEvent) {}

    virtual void resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {}

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

protected:
    void _onAwake() override;

    void _onEnable() override;

    void _onDisable() override;

    bool _started = false;
};

}  // namespace vox