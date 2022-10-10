//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/console.h"

#include "vox.editor/editor_actions.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/layout/spacing.h"
#include "vox.render/ui/widgets/selection/check_box.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
namespace {
template <typename Mutex>
class ConsoleSink : public spdlog::sinks::base_sink<Mutex> {
public:
    explicit ConsoleSink(Console *console) : console_(console) {}

    void OnDestroy() { console_ = nullptr; }

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override {
        if (console_) {
            // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
            // msg.raw contains preformatted log

            // If needed (very likely but not mandatory), the sink formats the message before sending it to its final
            // destination:
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
            console_->onLogIntercepted(msg.level, fmt::to_string(formatted));
        }
    }

    void flush_() override {}

private:
    Console *console_;
};

}  // namespace

Console::Console(const std::string &p_title, bool p_opened, const PanelWindowSettings &p_windowSettings)
    : PanelWindow(p_title, p_opened, p_windowSettings) {
    console_sink_ = std::make_shared<ConsoleSink<spdlog::details::null_mutex>>(this);
    spdlog::default_logger()->sinks().push_back(console_sink_);

    allow_horizontal_scrollbar_ = true;

    auto &clearButton = CreateWidget<ButtonSimple>("Clear");
    clearButton.size_ = {50.f, 0.f};
    clearButton.idle_background_color_ = {0.5f, 0.f, 0.f};
    clearButton.clicked_event_ += std::bind(&Console::clear, this);
    clearButton.line_break_ = false;

    auto &clearOnPlay = CreateWidget<CheckBox>(_clearOnPlay, "Auto clear on play");

    CreateWidget<Spacing>(5).line_break_ = false;

    auto &enableInfo = CreateWidget<CheckBox>(true, "Info");
    auto &enableWarning = CreateWidget<CheckBox>(true, "Warning");
    auto &enableError = CreateWidget<CheckBox>(true, "Error");

    clearOnPlay.line_break_ = false;
    enableInfo.line_break_ = false;
    enableWarning.line_break_ = false;
    enableError.line_break_ = true;

    clearOnPlay.value_changed_event_ += [this](bool p_value) { _clearOnPlay = p_value; };
    enableInfo.value_changed_event_ += std::bind(&Console::setShowInfoLogs, this, std::placeholders::_1);
    enableWarning.value_changed_event_ += std::bind(&Console::setShowWarningLogs, this, std::placeholders::_1);
    enableError.value_changed_event_ += std::bind(&Console::setShowErrorLogs, this, std::placeholders::_1);

    CreateWidget<Separator>();

    _logGroup = &CreateWidget<Group>();
    _logGroup->ReverseDrawOrder();

    EditorActions::getSingleton().playEvent += std::bind(&Console::clearOnPlay, this);
}

void Console::onLogIntercepted(spdlog::level::level_enum log_level, const std::string &message) {
    Color log_color;
    switch (log_level) {
        case spdlog::level::info:
            log_color = {0.f, 1.f, 1.f, 1.f};
            break;
        case spdlog::level::warn:
            log_color = {1.f, 1.f, 0.f, 1.f};
            break;
        case spdlog::level::err:
            log_color = {1.f, 0.f, 0.f, 1.f};
            break;
        default:
            log_color = {1.f, 1.f, 1.f, 1.f};
            break;
    }
    auto &console_item = _logGroup->CreateWidget<TextColored>(message, log_color);
    console_item.enabled_ = isAllowedByFilter(log_level);
    log_text_widgets_[&console_item] = log_level;
}

void Console::clearOnPlay() {
    if (_clearOnPlay) clear();
}

void Console::clear() {
    log_text_widgets_.clear();
    _logGroup->RemoveAllWidgets();
}

void Console::filterLogs() {
    for (const auto &[widget, logLevel] : log_text_widgets_) widget->enabled_ = isAllowedByFilter(logLevel);
}

bool Console::isAllowedByFilter(spdlog::level::level_enum log_level) const {
    switch (log_level) {
        case spdlog::level::info:
            return _showInfoLog;
        case spdlog::level::warn:
            return _showWarningLog;
        case spdlog::level::err:
            return _showErrorLog;
        default:
            return false;
    }

    return false;
}

void Console::setShowInfoLogs(bool p_value) {
    _showInfoLog = p_value;
    filterLogs();
}

void Console::setShowWarningLogs(bool p_value) {
    _showWarningLog = p_value;
    filterLogs();
}

void Console::setShowErrorLogs(bool p_value) {
    _showErrorLog = p_value;
    filterLogs();
}

}  // namespace vox::editor::ui
