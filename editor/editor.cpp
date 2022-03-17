//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "physics/static_collider.h"
#include "physics/dynamic_collider.h"
#include "physics/shape/box_collider_shape.h"
#include "physics/shape/sphere_collider_shape.h"
#include "lighting/point_light.h"

#include "grid.h"
#include <random>

namespace vox {
void Editor::loadScene() {
    _scene->background.solidColor = Color(0.9, 0.9, 0.9, 1.0);
    _scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
    
    auto rootEntity = _scene->createRootEntity();
    rootEntity->addComponent<editor::Grid>();

    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    // create box test entity
    float cubeSize = 2.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<BlinnPhongMaterial>(_device);
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxMtl->setBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    boxRenderer->setMesh(PrimitiveMesh::createCuboid(_device, cubeSize, cubeSize, cubeSize));
    boxRenderer->setMaterial(boxMtl);
    
    _gui = std::make_unique<GUI>(_renderContext.get());
    _entry = std::make_unique<editor::GUIEntry>();
    _entry->setApp(this);
}

void Editor::pickFunctor(Renderer *renderer, MeshPtr mesh) {
    if (renderer && mesh) {
        _entry->setRenderer(renderer);
    }
}

void Editor::inputEvent(const InputEvent &inputEvent) {
    EditorApplication::inputEvent(inputEvent);
    
    if (inputEvent.source() == EventSource::Mouse) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(inputEvent);
        if (mouse_button.action() == MouseAction::Down) {
            auto width = _mainCamera->width();
            auto height = _mainCamera->height();
            
            auto pickerUnitPosX = (mouse_button.pos_x() - _entry->viewportPos.x) / _entry->viewportSize.x;
            auto pickerUnitPosY = (mouse_button.pos_y() - _entry->viewportPos.y) / _entry->viewportSize.y;

            if (pickerUnitPosX <= 1 && pickerUnitPosX > 0 && pickerUnitPosY <= 1 && pickerUnitPosY > 0) {
                pick(pickerUnitPosX * width, pickerUnitPosY * height);
            }
        }
    }
}

void Editor::editorUpdate() {
    _entry->update();
}

}
