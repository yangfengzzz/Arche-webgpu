//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/navigation/base_tool.h"

namespace vox::nav {
void BaseTool::inputEvent(const vox::InputEvent &inputEvent) { handleClick(nullptr, nullptr, false); }

void BaseTool::onUpdate(float deltaTime) { handleRender(); }

}  // namespace vox::nav