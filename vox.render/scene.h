//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <webgpu/webgpu_cpp.h>

#include "scene_forward.h"
#include "components_manager.h"
#include "physics/physics_manager.h"
#include "lighting/light_manager.h"
#include "lighting/ambient_light.h"
#include "shader/shader_data.h"
#include "background.h"

namespace vox {
/// @brief A collection of entities organized in a tree structure.
///		   It can contain more than one root entity.
class Scene {
public:
    /** Scene name. */
    std::string name;
    
    ComponentsManager _componentsManager;
    physics::PhysicsManager _physicsManager;
    
    /** Light Manager */
    LightManager light_manager;
    
    /** The background of the scene. */
    Background background = Background();
    
    /** Scene-related shader data. */
    ShaderData shaderData;
    
    /**
     * Create scene.
     * @param device - Device
     */
    Scene(wgpu::Device &device);
    
    wgpu::Device &device();
    
    /**
     * Ambient light.
     */
    AmbientLight &ambientLight();
    
    /**
     * Count of root entities.
     */
    size_t rootEntitiesCount();
    
    /**
     * Root entity collection.
     */
    const std::vector<EntityPtr> &rootEntities();
    
    /**
     * Whether it's destroyed.
     */
    bool destroyed();
    
    /**
     * Create root entity.
     * @param name - Entity name
     * @returns Entity
     */
    EntityPtr createRootEntity(std::string name = "");
    
    /**
     * Append an entity.
     * @param entity - The root entity to add
     */
    void addRootEntity(EntityPtr entity);
    
    /**
     * Remove an entity.
     * @param entity - The root entity to remove
     */
    void removeRootEntity(EntityPtr entity);
    
    /**
     * Get root entity from index.
     * @param index - Index
     * @returns Entity
     */
    EntityPtr getRootEntity(size_t index = 0);
    
    /**
     * Find entity globally by name.
     * @param name - Entity name
     * @returns Entity
     */
    EntityPtr findEntityByName(const std::string &name);
    
    /**
     * Destroy this scene.
     */
    void destroy();
    
    void attachRenderCamera(Camera *camera);
    
    void detachRenderCamera(Camera *camera);
    
public:
    void update(float deltaTime);
    
    void updateInputEvent(const InputEvent &inputEvent);
    
    void updateSize(uint32_t win_width, uint32_t win_height,
                    uint32_t fb_width, uint32_t fb_height);
    
    void updateShaderData();
    
private:
    void _processActive(bool active);
        
    void _removeEntity(EntityPtr entity);
    
    std::vector<Camera *> _activeCameras;
    
    bool _destroyed = false;
    std::vector<EntityPtr> _rootEntities;
    AmbientLight _ambientLight;
    
    wgpu::Device &_device;
};

}        // namespace vox
