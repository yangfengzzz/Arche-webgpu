//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <PxPhysicsAPI.h>

#include "vox.render/component.h"
#include "vox.render/update_flag.h"

namespace vox {
class PhysxCollider : public Component {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit PhysxCollider(Entity *entity);

    void OnUpdate();

    virtual void OnLateUpdate() {}

    physx::PxRigidActor *native_actor_{};

    void _onEnable() override;

    void _onDisable() override;

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
    std::unique_ptr<UpdateFlag> update_flag_;
};
}  // namespace vox