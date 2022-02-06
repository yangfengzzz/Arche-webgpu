//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gizmo_hpp
#define gizmo_hpp

#include "script.h"
#include "gui/imgui.h"
#include "editor_component.h"
#include "imgui_zmo.h"
#include "imgui_node_editor.h"
#include <vector>

namespace vox {
namespace picker {
class FramebufferPicker;
}
namespace control {
class OrbitControl;
}

namespace editor {
class GUIEntry : public Script {
public:
    GUIEntry(Entity *entity);
    
    ~GUIEntry();
    
    void setRenderer(Renderer *render);
    
    void onUpdate(float deltaTime) override;
    
    void addEditorComponent(std::unique_ptr<EditorComponent> &&component);
    
    void removeEditorComponent(EditorComponent *component);
    
private:
    void editTransform(float *cameraView, float *cameraProjection, float *matrix, bool editTransformDecomposition);
    
private:
    void imGuiEx_BeginColumn();
    
    void imGuiEx_NextColumn();
    
    void imGuiEx_EndColumn();
    
    void nodeEditor();
    
private:
    Camera *_camera = nullptr;
    picker::FramebufferPicker *_picker = nullptr;
    control::OrbitControl *_controller = nullptr;
    
    //selected
    Renderer *_render = nullptr;
    
    //used for gui
    float _fov;
    float _camDistance = 8.f;
    ImGuizmo::OPERATION _currentGizmoOperation = ImGuizmo::TRANSLATE;
    
private:
    bool _showEditor = false;
    
    // Struct to hold basic information about connection between
    // pins. Note that connection (aka. link) has its own ID.
    // This is useful later with dealing with selections, deletion
    // or other operations.
    struct LinkInfo {
        NodeEditor::LinkId Id;
        NodeEditor::PinId InputId;
        NodeEditor::PinId OutputId;
    };
    
    NodeEditor::EditorContext *_context = nullptr;    // Editor context, required to trace a editor state.
    bool _firstFrame = true;    // Flag set for first frame only, some action need to be executed once.
    ImVector<LinkInfo> _links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
    int _nextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.
    
private:
    std::vector<std::unique_ptr<EditorComponent>> _editorScripts;
};

}
}

#endif /* gui_entry_hpp */
