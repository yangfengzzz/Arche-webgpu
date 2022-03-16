//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui_entry.h"
#include "editor.h"
#include "gui/imgui.h"
#include "component/main_menu.h"
#include "component/app_styles.h"
#include "camera.h"

namespace vox {
namespace editor {
GUIEntry::GUIEntry(Entity* entity):
Script(entity) {
    auto cameraEntity = entity->createChild("scene_camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _camera = cameraEntity->addComponent<Camera>();
    _cameraControl = cameraEntity->addComponent<control::OrbitControl>();
    
    _mainMenu = new MainMenu(this);
}

GUIEntry::~GUIEntry() {
    delete _mainMenu;
}

void GUIEntry::setApp(vox::Editor* app) {
    _app = app;
}

Camera* GUIEntry::camera() const {
    return _camera;
}

void GUIEntry::onUpdate(float deltaTime) {
    renderImGui();
}

void GUIEntry::renderImGui() {
    ImGui::NewFrame();
    
    onBeforeImGuiRender();
    _mainMenu->showMainMenu();
    showGeneralControls();
    
    if (windows.cameraControls) {
        ImGui::Begin("Camera Controls");
//        appState->cameras.main.ShowSettings();
        ImGui::Checkbox("Auto Calculate Aspect Ratio", &states.autoAspectCalcRatio);
        ImGui::End();
    }
    
    if (states.autoAspectCalcRatio) {
        _camera->setAspectRatio(globals.viewportSize[0] / globals.viewportSize[1]);
    }
    
    showMainScene();
    
    if (windows.styleEditor) {
        showStyleEditor(&windows.styleEditor);
    }
    
    onImGuiRenderEnd();
    
    ImGui::EndFrame();
    ImGui::Render();
}

void GUIEntry::onBeforeImGuiRender() {
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    
    if (opt_fullscreen) {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.1f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();
    
    if (opt_fullscreen) {
        ImGui::PopStyleVar(2);
    }
    
    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    ImGuiStyle &style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    
    style.WindowMinSize.x = minWinSizeX;
}

void GUIEntry::onImGuiRenderEnd() {
    ImGui::End();
}

void GUIEntry::showGeneralControls() {
    ImGui::Begin("General Controls");
    {
        ImGui::Checkbox("VSync ", &states.vSync);
    }
    ImGui::DragFloat("Mouse Speed", &globals.mouseSpeed);
    ImGui::DragFloat("Zoom Speed", &globals.scrollSpeed);
    
    if (ImGui::Button("Exit")) {
        exit(0);
    }
    
    ImGui::End();
}

void GUIEntry::showMainScene() {
    auto viewportOffset = ImGui::GetWindowPos();
    ImGui::Begin("Viewport");
    {
        ImGui::BeginChild("MainRender");
        
        if (ImGui::IsWindowHovered()) {
            _cameraControl->setEnabled(true);
            globals.viewportMousePosX = ImGui::GetIO().MousePos.x - viewportOffset.x;
            globals.viewportMousePosY = ImGui::GetIO().MousePos.y - viewportOffset.y;
        } else {
            _cameraControl->setEnabled(false);
        }
        
        ImVec2 wsize = ImGui::GetWindowSize();
        globals.viewportSize[0] = wsize.x;
        globals.viewportSize[1] = wsize.y;
        
        ImGui::Image((ImTextureID) _app->sceneTextureView().Get(), wsize);
        
        ImGui::EndChild();
    }
    ImGui::End();
}

}
}
