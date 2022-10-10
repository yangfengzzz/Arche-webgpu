//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.editor/profiling/profiler.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/layout/columns.h"
#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/selection/check_box.h"
#include "vox.render/ui/widgets/texts/text_colored.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class ProfilerWindow : public PanelWindow {
public:
    /**
     * Constructor
     */
    ProfilerWindow(const std::string &p_title,
                   bool p_opened,
                   const PanelWindowSettings &p_windowSettings,
                   float p_frequency);

    /**
     * Update profiling information
     */
    void update(float p_deltaTime);

    /**
     * Enable or disable the profiler
     */
    void enable(bool p_value, bool p_disableLog = false);

private:
    Color calculateActionColor(double p_percentage) const;

    std::string generateActionString(ProfilerReport::Action &p_action);

private:
    enum class ProfilingMode { DEFAULT, CAPTURE };

    float _frequency;
    float _timer = 0.f;
    float _fpsTimer = 0.f;
    ProfilingMode _profilingMode = ProfilingMode::DEFAULT;

    Profiler _profiler;

    Widget *_separator;
    ButtonSimple *_captureResumeButton;
    TextColored *_fpsText;
    TextColored *_elapsedFramesText;
    TextColored *_elapsedTimeText;
    Columns<5> *_actionList;
};

}  // namespace ui
}  // namespace editor
}  // namespace vox