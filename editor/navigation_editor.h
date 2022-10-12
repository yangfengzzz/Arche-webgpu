//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.editor/editor_application.h"

namespace vox::editor {
class NavigationEditor : public EditorApplication {
public:
    void setupUI() override;

    void updateViews(float deltaTime, wgpu::CommandEncoder& commandEncoder) override;

    void inputEvent(const InputEvent& inputEvent) override;
};
}  // namespace vox::editor