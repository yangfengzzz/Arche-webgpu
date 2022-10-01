//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/gui_app.h"

#include "vox.render/camera.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.render/entity.h"
#include "vox.render/ui/widgets/texts/text_labelled.h"
#include "vox.render/ui/widgets/sliders/slider_float.h"

namespace vox {
class CustomGUI : public ui::Widget {
    void DrawImpl() override {
        ImGui::Text("Custom UI");
        ImGui::Button("Click");
    }
};

void GuiApp::loadScene() {
    _gui->LoadFont("Ruda_Big", "Fonts/Ruda-Bold.ttf", 16);
    _gui->LoadFont("Ruda_Medium", "Fonts/Ruda-Bold.ttf", 14);
    _gui->LoadFont("Ruda_Small", "Fonts/Ruda-Bold.ttf", 12);
    _gui->UseFont("Ruda_Medium");
    _gui->SetEditorLayoutAutosaveFrequency(60.0f);
    _gui->EnableEditorLayoutSave(true);
    _gui->EnableDocking(true);

    _gui->SetCanvas(canvas_);
    canvas_.AddPanel(panel_);
    panel_.CreateWidget<CustomGUI>();
    panel_.CreateWidget<ui::TextLabelled>("hello", "world");
    panel_.CreateWidget<ui::SliderFloat>();
    
    auto scene = _sceneManager->currentScene();
    auto root_entity = scene->createRootEntity();

    auto camera_entity = root_entity->createChild();
    camera_entity->transform->setPosition(10, 10, 10);
    camera_entity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = camera_entity->addComponent<Camera>();
    camera_entity->addComponent<control::OrbitControl>();

    scene->play();
}

}  // namespace vox
