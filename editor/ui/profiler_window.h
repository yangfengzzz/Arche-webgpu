//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef profiler_window_hpp
#define profiler_window_hpp

#include "profiling/profiler.h"

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/texts/text_colored.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/buttons/button_simple.h"

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
    enum class ProfilingMode {
        DEFAULT,
        CAPTURE
    };
    
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


}
}
}
#endif /* profiler_window_hpp */
