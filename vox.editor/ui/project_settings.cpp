//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/project_settings.h"

#include "vox.render/ui/gui_drawer.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/layout/columns.h"
#include "vox.render/ui/widgets/layout/group_collapsable.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
ProjectSettings::ProjectSettings(const std::string &p_title,
                                 bool p_opened,
                                 const PanelWindowSettings &p_windowSettings,
                                 const std::string &p_projectPath,
                                 const std::string &p_projectName)
    : PanelWindow(p_title, p_opened, p_windowSettings), _projectSettings(p_projectPath + p_projectName + ".project") {
    auto &saveButton = CreateWidget<ButtonSimple>("Apply");
    saveButton.idle_background_color_ = {0.0f, 0.5f, 0.0f};
    saveButton.clicked_event_ += [this] {
        applyProjectSettings();
        _projectSettings.Rewrite();
    };

    saveButton.line_break_ = false;

    auto &resetButton = CreateWidget<ButtonSimple>("Reset");
    resetButton.idle_background_color_ = {0.5f, 0.0f, 0.0f};
    resetButton.clicked_event_ += [this] { resetProjectSettings(); };

    CreateWidget<Separator>();

    {
        /* Physics settings */
        auto &root = CreateWidget<GroupCollapsable>("Physics");
        auto &columns = root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawScalar<float>(columns, "Gravity", generateGatherer<float>("gravity"),
                                     generateProvider<float>("gravity"), 0.1f, GuiDrawer::min_float_,
                                     GuiDrawer::max_float_);
    }

    {
        /* Build settings */
        auto &generationRoot = CreateWidget<GroupCollapsable>("Build");
        auto &columns = generationRoot.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawBoolean(columns, "Development build", generateGatherer<bool>("dev_build"),
                               generateProvider<bool>("dev_build"));
    }

    {
        /* Windowing settings */
        auto &windowingRoot = CreateWidget<GroupCollapsable>("Windowing");
        auto &columns = windowingRoot.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawScalar<int>(columns, "Resolution X", generateGatherer<int>("x_resolution"),
                                   generateProvider<int>("x_resolution"), 1, 0, 10000);
        GuiDrawer::DrawScalar<int>(columns, "Resolution Y", generateGatherer<int>("y_resolution"),
                                   generateProvider<int>("y_resolution"), 1, 0, 10000);
        GuiDrawer::DrawBoolean(columns, "Fullscreen", generateGatherer<bool>("fullscreen"),
                               generateProvider<bool>("fullscreen"));
        GuiDrawer::DrawString(columns, "Executable name", generateGatherer<std::string>("executable_name"),
                              generateProvider<std::string>("executable_name"));
    }

    {
        /* Rendering settings */
        auto &renderingRoot = CreateWidget<GroupCollapsable>("Rendering");
        auto &columns = renderingRoot.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawBoolean(columns, "Vertical Sync.", generateGatherer<bool>("vsync"),
                               generateProvider<bool>("vsync"));
        GuiDrawer::DrawBoolean(columns, "Multi-sampling", generateGatherer<bool>("multisampling"),
                               generateProvider<bool>("multisampling"));
        GuiDrawer::DrawScalar<int>(columns, "Samples", generateGatherer<int>("samples"),
                                   generateProvider<int>("samples"), 1, 2, 16);
        GuiDrawer::DrawScalar<int>(columns, "OpenGL Major", generateGatherer<int>("opengl_major"),
                                   generateProvider<int>("opengl_major"), 1, 3, 4);
        GuiDrawer::DrawScalar<int>(columns, "OpenGL Minor", generateGatherer<int>("opengl_minor"),
                                   generateProvider<int>("opengl_minor"), 1, 0, 6);
    }

    {
        /* Scene Management settings */
        auto &gameRoot = CreateWidget<GroupCollapsable>("Scene Management");
        auto &columns = gameRoot.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawDdstring(columns, "Start scene", generateGatherer<std::string>("start_scene"),
                                generateProvider<std::string>("start_scene"), "File");
    }
}

void ProjectSettings::resetProjectSettings() {
    _projectSettings.RemoveAll();
    _projectSettings.Add<float>("gravity", -9.81f);
    _projectSettings.Add<int>("x_resolution", 1280);
    _projectSettings.Add<int>("y_resolution", 720);
    _projectSettings.Add<bool>("fullscreen", false);
    _projectSettings.Add<std::string>("executable_name", "Game");
    _projectSettings.Add<std::string>("start_scene", "Scene.ovscene");
    _projectSettings.Add<bool>("vsync", true);
    _projectSettings.Add<bool>("multisampling", false);
    _projectSettings.Add<int>("samples", 1);
    _projectSettings.Add<int>("webgpu_major", 0);
    _projectSettings.Add<int>("webgpu_minor", 1);
    _projectSettings.Add<bool>("dev_build", true);
}

bool ProjectSettings::isProjectSettingsIntegrityVerified() {
    return _projectSettings.IsKeyExisting("gravity") && _projectSettings.IsKeyExisting("x_resolution") &&
           _projectSettings.IsKeyExisting("y_resolution") && _projectSettings.IsKeyExisting("fullscreen") &&
           _projectSettings.IsKeyExisting("executable_name") && _projectSettings.IsKeyExisting("start_scene") &&
           _projectSettings.IsKeyExisting("vsync") && _projectSettings.IsKeyExisting("multisampling") &&
           _projectSettings.IsKeyExisting("samples") && _projectSettings.IsKeyExisting("webgpu_major") &&
           _projectSettings.IsKeyExisting("webgpu_minor") && _projectSettings.IsKeyExisting("dev_build");
}

fs::IniFile &ProjectSettings::projectSettings() { return _projectSettings; }

void ProjectSettings::applyProjectSettings() {
    // todo
}

}  // namespace vox
