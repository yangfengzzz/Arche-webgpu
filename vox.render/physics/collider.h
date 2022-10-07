//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Jolt/Jolt.h>
//
#include <Jolt/Physics/Body/Body.h>

#include "vox.render/component.h"

namespace vox {
class Collider : public Component {
public:
    explicit Collider(Entity* entity);

    [[nodiscard]] uint32_t getIndex() const;

    void setShape(const JPH::Shape* inShape);

private:
    void _onEnable() override;

    void _onDisable() override;

private:
    JPH::Body* _body{nullptr};
};
}  // namespace vox