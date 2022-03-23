//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "project_settings.h"

#include "ui/gui_drawer.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/layout/group_collapsable.h"
#include "ui/widgets/visual/separator.h"
#include "ui/widgets/buttons/button_simple.h"

namespace vox {
namespace editor {
namespace ui {
ProjectSettings::ProjectSettings(const std::string &p_title, bool p_opened,
                                 const PanelWindowSettings &p_windowSettings,
                                 const std::string &p_projectPath, const std::string &p_projectName) :
PanelWindow(p_title, p_opened, p_windowSettings),
_projectSettings(p_projectPath + p_projectName + ".project") {
    auto &saveButton = createWidget<ButtonSimple>("Apply");
    saveButton.idleBackgroundColor = {0.0f, 0.5f, 0.0f};
    saveButton.clickedEvent += [this] {
        applyProjectSettings();
        _projectSettings.rewrite();
    };
    
    saveButton.lineBreak = false;
    
    auto &resetButton = createWidget<ButtonSimple>("Reset");
    resetButton.idleBackgroundColor = {0.5f, 0.0f, 0.0f};
    resetButton.clickedEvent += [this] {
        resetProjectSettings();
    };
    
    createWidget<Separator>();
    
    {
        /* Physics settings */
        auto &root = createWidget<GroupCollapsable>("Physics");
        auto &columns = root.createWidget<Columns<2>>();
        columns.widths[0] = 125;
        
        GUIDrawer::drawScalar<float>(columns, "Gravity", generateGatherer<float>("gravity"),
                                     generateProvider<float>("gravity"), 0.1f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);
    }
    
    {
        /* Build settings */
        auto &generationRoot = createWidget<GroupCollapsable>("Build");
        auto &columns = generationRoot.createWidget<Columns<2>>();
        columns.widths[0] = 125;
        
        GUIDrawer::drawBoolean(columns, "Development build", generateGatherer<bool>("dev_build"),
                               generateProvider<bool>("dev_build"));
    }
    
    {
        /* Windowing settings */
        auto &windowingRoot = createWidget<GroupCollapsable>("Windowing");
        auto &columns = windowingRoot.createWidget<Columns<2>>();
        columns.widths[0] = 125;
        
        GUIDrawer::drawScalar<int>(columns, "Resolution X", generateGatherer<int>("x_resolution"),
                                   generateProvider<int>("x_resolution"), 1, 0, 10000);
        GUIDrawer::drawScalar<int>(columns, "Resolution Y", generateGatherer<int>("y_resolution"),
                                   generateProvider<int>("y_resolution"), 1, 0, 10000);
        GUIDrawer::drawBoolean(columns, "Fullscreen", generateGatherer<bool>("fullscreen"),
                               generateProvider<bool>("fullscreen"));
        GUIDrawer::drawString(columns, "Executable name", generateGatherer<std::string>("executable_name"),
                              generateProvider<std::string>("executable_name"));
    }
    
    {
        /* Rendering settings */
        auto &renderingRoot = createWidget<GroupCollapsable>("Rendering");
        auto &columns = renderingRoot.createWidget<Columns<2>>();
        columns.widths[0] = 125;
        
        GUIDrawer::drawBoolean(columns, "Vertical Sync.", generateGatherer<bool>("vsync"),
                               generateProvider<bool>("vsync"));
        GUIDrawer::drawBoolean(columns, "Multi-sampling", generateGatherer<bool>("multisampling"),
                               generateProvider<bool>("multisampling"));
        GUIDrawer::drawScalar<int>(columns, "Samples", generateGatherer<int>("samples"),
                                   generateProvider<int>("samples"), 1, 2, 16);
        GUIDrawer::drawScalar<int>(columns, "OpenGL Major", generateGatherer<int>("opengl_major"),
                                   generateProvider<int>("opengl_major"), 1, 3, 4);
        GUIDrawer::drawScalar<int>(columns, "OpenGL Minor", generateGatherer<int>("opengl_minor"),
                                   generateProvider<int>("opengl_minor"), 1, 0, 6);
    }
    
    {
        /* Scene Management settings */
        auto &gameRoot = createWidget<GroupCollapsable>("Scene Management");
        auto &columns = gameRoot.createWidget<Columns<2>>();
        columns.widths[0] = 125;
        
        GUIDrawer::drawDDString(columns, "Start scene", generateGatherer<std::string>("start_scene"),
                                generateProvider<std::string>("start_scene"), "File");
    }
}

void ProjectSettings::resetProjectSettings() {
    _projectSettings.removeAll();
    _projectSettings.add<float>("gravity", -9.81f);
    _projectSettings.add<int>("x_resolution", 1280);
    _projectSettings.add<int>("y_resolution", 720);
    _projectSettings.add<bool>("fullscreen", false);
    _projectSettings.add<std::string>("executable_name", "Game");
    _projectSettings.add<std::string>("start_scene", "Scene.ovscene");
    _projectSettings.add<bool>("vsync", true);
    _projectSettings.add<bool>("multisampling", false);
    _projectSettings.add<int>("samples", 1);
    _projectSettings.add<int>("webgpu_major", 0);
    _projectSettings.add<int>("webgpu_minor", 1);
    _projectSettings.add<bool>("dev_build", true);
}

bool ProjectSettings::isProjectSettingsIntegrityVerified() {
    return
    _projectSettings.isKeyExisting("gravity") &&
    _projectSettings.isKeyExisting("x_resolution") &&
    _projectSettings.isKeyExisting("y_resolution") &&
    _projectSettings.isKeyExisting("fullscreen") &&
    _projectSettings.isKeyExisting("executable_name") &&
    _projectSettings.isKeyExisting("start_scene") &&
    _projectSettings.isKeyExisting("vsync") &&
    _projectSettings.isKeyExisting("multisampling") &&
    _projectSettings.isKeyExisting("samples") &&
    _projectSettings.isKeyExisting("webgpu_major") &&
    _projectSettings.isKeyExisting("webgpu_minor") &&
    _projectSettings.isKeyExisting("dev_build");
}

fs::IniFile &ProjectSettings::projectSettings() {
    return _projectSettings;
}

void ProjectSettings::applyProjectSettings() {
    //todo
}

}
}
}
