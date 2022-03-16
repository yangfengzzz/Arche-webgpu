//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui_entry.h"

// vox.render
#include "editor.h"
#include "entity.h"
#include "camera.h"
#include "renderer.h"
#include "controls/orbit_control.h"

// editor component
#include "component/main_menu.h"
#include "component/app_styles.h"

namespace vox {
namespace editor {
GUIEntry::GUIEntry(Entity* entity):
Script(entity) {
    auto cameraEntity = entity->createChild("scene_camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _camera = cameraEntity->addComponent<Camera>();
    _fov = _camera->fieldOfView();
    
    _cameraControl = cameraEntity->addComponent<control::OrbitControl>();
    
    _mainMenu = new MainMenu(this);
}

GUIEntry::~GUIEntry() {
    delete _mainMenu;
}

void GUIEntry::setApp(vox::Editor* app) {
    _app = app;
}

void GUIEntry::setRenderer(Renderer *render) {
    _render = render;
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
        showGizmoControls();
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

//MARK: - Gizmos
void GUIEntry::showGizmoControls() {
    ImGuizmo::SetOrthographic(_camera->isOrthographic());
    ImGuizmo::BeginFrame();
    
    ImGui::Begin("Camera Controls");
    ImGui::Checkbox("Auto Calculate Aspect Ratio", &states.autoAspectCalcRatio);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Separator();
    
    ImGui::Text("Camera");
    ImGui::SliderFloat("Fov", &_fov, 20.f, 110.f);
    _camera->setFieldOfView(_fov);
    
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("X: %f Y: %f", io.MousePos.x, io.MousePos.y);
    if (ImGuizmo::IsUsing()) {
        ImGui::Text("Using gizmo");
    } else {
        ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");
        ImGui::SameLine();
        ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");
        ImGui::SameLine();
        ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");
        ImGui::SameLine();
        ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
    }
    ImGui::Separator();
    
    if (_render != nullptr) {
        if (ImGuizmo::IsOver()) {
            _cameraControl->setEnabled(false);
        }
        
        auto cameraProjection = _camera->projectionMatrix();
        auto cameraView = _camera->viewMatrix();
        auto modelMat = _render->entity()->transform->localMatrix();
        editTransform(cameraView.data(), cameraProjection.data(), modelMat.data(), true);
        _render->entity()->transform->setLocalMatrix(modelMat);
        cameraView.invert();
        _camera->entity()->transform->setWorldMatrix(cameraView);
    }
    
    ImGui::End();
}

void GUIEntry::editTransform(float *cameraView, float *cameraProjection, float *matrix, bool editTransformDecomposition) {
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = {1.f, 1.f, 1.f};
    static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
    static float boundsSnap[] = {0.1f, 0.1f, 0.1f};
    static bool boundSizing = false;
    static bool boundSizingSnap = false;
    
    if (editTransformDecomposition) {
        if (ImGui::IsKeyPressed(90))
            _currentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(69))
            _currentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(82)) // r Key
            _currentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", _currentGizmoOperation == ImGuizmo::TRANSLATE))
            _currentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", _currentGizmoOperation == ImGuizmo::ROTATE))
            _currentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", _currentGizmoOperation == ImGuizmo::SCALE))
            _currentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Universal", _currentGizmoOperation == ImGuizmo::UNIVERSAL))
            _currentGizmoOperation = ImGuizmo::UNIVERSAL;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);
        
        if (_currentGizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed(83))
            useSnap = !useSnap;
        ImGui::Checkbox("##", &useSnap);
        ImGui::SameLine();
        
        switch (_currentGizmoOperation) {
            case ImGuizmo::TRANSLATE:
                ImGui::InputFloat3("Snap", &snap[0]);
                break;
            case ImGuizmo::ROTATE:
                ImGui::InputFloat("Angle Snap", &snap[0]);
                break;
            case ImGuizmo::SCALE:
                ImGui::InputFloat("Scale Snap", &snap[0]);
                break;
            default:
                break;
        }
        ImGui::Checkbox("Bound Sizing", &boundSizing);
        if (boundSizing) {
            ImGui::PushID(3);
            ImGui::Checkbox("", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }
    }
    
    ImGuiIO &io = ImGui::GetIO();
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    
    ImGuizmo::Manipulate(cameraView, cameraProjection, _currentGizmoOperation, mCurrentGizmoMode,
                         matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
    
    ImGuizmo::ViewManipulate(cameraView, _camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);
}

// MARK: - Viewport
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
