//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "menu_bar.h"

#include "ui/widgets/visual/separator.h"
#include "ui/widgets/sliders/slider_int.h"
#include "ui/widgets/sliders/slider_float.h"
#include "ui/widgets/drags/drag_float.h"
#include "ui/widgets/selection/color_edit.h"
#include "ui/widgets/texts/text.h"

#include "editor_utils.h"
#include "editor_actions.h"
#include "entity_creation_menu.h"
#include "editor_settings.h"

namespace vox {
namespace editor {
namespace ui {
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

void MenuBar::handleShortcuts(float p_deltaTime) {
    
}

void MenuBar::registerPanel(const std::string &p_name, PanelWindow &p_panel) {
    auto &menuItem = _windowMenu->createWidget<MenuItem>(p_name, "", true, true);
    menuItem.valueChangedEvent += std::bind(&PanelWindow::setOpened, &p_panel, std::placeholders::_1);
    
    _panels.emplace(p_name, std::make_pair(std::ref(p_panel), std::ref(menuItem)));
}

void MenuBar::createFileMenu() {
    auto &fileMenu = createWidget<MenuList>("File");
    fileMenu.createWidget<MenuItem>("New Scene", "CTRL + N").clickedEvent +=
    std::bind(&EditorActions::loadEmptyScene, EditorActions::getSingletonPtr());
    
    fileMenu.createWidget<MenuItem>("Save Scene", "CTRL + S").clickedEvent +=
    std::bind(&EditorActions::saveSceneChanges, EditorActions::getSingletonPtr());
    
    fileMenu.createWidget<MenuItem>("Save Scene As...", "CTRL + SHIFT + S").clickedEvent +=
    std::bind(&EditorActions::saveAs, EditorActions::getSingletonPtr());
    
    fileMenu.createWidget<MenuItem>("Exit", "ALT + F4").clickedEvent += [] {
        // EDITOR_CONTEXT(window)->SetShouldClose(true);
    };
}

void MenuBar::createBuildMenu() {
    auto &buildMenu = createWidget<MenuList>("Build");
    buildMenu.createWidget<MenuItem>("Build game").clickedEvent +=
    std::bind(&EditorActions::build, EditorActions::getSingletonPtr(), false, false);
    
    buildMenu.createWidget<MenuItem>("Build game and run").clickedEvent +=
    std::bind(&EditorActions::build, EditorActions::getSingletonPtr(), true, false);
    
    buildMenu.createWidget<Separator>();
    buildMenu.createWidget<MenuItem>("Temporary build").clickedEvent +=
    std::bind(&EditorActions::build, EditorActions::getSingletonPtr(), true, true);
}

void MenuBar::createWindowMenu() {
    _windowMenu = &createWidget<MenuList>("Window");
    _windowMenu->createWidget<MenuItem>("Close all").clickedEvent += std::bind(&MenuBar::openEveryWindows, this, false);
    _windowMenu->createWidget<MenuItem>("Open all").clickedEvent += std::bind(&MenuBar::openEveryWindows, this, true);
    _windowMenu->createWidget<Separator>();
    
    /* When the menu is opened, we update which window is marked as "Opened" or "Closed" */
    _windowMenu->clickedEvent += std::bind(&MenuBar::updateToggleableItems, this);
}

void MenuBar::createActorsMenu() {
    auto &actorsMenu = createWidget<MenuList>("Actors");
    EntityCreationMenu::generateEntityCreationMenu(actorsMenu);
}

void MenuBar::createResourcesMenu() {
    auto &resourcesMenu = createWidget<MenuList>("Resources");
    resourcesMenu.createWidget<MenuItem>("Compile shaders").clickedEvent +=
    std::bind(&EditorActions::compileShaders, EditorActions::getSingletonPtr());
    
    resourcesMenu.createWidget<MenuItem>("Save materials").clickedEvent +=
    std::bind(&EditorActions::saveMaterials, EditorActions::getSingletonPtr());
}

void MenuBar::createSettingsMenu() {
    auto &settingsMenu = createWidget<MenuList>("Settings");
    settingsMenu.createWidget<MenuItem>("Spawn actors at origin", "", true, true).valueChangedEvent +=
    std::bind(&EditorActions::setEntitySpawnAtOrigin, EditorActions::getSingletonPtr(), std::placeholders::_1);
    
    settingsMenu.createWidget<MenuItem>("Vertical Synchronization", "", true, true).valueChangedEvent += [this](bool p_value) {
        // EDITOR_CONTEXT(device)->SetVsync(p_value);
    };
    
    auto &cameraSpeedMenu = settingsMenu.createWidget<MenuList>("Camera Speed");
    {
        cameraSpeedMenu.createWidget<SliderInt>(1, 50, 15, SliderOrientation::HORIZONTAL, "Scene View").valueChangedEvent +=
        std::bind(&EditorActions::setSceneViewCameraSpeed, EditorActions::getSingletonPtr(), std::placeholders::_1);
        
        cameraSpeedMenu.createWidget<SliderInt>(1, 50, 15, SliderOrientation::HORIZONTAL, "Asset View").valueChangedEvent +=
        std::bind(&EditorActions::setAssetViewCameraSpeed, EditorActions::getSingletonPtr(), std::placeholders::_1);
    }
    
    auto &cameraPositionMenu = settingsMenu.createWidget<MenuList>("Reset Camera");
    {
        cameraPositionMenu.createWidget<MenuItem>("Scene View").clickedEvent +=
        std::bind(&EditorActions::resetSceneViewCameraPosition, EditorActions::getSingletonPtr());
        
        cameraPositionMenu.createWidget<MenuItem>("Asset View").clickedEvent +=
        std::bind(&EditorActions::resetAssetViewCameraPosition, EditorActions::getSingletonPtr());
    }
    
    auto &viewColors = settingsMenu.createWidget<MenuList>("View Colors");
    {
        auto &sceneViewBackground = viewColors.createWidget<MenuList>("Scene View Background");
        auto &sceneViewBackgroundPicker = sceneViewBackground.createWidget<ColorEdit>(false, Color{0.098f, 0.098f, 0.098f});
        sceneViewBackgroundPicker.colorChangedEvent += [this](const auto &color) {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").GetCamera().SetClearColor({ color.r, color.g, color.b });
        };
        sceneViewBackground.createWidget<MenuItem>("Reset").clickedEvent += [this, &sceneViewBackgroundPicker] {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").GetCamera().SetClearColor({ 0.098f, 0.098f, 0.098f });
            // sceneViewBackgroundPicker.color = { 0.098f, 0.098f, 0.098f };
        };
        
        auto &sceneViewGrid = viewColors.createWidget<MenuList>("Scene View Grid");
        auto &sceneViewGridPicker = sceneViewGrid.createWidget<ColorEdit>(false, Color(0.176f, 0.176f, 0.176f));
        sceneViewGridPicker.colorChangedEvent += [this](const auto &color) {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").SetGridColor({ color.r, color.g, color.b });
        };
        sceneViewGrid.createWidget<MenuItem>("Reset").clickedEvent += [this, &sceneViewGridPicker] {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").SetGridColor(OvMaths::FVector3(0.176f, 0.176f, 0.176f));
            // sceneViewGridPicker.color = OvUI::Types::Color(0.176f, 0.176f, 0.176f);
        };
        
        auto &assetViewBackground = viewColors.createWidget<MenuList>("Asset View Background");
        auto &assetViewBackgroundPicker = assetViewBackground.createWidget<ColorEdit>(false, Color{0.098f, 0.098f, 0.098f});
        assetViewBackgroundPicker.colorChangedEvent += [this](const auto &color) {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").GetCamera().SetClearColor({ color.r, color.g, color.b });
        };
        assetViewBackground.createWidget<MenuItem>("Reset").clickedEvent += [this, &assetViewBackgroundPicker] {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").GetCamera().SetClearColor({ 0.098f, 0.098f, 0.098f });
            // assetViewBackgroundPicker.color = { 0.098f, 0.098f, 0.098f };
        };
        
        auto &assetViewGrid = viewColors.createWidget<MenuList>("Asset View Grid");
        auto &assetViewGridPicker = assetViewGrid.createWidget<ColorEdit>(false, Color(0.176f, 0.176f, 0.176f));
        assetViewGridPicker.colorChangedEvent += [this](const auto &color) {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").SetGridColor({ color.r, color.g, color.b });
        };
        assetViewGrid.createWidget<MenuItem>("Reset").clickedEvent += [this, &assetViewGridPicker] {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").SetGridColor(OvMaths::FVector3(0.176f, 0.176f, 0.176f));
            // assetViewGridPicker.color = OvUI::Types::Color(0.176f, 0.176f, 0.176f);
        };
    }
    
    auto &sceneViewBillboardScaleMenu = settingsMenu.createWidget<MenuList>("3D Icons Scales");
    {
        auto &lightBillboardScaleSlider =
        sceneViewBillboardScaleMenu.createWidget<SliderInt>(0, 100, static_cast<int>(EditorSettings::LightBillboardScale * 100.0f),
                                                            SliderOrientation::HORIZONTAL, "Lights");
        lightBillboardScaleSlider.valueChangedEvent += [this](int p_value) {
            EditorSettings::LightBillboardScale = p_value / 100.0f;
        };
        lightBillboardScaleSlider.format = "%d %%";
    }
    
    auto &snappingMenu = settingsMenu.createWidget<MenuList>("Snapping");
    {
        snappingMenu.createWidget<DragFloat>(0.001f, 999999.0f, EditorSettings::TranslationSnapUnit, 0.05f,
                                             "Translation Unit").valueChangedEvent += [this](float p_value) {
            EditorSettings::TranslationSnapUnit = p_value;
        };
        snappingMenu.createWidget<DragFloat>(0.001f, 999999.0f, EditorSettings::RotationSnapUnit, 1.0f,
                                             "Rotation Unit").valueChangedEvent += [this](float p_value) {
            EditorSettings::RotationSnapUnit = p_value;
        };
        snappingMenu.createWidget<DragFloat>(0.001f, 999999.0f, EditorSettings::ScalingSnapUnit, 0.05f,
                                             "Scaling Unit").valueChangedEvent += [this](float p_value) {
            EditorSettings::ScalingSnapUnit = p_value;
        };
    }
    
    auto &debuggingMenu = settingsMenu.createWidget<MenuList>("Debugging");
    {
        debuggingMenu.createWidget<MenuItem>("Show geometry bounds", "", true,
                                             EditorSettings::ShowGeometryBounds).valueChangedEvent += [this](bool p_value) {
            EditorSettings::ShowGeometryBounds = p_value;
        };
        debuggingMenu.createWidget<MenuItem>("Show lights bounds", "", true,
                                             EditorSettings::ShowLightBounds).valueChangedEvent += [this](bool p_value) {
            EditorSettings::ShowLightBounds = p_value;
        };
        auto &subMenu = debuggingMenu.createWidget<MenuList>("Frustum culling visualizer...");
        subMenu.createWidget<MenuItem>("For geometry", "", true,
                                       EditorSettings::ShowGeometryFrustumCullingInSceneView).valueChangedEvent += [this](bool p_value) {
            EditorSettings::ShowGeometryFrustumCullingInSceneView = p_value;
        };
        subMenu.createWidget<MenuItem>("For lights", "", true,
                                       EditorSettings::ShowLightFrustumCullingInSceneView).valueChangedEvent += [this](bool p_value) {
            EditorSettings::ShowLightFrustumCullingInSceneView = p_value;
        };
    }
}

void MenuBar::createLayoutMenu() {
    auto &layoutMenu = createWidget<MenuList>("Layout");
    layoutMenu.createWidget<MenuItem>("Reset").clickedEvent +=
    std::bind(&EditorActions::resetLayout, EditorActions::getSingletonPtr());
}

void MenuBar::createHelpMenu() {
    auto &helpMenu = createWidget<MenuList>("Help");
    helpMenu.createWidget<MenuItem>("GitHub").clickedEvent += [] {
        openURL("https://github.com/ArcheGraphics");
    };
    helpMenu.createWidget<MenuItem>("Tutorials").clickedEvent += [] {
        openURL("https://arche.graphics/docs/intro");
    };
    helpMenu.createWidget<MenuItem>("Scripting API").clickedEvent += [] {
        openURL("https://arche.graphics/docs/intro");
    };
    helpMenu.createWidget<Separator>();
    helpMenu.createWidget<MenuItem>("Bug Report").clickedEvent += [] {
        openURL("https://github.com/ArcheGraphics/Arche-cpp/issues");
    };
    helpMenu.createWidget<MenuItem>("Feature Request").clickedEvent += [] {
        openURL("https://github.com/ArcheGraphics/Arche-cpp/issues");
    };
    helpMenu.createWidget<Separator>();
    helpMenu.createWidget<MenuItem>("Arche.js").clickedEvent += [] {
        openURL("https://github.com/ArcheGraphics/Arche.js");
    };
    helpMenu.createWidget<::vox::ui::Text>("Version: 0.0.1");
}

void MenuBar::updateToggleableItems() {
    for (auto&[name, panel]: _panels)
        panel.second.get().checked = panel.first.get().isOpened();
}

void MenuBar::openEveryWindows(bool p_state) {
    for (auto&[name, panel]: _panels)
        panel.first.get().setOpened(p_state);
}


}
}
}
