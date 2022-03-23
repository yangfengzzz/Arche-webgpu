//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef behaviour_hpp
#define behaviour_hpp

#include "script.h"

namespace vox {
/**
 * Behaviour class, used for lua logic writing.
 */
class Behaviour : public Script {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    explicit Behaviour(Entity *entity);
    
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
    
    /**
     * Called when the collision enter.
     * @param other ColliderShape
     */
    void onTriggerEnter(physics::ColliderShapePtr other) override;
    
    /**
     * Called when the collision stay.
     * @remarks onTriggerStay is called every frame while the collision stay.
     * @param other ColliderShape
     */
    void onTriggerExit(physics::ColliderShapePtr other) override;
    
    /**
     * Called when the collision exit.
     * @param other ColliderShape
     */
    void onTriggerStay(physics::ColliderShapePtr other) override;
    
    void inputEvent(const InputEvent &inputEvent) override;
    
    void resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;
};

}
#endif /* behaviour_hpp */
