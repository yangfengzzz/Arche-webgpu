//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "console.h"

#include "ui/widgets/buttons/button_simple.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/visual/separator.h"
#include "ui/widgets/layout/spacing.h"
#include "editor_actions.h"

namespace vox {
namespace editor {
namespace ui {
namespace {
class ConsoleSink : public google::LogSink {
public:
    ConsoleSink(Console *console) : _console(console) {
    }
    
    virtual void send(google::LogSeverity severity, const char *full_filename,
                      const char *base_filename, int line,
                      const google::LogMessageTime &logmsgtime, const char *message,
                      size_t message_len) {
        _console->onLogIntercepted(severity, google::LogSink::ToString(severity, full_filename,
                                                                       line, logmsgtime,
                                                                       message, message_len));
    }
    
    virtual void WaitTillSent() {
        _console->onLogIntercepted(google::INFO, "console: waitTillSent");
    }
    
private:
    Console *_console;
};

} // namespace

Console::Console(const std::string &p_title,
                 bool p_opened,
                 const PanelWindowSettings &p_windowSettings) :
PanelWindow(p_title, p_opened, p_windowSettings) {
    _consoleSink = std::make_unique<ConsoleSink>(this);
    google::AddLogSink(_consoleSink.get());
    
    allowHorizontalScrollbar = true;
    
    auto &clearButton = createWidget<ButtonSimple>("Clear");
    clearButton.size = {50.f, 0.f};
    clearButton.idleBackgroundColor = {0.5f, 0.f, 0.f};
    clearButton.clickedEvent += std::bind(&Console::clear, this);
    clearButton.lineBreak = false;
    
    auto &clearOnPlay = createWidget<CheckBox>(_clearOnPlay, "Auto clear on play");
    
    createWidget<Spacing>(5).lineBreak = false;
    
    auto &enableInfo = createWidget<CheckBox>(true, "Info");
    auto &enableWarning = createWidget<CheckBox>(true, "Warning");
    auto &enableError = createWidget<CheckBox>(true, "Error");
    
    clearOnPlay.lineBreak = false;
    enableInfo.lineBreak = false;
    enableWarning.lineBreak = false;
    enableError.lineBreak = true;
    
    clearOnPlay.valueChangedEvent += [this](bool p_value) {
        _clearOnPlay = p_value;
    };
    enableInfo.valueChangedEvent += std::bind(&Console::setShowInfoLogs, this, std::placeholders::_1);
    enableWarning.valueChangedEvent += std::bind(&Console::setShowWarningLogs, this, std::placeholders::_1);
    enableError.valueChangedEvent += std::bind(&Console::setShowErrorLogs, this, std::placeholders::_1);
    
    createWidget<Separator>();
    
    _logGroup = &createWidget<Group>();
    _logGroup->reverseDrawOrder();
    
    EditorActions::getSingleton().playEvent += std::bind(&Console::clearOnPlay, this);
}

void Console::onLogIntercepted(google::LogSeverity logLevel,
                               const std::string &message) {
    Color logColor;
    switch (logLevel) {
        case google::INFO:
            logColor = {0.f, 1.f, 1.f, 1.f};
            break;
        case google::WARNING:
            logColor = {1.f, 1.f, 0.f, 1.f};
            break;
        case google::ERROR:
            logColor = {1.f, 0.f, 0.f, 1.f};
            break;
        default:
            logColor = {1.f, 1.f, 1.f, 1.f};
            break;
    }
    auto &consoleItem1 = _logGroup->createWidget<TextColored>(message, logColor);
    consoleItem1.enabled = isAllowedByFilter(logLevel);
    _logTextWidgets[&consoleItem1] = logLevel;
}

void Console::clearOnPlay() {
    if (_clearOnPlay)
        clear();
}

void Console::clear() {
    _logTextWidgets.clear();
    _logGroup->removeAllWidgets();
}

void Console::filterLogs() {
    for (const auto&[widget, logLevel]: _logTextWidgets)
        widget->enabled = isAllowedByFilter(logLevel);
}

bool Console::isAllowedByFilter(google::LogSeverity logLevel) {
    switch (logLevel) {
        case google::INFO:
            return _showInfoLog;
        case google::WARNING:
            return _showWarningLog;
        case google::ERROR:
            return _showErrorLog;
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


}
}
}
