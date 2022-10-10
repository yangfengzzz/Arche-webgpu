//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/menu_bar.h"

#include "vox.editor/editor_actions.h"
#include "vox.editor/editor_settings.h"
#include "vox.editor/editor_utils.h"
#include "vox.editor/entity_creation_menu.h"
#include "vox.render/ui/widgets/drags/drag_float.h"
#include "vox.render/ui/widgets/selection/color_edit.h"
#include "vox.render/ui/widgets/sliders/slider_float.h"
#include "vox.render/ui/widgets/sliders/slider_int.h"
#include "vox.render/ui/widgets/texts/text.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
MenuBar::MenuBar() {
    createFileMenu();
    createBuildMenu();
    createWindowMenu();
    createActorsMenu();
    createResourcesMenu();
    createSettingsMenu();
    createLayoutMenu();
    createHelpMenu();
}

void MenuBar::handleShortcuts(float p_deltaTime) {}

void MenuBar::registerPanel(const std::string &p_name, PanelWindow &p_panel) {
    auto &menuItem = _windowMenu->CreateWidget<MenuItem>(p_name, "", true, true);
    menuItem.value_changed_event_ += std::bind(&PanelWindow::SetOpened, &p_panel, std::placeholders::_1);

    _panels.emplace(p_name, std::make_pair(std::ref(p_panel), std::ref(menuItem)));
}

void MenuBar::createFileMenu() {
    auto &fileMenu = CreateWidget<MenuList>("File");
    fileMenu.CreateWidget<MenuItem>("New Scene", "CTRL + N").clicked_event_ +=
            std::bind(&EditorActions::loadEmptyScene, EditorActions::getSingletonPtr());

    fileMenu.CreateWidget<MenuItem>("Save Scene", "CTRL + S").clicked_event_ +=
            std::bind(&EditorActions::saveSceneChanges, EditorActions::getSingletonPtr());

    fileMenu.CreateWidget<MenuItem>("Save Scene As...", "CTRL + SHIFT + S").clicked_event_ +=
            std::bind(&EditorActions::saveAs, EditorActions::getSingletonPtr());

    fileMenu.CreateWidget<MenuItem>("Exit", "ALT + F4").clicked_event_ += [] {
        // EDITOR_CONTEXT(window)->SetShouldClose(true);
    };
}

void MenuBar::createBuildMenu() {
    auto &buildMenu = CreateWidget<MenuList>("Build");
    buildMenu.CreateWidget<MenuItem>("Build game").clicked_event_ +=
            std::bind(&EditorActions::build, EditorActions::getSingletonPtr(), false, false);

    buildMenu.CreateWidget<MenuItem>("Build game and run").clicked_event_ +=
            std::bind(&EditorActions::build, EditorActions::getSingletonPtr(), true, false);

    buildMenu.CreateWidget<Separator>();
    buildMenu.CreateWidget<MenuItem>("Temporary build").clicked_event_ +=
            std::bind(&EditorActions::build, EditorActions::getSingletonPtr(), true, true);
}

void MenuBar::createWindowMenu() {
    _windowMenu = &CreateWidget<MenuList>("Window");
    _windowMenu->CreateWidget<MenuItem>("Close all").clicked_event_ +=
            std::bind(&MenuBar::openEveryWindows, this, false);
    _windowMenu->CreateWidget<MenuItem>("Open all").clicked_event_ += std::bind(&MenuBar::openEveryWindows, this, true);
    _windowMenu->CreateWidget<Separator>();

    /* When the menu is opened, we update which window is marked as "Opened" or "Closed" */
    _windowMenu->clicked_event_ += std::bind(&MenuBar::updateToggleableItems, this);
}

void MenuBar::createActorsMenu() {
    auto &actorsMenu = CreateWidget<MenuList>("Actors");
    EntityCreationMenu::generateEntityCreationMenu(actorsMenu);
}

void MenuBar::createResourcesMenu() {
    auto &resourcesMenu = CreateWidget<MenuList>("Resources");
    resourcesMenu.CreateWidget<MenuItem>("Compile shaders").clicked_event_ +=
            std::bind(&EditorActions::compileShaders, EditorActions::getSingletonPtr());

    resourcesMenu.CreateWidget<MenuItem>("Save materials").clicked_event_ +=
            std::bind(&EditorActions::saveMaterials, EditorActions::getSingletonPtr());
}

void MenuBar::createSettingsMenu() {
    auto &settingsMenu = CreateWidget<MenuList>("Settings");
    settingsMenu.CreateWidget<MenuItem>("Spawn actors at origin", "", true, true).value_changed_event_ +=
            std::bind(&EditorActions::setEntitySpawnAtOrigin, EditorActions::getSingletonPtr(), std::placeholders::_1);

    settingsMenu.CreateWidget<MenuItem>("Vertical Synchronization", "", true, true).value_changed_event_ +=
            [this](bool p_value) {
                // EDITOR_CONTEXT(device)->SetVsync(p_value);
            };

    auto &cameraSpeedMenu = settingsMenu.CreateWidget<MenuList>("Camera Speed");
    {
        cameraSpeedMenu.CreateWidget<SliderInt>(1, 50, 15, SliderOrientation::HORIZONTAL, "Scene View")
                .value_changed_event_ += std::bind(&EditorActions::setSceneViewCameraSpeed,
                                                   EditorActions::getSingletonPtr(), std::placeholders::_1);

        cameraSpeedMenu.CreateWidget<SliderInt>(1, 50, 15, SliderOrientation::HORIZONTAL, "Asset View")
                .value_changed_event_ += std::bind(&EditorActions::setAssetViewCameraSpeed,
                                                   EditorActions::getSingletonPtr(), std::placeholders::_1);
    }

    auto &cameraPositionMenu = settingsMenu.CreateWidget<MenuList>("Reset Camera");
    {
        cameraPositionMenu.CreateWidget<MenuItem>("Scene View").clicked_event_ +=
                std::bind(&EditorActions::resetSceneViewCameraPosition, EditorActions::getSingletonPtr());

        cameraPositionMenu.CreateWidget<MenuItem>("Asset View").clicked_event_ +=
                std::bind(&EditorActions::resetAssetViewCameraPosition, EditorActions::getSingletonPtr());
    }

    auto &viewColors = settingsMenu.CreateWidget<MenuList>("View Colors");
    {
        auto &sceneViewBackground = viewColors.CreateWidget<MenuList>("Scene View Background");
        auto &sceneViewBackgroundPicker =
                sceneViewBackground.CreateWidget<ColorEdit>(false, Color{0.098f, 0.098f, 0.098f});
        sceneViewBackgroundPicker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").GetCamera().SetClearColor({ color.r, color.g, color.b });
        };
        sceneViewBackground.CreateWidget<MenuItem>("Reset").clicked_event_ += [this, &sceneViewBackgroundPicker] {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").GetCamera().SetClearColor({ 0.098f, 0.098f, 0.098f });
            // sceneViewBackgroundPicker.color = { 0.098f, 0.098f, 0.098f };
        };

        auto &sceneViewGrid = viewColors.CreateWidget<MenuList>("Scene View Grid");
        auto &sceneViewGridPicker = sceneViewGrid.CreateWidget<ColorEdit>(false, Color(0.176f, 0.176f, 0.176f));
        sceneViewGridPicker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").SetGridColor({ color.r, color.g, color.b });
        };
        sceneViewGrid.CreateWidget<MenuItem>("Reset").clicked_event_ += [this, &sceneViewGridPicker] {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").SetGridColor(OvMaths::FVector3(0.176f, 0.176f, 0.176f));
            // sceneViewGridPicker.color = OvUI::Types::Color(0.176f, 0.176f, 0.176f);
        };

        auto &assetViewBackground = viewColors.CreateWidget<MenuList>("Asset View Background");
        auto &assetViewBackgroundPicker =
                assetViewBackground.CreateWidget<ColorEdit>(false, Color{0.098f, 0.098f, 0.098f});
        assetViewBackgroundPicker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").GetCamera().SetClearColor({ color.r, color.g, color.b });
        };
        assetViewBackground.CreateWidget<MenuItem>("Reset").clicked_event_ += [this, &assetViewBackgroundPicker] {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").GetCamera().SetClearColor({ 0.098f, 0.098f, 0.098f });
            // assetViewBackgroundPicker.color = { 0.098f, 0.098f, 0.098f };
        };

        auto &assetViewGrid = viewColors.CreateWidget<MenuList>("Asset View Grid");
        auto &assetViewGridPicker = assetViewGrid.CreateWidget<ColorEdit>(false, Color(0.176f, 0.176f, 0.176f));
        assetViewGridPicker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").SetGridColor({ color.r, color.g, color.b });
        };
        assetViewGrid.CreateWidget<MenuItem>("Reset").clicked_event_ += [this, &assetViewGridPicker] {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").SetGridColor(OvMaths::FVector3(0.176f, 0.176f, 0.176f));
            // assetViewGridPicker.color = OvUI::Types::Color(0.176f, 0.176f, 0.176f);
        };
    }

    auto &sceneViewBillboardScaleMenu = settingsMenu.CreateWidget<MenuList>("3D Icons Scales");
    {
        auto &lightBillboardScaleSlider = sceneViewBillboardScaleMenu.CreateWidget<SliderInt>(
                0, 100, static_cast<int>(EditorSettings::LightBillboardScale * 100.0f), SliderOrientation::HORIZONTAL,
                "Lights");
        lightBillboardScaleSlider.value_changed_event_ +=
                [this](int p_value) { EditorSettings::LightBillboardScale = p_value / 100.0f; };
        lightBillboardScaleSlider.format_ = "%d %%";
    }

    auto &snappingMenu = settingsMenu.CreateWidget<MenuList>("Snapping");
    {
        snappingMenu
                .CreateWidget<DragFloat>(0.001f, 999999.0f, EditorSettings::TranslationSnapUnit, 0.05f,
                                         "Translation Unit")
                .value_changed_event_ += [this](float p_value) { EditorSettings::TranslationSnapUnit = p_value; };
        snappingMenu.CreateWidget<DragFloat>(0.001f, 999999.0f, EditorSettings::RotationSnapUnit, 1.0f, "Rotation Unit")
                .value_changed_event_ += [this](float p_value) { EditorSettings::RotationSnapUnit = p_value; };
        snappingMenu.CreateWidget<DragFloat>(0.001f, 999999.0f, EditorSettings::ScalingSnapUnit, 0.05f, "Scaling Unit")
                .value_changed_event_ += [this](float p_value) { EditorSettings::ScalingSnapUnit = p_value; };
    }

    auto &debuggingMenu = settingsMenu.CreateWidget<MenuList>("Debugging");
    {
        debuggingMenu.CreateWidget<MenuItem>("Show geometry bounds", "", true, EditorSettings::ShowGeometryBounds)
                .value_changed_event_ += [this](bool p_value) { EditorSettings::ShowGeometryBounds = p_value; };
        debuggingMenu.CreateWidget<MenuItem>("Show lights bounds", "", true, EditorSettings::ShowLightBounds)
                .value_changed_event_ += [this](bool p_value) { EditorSettings::ShowLightBounds = p_value; };
        auto &subMenu = debuggingMenu.CreateWidget<MenuList>("Frustum culling visualizer...");
        subMenu.CreateWidget<MenuItem>("For geometry", "", true, EditorSettings::ShowGeometryFrustumCullingInSceneView)
                .value_changed_event_ +=
                [this](bool p_value) { EditorSettings::ShowGeometryFrustumCullingInSceneView = p_value; };
        subMenu.CreateWidget<MenuItem>("For lights", "", true, EditorSettings::ShowLightFrustumCullingInSceneView)
                .value_changed_event_ +=
                [this](bool p_value) { EditorSettings::ShowLightFrustumCullingInSceneView = p_value; };
    }
}

void MenuBar::createLayoutMenu() {
    auto &layoutMenu = CreateWidget<MenuList>("Layout");
    layoutMenu.CreateWidget<MenuItem>("Reset").clicked_event_ +=
            std::bind(&EditorActions::resetLayout, EditorActions::getSingletonPtr());
}

void MenuBar::createHelpMenu() {
    auto &helpMenu = CreateWidget<MenuList>("Help");
    helpMenu.CreateWidget<MenuItem>("GitHub").clicked_event_ += [] { openURL("https://github.com/ArcheGraphics"); };
    helpMenu.CreateWidget<MenuItem>("Tutorials").clicked_event_ += [] { openURL("https://arche.graphics/docs/intro"); };
    helpMenu.CreateWidget<MenuItem>("Scripting API").clicked_event_ +=
            [] { openURL("https://arche.graphics/docs/intro"); };
    helpMenu.CreateWidget<Separator>();
    helpMenu.CreateWidget<MenuItem>("Bug Report").clicked_event_ +=
            [] { openURL("https://github.com/ArcheGraphics/Arche-cpp/issues"); };
    helpMenu.CreateWidget<MenuItem>("Feature Request").clicked_event_ +=
            [] { openURL("https://github.com/ArcheGraphics/Arche-cpp/issues"); };
    helpMenu.CreateWidget<Separator>();
    helpMenu.CreateWidget<MenuItem>("Arche.js").clicked_event_ +=
            [] { openURL("https://github.com/ArcheGraphics/Arche.js"); };
    helpMenu.CreateWidget<::vox::ui::Text>("Version: 0.0.1");
}

void MenuBar::updateToggleableItems() {
    for (auto &[name, panel] : _panels) panel.second.get().checked_ = panel.first.get().IsOpened();
}

void MenuBar::openEveryWindows(bool p_state) {
    for (auto &[name, panel] : _panels) panel.first.get().SetOpened(p_state);
}

}  // namespace vox::editor::ui
