//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/script.h"
#include "vox.toolkit/debug_draw/debug_draw.h"

namespace vox::nav {
class BaseTool : public Script {
public:
    debug::DebugDraw* debugDraw{nullptr};

    void inputEvent(const vox::InputEvent& inputEvent) override;

    void onUpdate(float deltaTime) override;

    virtual void handleClick(const float* s, const float* p, bool shift) = 0;

    virtual void handleRender() = 0;
};
}  // namespace vox::nav