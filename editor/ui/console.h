//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef console_hpp
#define console_hpp

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/texts/text_colored.h"
#include <glog/logging.h>

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
    Console(const std::string &p_title,
            bool p_opened,
            const PanelWindowSettings &p_windowSettings);
    
    /**
     * Method called when a log event occurred
     */
    void onLogIntercepted(google::LogSeverity logLevel,
                          const std::string &message);
    
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
    bool isAllowedByFilter(google::LogSeverity p_logLevel);
    
private:
    void setShowInfoLogs(bool p_value);
    
    void setShowWarningLogs(bool p_value);
    
    void setShowErrorLogs(bool p_value);
    
private:
    Group *_logGroup{nullptr};
    std::unique_ptr<google::LogSink> _consoleSink{nullptr};
    std::unordered_map<TextColored *, google::LogSeverity> _logTextWidgets{};
    
    bool _clearOnPlay = true;
    bool _showInfoLog = true;
    bool _showWarningLog = true;
    bool _showErrorLog = true;
};


}
}
}
#endif /* console_hpp */
