//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef project_settings_hpp
#define project_settings_hpp

#include "ui/widgets/texts/text.h"
#include "ui/widgets/panel_transformables/panel_window.h"
#include "ini_file.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class ProjectSettings : public PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    ProjectSettings(const std::string &p_title, bool p_opened,
                    const PanelWindowSettings &p_windowSettings,
                    const std::string &p_projectPath, const std::string &p_projectName);
    
    /**
     * Generate a gatherer that will get the value associated to the given key
     * @param p_keyName p_keyName
     */
    template<typename T>
    std::function<T()> generateGatherer(const std::string &p_keyName) {
        return std::bind(&fs::IniFile::get<T>, &_projectSettings, p_keyName);
    }
    
    /**
     * Generate a provider that will set the value associated to the given key
     * @param p_keyName p_keyName
     */
    template<typename T>
    std::function<void(T)> generateProvider(const std::string &p_keyName) {
        return std::bind(&fs::IniFile::set<T>, &_projectSettings, p_keyName, std::placeholders::_1);
    }
    
public:
    /**
     * Reset project settings ini file
     */
    void resetProjectSettings();
    
    /**
     * Verify that project settings are complete (No missing key).
     * Returns true if the integrity is verified
     */
    bool isProjectSettingsIntegrityVerified();
    
    /**
     * Apply project settings to the ini file
     */
    void applyProjectSettings();
    
    
    fs::IniFile &projectSettings();
    
private:
    fs::IniFile _projectSettings;
};


}
}
}
#endif /* project_settings_hpp */
