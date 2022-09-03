//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "background.h"
#include "lighting/ambient_light.h"
#include "platform/input_events.h"
#include "scene_forward.h"
#include "shader/shader_data.h"
#include "vobject.h"

namespace vox {
/// @brief A collection of entities organized in a tree structure.
///		   It can contain more than one root entity.
class Scene : public VObject {
public:
    /** Scene name. */
    std::string name;

    /** The background of the scene. */
    Background background = Background();

    /** Scene-related shader data. */
    ShaderData shaderData;

    /**
     * Create scene.
     * @param device - Device
     */
    Scene(wgpu::Device &device);

    ~Scene();

    wgpu::Device &device();

    /**
     * Ambient light.
     */
    const std::shared_ptr<AmbientLight> &ambientLight() const;

    void setAmbientLight(const std::shared_ptr<AmbientLight> &light);

    /**
     * Count of root entities.
     */
    size_t rootEntitiesCount();

    /**
     * Root entity collection.
     */
    const std::vector<std::unique_ptr<Entity>> &rootEntities() const;

    /**
     * Play the scene
     */
    void play();

    /**
     * Returns true if the scene is playing
     */
    bool isPlaying() const;

    /**
     * Create root entity.
     * @param name - Entity name
     * @returns Entity
     */
    Entity *createRootEntity(std::string name = "");

    /**
     * Append an entity.
     * @param entity - The root entity to add
     */
    void addRootEntity(std::unique_ptr<Entity> &&entity);

    /**
     * Remove an entity.
     * @param entity - The root entity to remove
     */
    void removeRootEntity(Entity *entity);

    /**
     * Get root entity from index.
     * @param index - Index
     * @returns Entity
     */
    Entity *getRootEntity(size_t index = 0);

    /**
     * Find entity globally by name.
     * @param name - Entity name
     * @returns Entity
     */
    Entity *findEntityByName(const std::string &name);

    void attachRenderCamera(Camera *camera);

    void detachRenderCamera(Camera *camera);

public:
    void updateShaderData();

public:
    /**
     * Called when the serialization is asked
     */
    void onSerialize(nlohmann::json &data) override;

    /**
     * Called when the deserialization is asked
     */
    void onDeserialize(nlohmann::json &data) override;

private:
    friend class SceneManager;

    void _processActive(bool active);

    void _removeEntity(Entity *entity);

    std::vector<Camera *> _activeCameras{};

    bool _isActiveInEngine = false;

    std::vector<std::unique_ptr<Entity>> _rootEntities;
    std::shared_ptr<AmbientLight> _ambientLight;

    wgpu::Device &_device;
};

}  // namespace vox
