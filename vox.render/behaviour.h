//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <sol/sol.hpp>

#include "vox.render/event.h"
#include "vox.render/script.h"

namespace vox {
/**
 * Behaviour class, used for lua logic writing.
 */
class Behaviour : public Script {
public:
    static Event<Behaviour *> createdEvent;
    static Event<Behaviour *> destroyedEvent;

    std::string scriptName;

    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit Behaviour(Entity *entity);

    /**
     * Register the behaviour to lua
     * Returns true on success
     */
    bool registerToLuaContext(sol::state &p_luaState, const std::string &p_scriptFolder);

    /**
     * Register the behaviour to lua
     * Returns true on success
     */
    void unregisterFromLuaContext();

    /**
     * Call a lua function for this behaviour
     */
    template <typename... Args>
    void luaCall(const std::string &p_functionName, Args &&...p_args);

    /**
     * Return the lua table attached to this behaviour
     */
    sol::table &table();

    /**
     * Destructor
     */
    ~Behaviour() override;

public:
    /**
     * Called when be enabled first time, only once.
     */
    void onAwake() override;

    /**
     * Called when be enabled.
     */
    void onEnable() override;

    /**
     * Called when be disabled.
     */
    void onDisable() override;

    /**
     * Called at the end of the destroyed frame.
     */
    void onDestroy() override;

public:
    /**
     * Called before the frame-level loop start for the first time, only once.
     */
    void onStart() override;

    /**
     * The main loop, called frame by frame.
     * @param deltaTime - The deltaTime when the script update.
     */
    void onUpdate(float deltaTime) override;

    /**
     * Called after the onUpdate finished, called frame by frame.
     * @param deltaTime - The deltaTime when the script update.
     */
    void onLateUpdate(float deltaTime) override;

    /**
     * Called before camera rendering, called per camera.
     * @param camera - Current camera.
     */
    void onBeginRender(Camera *camera) override;

    /**
     * Called after camera rendering, called per camera.
     * @param camera - Current camera.
     */
    void onEndRender(Camera *camera) override;

    void inputEvent(const InputEvent &inputEvent) override;

    void resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer &p_root) override;

private:
    sol::table _object = sol::lua_nil;
};

}  // namespace vox

#include "behaviour-inl.h"