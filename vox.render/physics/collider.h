//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Jolt/Jolt.h>
//
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "vox.render/component.h"

namespace vox {
class Collider : public Component {
public:
    explicit Collider(Entity* entity);

    ~Collider() override;

    void setShape(const std::shared_ptr<JPH::Shape>& shape);

    std::shared_ptr<JPH::Shape> getShape();

public:
    void onUpdate(){};

    virtual void onLateUpdate() {}

private:
    void _onEnable() override;

    void _onDisable() override;

private:
    JPH::BodyInterface* _bodyInterface{nullptr};
    std::shared_ptr<JPH::Shape> _shape{nullptr};
    JPH::BodyID _bodyID;
};
}  // namespace vox