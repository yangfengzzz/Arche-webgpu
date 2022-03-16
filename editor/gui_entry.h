//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gui_entry_hpp
#define gui_entry_hpp

#include "script.h"

namespace vox {
namespace editor {
class MainMenu;

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

class GUIEntry: public Script {
public:
    GUIEntry(Entity* entity);
    
    ~GUIEntry();
    
    void onUpdate(float deltaTime) override;
    
    void renderImGui();
    
    void onBeforeImGuiRender();
    
    void onImGuiRenderEnd();
    
    void showGeneralControls();
    
private:
    MainMenu* _mainMenu{nullptr};
    
    ApplicationStateStates states;
    ApplicationStateGlobals globals;
};

}
}
#endif /* gui_entry_hpp */
