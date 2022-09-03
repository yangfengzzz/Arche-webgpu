//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <vector>

#include "vox.render/inspector_item.h"
#include "vox.render/scene_forward.h"

namespace vox {
/**
 * The base class of the components.
 */
class Component : public InspectorItem {
public:
    explicit Component(Entity *entity);

    Component(Component &&other) = default;

    ~Component() override;

    /**
     * Indicates whether the component is enabled.
     */
    bool enabled();

    void setEnabled(bool value);

    /**
     * The entity which the component belongs to.
     */
    [[nodiscard]] Entity *entity() const;

    /**
     * The scene which the component's entity belongs to.
     */
    Scene *scene();

public:
    virtual void _onAwake() {}

    virtual void _onEnable() {}

    virtual void _onDisable() {}

    virtual void _onActive() {}

    virtual void _onInActive() {}

protected:
    friend class Entity;

    void _setActive(bool value);

    Entity *_entity;

private:
    bool _enabled = true;
    bool _awoken = false;
};

}  // namespace vox
