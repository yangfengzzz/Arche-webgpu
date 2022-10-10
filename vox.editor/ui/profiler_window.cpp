//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/profiler_window.h"

#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
ProfilerWindow::ProfilerWindow(const std::string &p_title,
                               bool p_opened,
                               const PanelWindowSettings &p_windowSettings,
                               float p_frequency)
    : PanelWindow(p_title, p_opened, p_windowSettings), _frequency(p_frequency) {
    allow_horizontal_scrollbar_ = true;

    CreateWidget<Text>("Profiler state: ").line_break_ = false;
    CreateWidget<CheckBox>(false, "").value_changed_event_ +=
            std::bind(&ProfilerWindow::enable, this, std::placeholders::_1, false);

    _fpsText = &CreateWidget<TextColored>("");
    _captureResumeButton = &CreateWidget<ButtonSimple>("Capture");
    _captureResumeButton->idle_background_color_ = {0.7f, 0.5f, 0.f};
    _captureResumeButton->clicked_event_ += [this] {
        _profilingMode = _profilingMode == ProfilingMode::CAPTURE ? ProfilingMode::DEFAULT : ProfilingMode::CAPTURE;
        _captureResumeButton->label_ = _profilingMode == ProfilingMode::CAPTURE ? "Resume" : "Capture";
    };
    _elapsedFramesText = &CreateWidget<TextColored>("", Color(1.f, 0.8f, 0.01f, 1));
    _elapsedTimeText = &CreateWidget<TextColored>("", Color(1.f, 0.8f, 0.01f, 1));
    _separator = &CreateWidget<::vox::ui::Separator>();
    _actionList = &CreateWidget<Columns<5>>();
    _actionList->widths_ = {300.f, 100.f, 100.f, 100.f, 200.f};

    enable(false, true);
}

void ProfilerWindow::update(float p_deltaTime) {
    _timer += p_deltaTime;
    _fpsTimer += p_deltaTime;

    while (_fpsTimer >= 0.07f) {
        _fpsText->content_ = "FPS: " + std::to_string(static_cast<int>(1.0f / p_deltaTime));
        _fpsTimer -= 0.07f;
    }

    if (_profiler.IsEnabled()) {
        _profiler.Update(p_deltaTime);

        while (_timer >= _frequency) {
            if (_profilingMode == ProfilingMode::DEFAULT) {
                ProfilerReport report = _profiler.GenerateReport();
                _profiler.ClearHistory();
                _actionList->RemoveAllWidgets();

                _elapsedFramesText->content_ = "Elapsed frames: " + std::to_string(report.elapsed_frames);
                _elapsedTimeText->content_ = "Elapsed time: " + std::to_string(report.elapsed_time);

                _actionList->CreateWidget<Text>("Action");
                _actionList->CreateWidget<Text>("Total duration");
                _actionList->CreateWidget<Text>("Frame duration");
                _actionList->CreateWidget<Text>("Frame load");
                _actionList->CreateWidget<Text>("Total calls");

                for (auto &action : report.actions) {
                    auto color = calculateActionColor(action.percentage);
                    _actionList->CreateWidget<TextColored>(action.name, color);
                    _actionList->CreateWidget<TextColored>(std::to_string(action.duration) + "s", color);
                    _actionList->CreateWidget<TextColored>(std::to_string(action.duration / action.calls) + "s", color);
                    _actionList->CreateWidget<TextColored>(std::to_string(action.percentage) + "%%", color);
                    _actionList->CreateWidget<TextColored>(std::to_string(action.calls) + " calls", color);
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
            _profiler.Enable();
    } else {
        if (!p_disableLog)
            // OVLOG_INFO("Profiling stoped!");
            _profiler.Disable();
        _profiler.ClearHistory();
        _actionList->RemoveAllWidgets();
    }

    _captureResumeButton->enabled_ = p_value;
    _elapsedFramesText->enabled_ = p_value;
    _elapsedTimeText->enabled_ = p_value;
    _separator->enabled_ = p_value;
}

Color ProfilerWindow::calculateActionColor(double p_percentage) const {
    if (p_percentage <= 25.0f)
        return {0.0f, 1.0f, 0.0f, 1.0f};
    else if (p_percentage <= 50.0f)
        return {1.0f, 1.0f, 0.0f, 1.0f};
    else if (p_percentage <= 75.0f)
        return {1.0f, 0.6f, 0.0f, 1.0f};
    else
        return {1.0f, 0.0f, 0.0f, 1.0f};
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

}  // namespace vox::editor::ui
