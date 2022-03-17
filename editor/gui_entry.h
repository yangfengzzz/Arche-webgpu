//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gui_entry_hpp
#define gui_entry_hpp

#include <string>
#include <webgpu/webgpu_cpp.h>
#include "singleton.h"

#include "editor_component.h"
#include "gui/imgui.h"
#include "imgui_zmo.h"
#include "imgui_node_editor.h"

namespace vox {
class Editor;
namespace control {
class OrbitControl;
}

namespace editor {
class MainMenu;

struct ApplicationStateWindows {
    bool styleEditor = false;
    bool statsWindow = false;
    bool shaderEditorWindow = false;
    bool texturEditorWindow = false;
    bool seaEditor = false;
    bool textureStore = false;
    bool osLisc = false;
    bool filtersManager = false;
    bool foliageManager = false;
    bool supportersTribute = false;
    bool skySettings = false;
    bool modulesManager = false;
    bool lightControls = true;
    bool cameraControls = true;
};

struct ApplicationStateGlobals {
    float mouseSpeed = 25;
    float scrollSpeed = 0.5f;
    float mouseScrollAmount = 0;
    float viewportMousePosX = 0;
    float viewportMousePosY = 0;
    float scale = 1.0f;
    float offset[3];


    int resolution = 256;
    int numberOfNoiseTypes = 3;
    int secondCounter = 0;
    int textureBakeMode = 0;
    int texBakeRes = 1024;

    std::string currentOpenFilePath = "";
    std::string currentBaseModelPath = "";
    std::string kernelsIncludeDir = "";

    float viewportSize[4];
    float hMapC[4];
};

struct ApplicationStateStates {
    bool usingBase = true;
    bool skyboxEnabled = false;
    bool vSync = true;
    bool autoUpdate = false;
    bool mouseButton1, mouseButton2, mouseButton3;
    bool wireFrameMode = false;
    bool reqTexRfrsh = false;
    bool autoSave = false;
    bool exploreMode = false;
    bool iExploreMode = false;
    bool showFoliage = true;
    bool textureBake = false;
    bool useGPUForNormals = true;
    bool postProcess = false;
    bool autoAspectCalcRatio = true;
    std::atomic<bool> ruinning = true;
    std::atomic<bool> remeshing = false;
};

class GUIEntry : public Singleton<GUIEntry> {
public:
    static GUIEntry &getSingleton(void);
    
    static GUIEntry *getSingletonPtr(void);
    
    ApplicationStateWindows windows;
    ApplicationStateStates states;
    ApplicationStateGlobals globals;
    
    GUIEntry();
    
    ~GUIEntry();
    
    void setRootEntity(EntityPtr root);
    
    void setRenderer(Renderer *render);
        
    Camera* sceneCamera{nullptr};
    ImVec2 viewportPos;
    ImVec2 viewportSize;
    
    void update(const wgpu::TextureView& view);
    
public:
    void renderImGui(const wgpu::TextureView& view);
    
    void onBeforeImGuiRender();
    
    void onImGuiRenderEnd();
    
    void showGeneralControls();
    
    void showGizmoControls();
    
    void editTransformPanel(float *cameraView, float *cameraProjection, float *matrix);

    void editTransform(float *cameraView, float *cameraProjection, float *matrix);
    
    void showMainScene(const wgpu::TextureView& view);
    
private:
    EntityPtr _root{nullptr};
    control::OrbitControl* _cameraControl{nullptr};
    
    //selected
    Renderer *_render = nullptr;
    
    //used for gui
    float _fov;
    float _camDistance = 8.f;
    ImGuizmo::OPERATION _currentGizmoOperation = ImGuizmo::TRANSLATE;
    
private:
    MainMenu* _mainMenu{nullptr};
};

}
template<> inline editor::GUIEntry* Singleton<editor::GUIEntry>::msSingleton{nullptr};
}
#endif /* gui_entry_hpp */
