//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <spdlog/sinks/base_sink.h>

#include "vox.base/logging.h"
#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/texts/text_colored.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class Console : public PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    Console(const std::string &p_title, bool p_opened, const PanelWindowSettings &p_windowSettings);

    /**
     * Method called when a log event occurred
     */
    void onLogIntercepted(spdlog::level::level_enum log_level, const std::string &message);

    /**
     * Called when the scene plays. It will clear the console if the "Clear on play" settings is on
     */
    void clearOnPlay();

    /**
     * Clear the console
     */
    void clear();

    /**
     * Filter logs using defined filters
     */
    void filterLogs();

    /**
     * Verify if a given log level is allowed by the current filter
     * @param p_logLevel p_logLevel
     */
    [[nodiscard]] bool isAllowedByFilter(spdlog::level::level_enum log_level) const;

private:
    void setShowInfoLogs(bool p_value);

    void setShowWarningLogs(bool p_value);

    void setShowErrorLogs(bool p_value);

private:
    Group *_logGroup{nullptr};
    std::unordered_map<TextColored *, spdlog::level::level_enum> log_text_widgets_{};
    std::shared_ptr<spdlog::sinks::base_sink<spdlog::details::null_mutex>> console_sink_{nullptr};

    bool _clearOnPlay = true;
    bool _showInfoLog = true;
    bool _showWarningLog = true;
    bool _showErrorLog = true;
};

}  // namespace ui
}  // namespace editor
}  // namespace vox
