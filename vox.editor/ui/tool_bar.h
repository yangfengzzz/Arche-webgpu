//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/buttons/button_image.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"

namespace vox {
using namespace ui;

namespace editor {
class EditorResources;

namespace ui {
class Toolbar : public PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    Toolbar(const std::string &p_title,
            bool p_opened,
            const PanelWindowSettings &p_windowSettings,
            EditorResources *resource);

    /**
     * Custom implementation of the draw method
     */
    void DrawImpl() override;

private:
    EditorResources *_resource{nullptr};
    ButtonImage *_playButton{nullptr};
    ButtonImage *_pauseButton{nullptr};
    ButtonImage *_stopButton{nullptr};
    ButtonImage *_nextButton{nullptr};
};

}  // namespace ui
}  // namespace editor
}  // namespace vox