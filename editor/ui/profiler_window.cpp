//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "profiler_window.h"
#include "ui/widgets/visual/separator.h"

namespace vox {
namespace editor {
namespace ui {
ProfilerWindow::ProfilerWindow(const std::string &p_title,
                               bool p_opened,
                               const PanelWindowSettings &p_windowSettings,
                               float p_frequency) :
PanelWindow(p_title, p_opened, p_windowSettings),
_frequency(p_frequency) {
    allowHorizontalScrollbar = true;
    
    createWidget<Text>("Profiler state: ").lineBreak = false;
    createWidget<CheckBox>(false, "").valueChangedEvent += std::bind(&ProfilerWindow::enable, this, std::placeholders::_1, false);
    
    _fpsText = &createWidget<TextColored>("");
    _captureResumeButton = &createWidget<ButtonSimple>("Capture");
    _captureResumeButton->idleBackgroundColor = {0.7f, 0.5f, 0.f};
    _captureResumeButton->clickedEvent += [this] {
        _profilingMode = _profilingMode == ProfilingMode::CAPTURE ? ProfilingMode::DEFAULT : ProfilingMode::CAPTURE;
        _captureResumeButton->label = _profilingMode == ProfilingMode::CAPTURE ? "Resume" : "Capture";
    };
    _elapsedFramesText = &createWidget<TextColored>("", Color(1.f, 0.8f, 0.01f, 1));
    _elapsedTimeText = &createWidget<TextColored>("", Color(1.f, 0.8f, 0.01f, 1));
    _separator = &createWidget<::vox::ui::Separator>();
    _actionList = &createWidget<Columns<5>>();
    _actionList->widths = {300.f, 100.f, 100.f, 100.f, 200.f};
    
    enable(false, true);
}

void ProfilerWindow::update(float p_deltaTime) {
    _timer += p_deltaTime;
    _fpsTimer += p_deltaTime;
    
    while (_fpsTimer >= 0.07f) {
        _fpsText->content = "FPS: " + std::to_string(static_cast<int>(1.0f / p_deltaTime));
        _fpsTimer -= 0.07f;
    }
    
    if (_profiler.isEnabled()) {
        _profiler.update(p_deltaTime);
        
        while (_timer >= _frequency) {
            if (_profilingMode == ProfilingMode::DEFAULT) {
                ProfilerReport report = _profiler.generateReport();
                _profiler.clearHistory();
                _actionList->removeAllWidgets();
                
                _elapsedFramesText->content = "Elapsed frames: " + std::to_string(report.elapsedFrames);
                _elapsedTimeText->content = "Elapsed time: " + std::to_string(report.elaspedTime);
                
                _actionList->createWidget<Text>("Action");
                _actionList->createWidget<Text>("Total duration");
                _actionList->createWidget<Text>("Frame duration");
                _actionList->createWidget<Text>("Frame load");
                _actionList->createWidget<Text>("Total calls");
                
                for (auto &action: report.actions) {
                    auto color = calculateActionColor(action.percentage);
                    _actionList->createWidget<TextColored>(action.name, color);
                    _actionList->createWidget<TextColored>(std::to_string(action.duration) + "s", color);
                    _actionList->createWidget<TextColored>(std::to_string(action.duration / action.calls) + "s", color);
                    _actionList->createWidget<TextColored>(std::to_string(action.percentage) + "%%", color);
                    _actionList->createWidget<TextColored>(std::to_string(action.calls) + " calls", color);
                }
            }
            
            _timer -= _frequency;
        }
    }
}

void ProfilerWindow::enable(bool p_value, bool p_disableLog) {
    if (p_value) {
        if (!p_disableLog)
            // OVLOG_INFO("Profiling started!");
        _profiler.enable();
    } else {
        if (!p_disableLog)
            // OVLOG_INFO("Profiling stoped!");
        _profiler.disable();
        _profiler.clearHistory();
        _actionList->removeAllWidgets();
    }
    
    _captureResumeButton->enabled = p_value;
    _elapsedFramesText->enabled = p_value;
    _elapsedTimeText->enabled = p_value;
    _separator->enabled = p_value;
}

Color ProfilerWindow::calculateActionColor(double p_percentage) const {
    if (p_percentage <= 25.0f) return {0.0f, 1.0f, 0.0f, 1.0f};
    else if (p_percentage <= 50.0f) return {1.0f, 1.0f, 0.0f, 1.0f};
    else if (p_percentage <= 75.0f) return {1.0f, 0.6f, 0.0f, 1.0f};
    else return {1.0f, 0.0f, 0.0f, 1.0f};
}

std::string ProfilerWindow::generateActionString(ProfilerReport::Action &p_action) {
    std::string result;
    
    result += "[" + p_action.name + "]";
    result += std::to_string(p_action.duration) + "s (total) | ";
    result += std::to_string(p_action.duration / p_action.calls) + "s (per call) | ";
    result += std::to_string(p_action.percentage) + "%% | ";
    result += std::to_string(p_action.calls) + " calls";
    
    return result;
}


}
}
}
