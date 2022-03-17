//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "framebuffer_picker_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "lighting/point_light.h"
#include "controls/orbit_control.h"
#include "gui.h"

namespace vox {
void FramebufferPickerApp::loadScene() {
    _gui = std::make_unique<GUI>(_renderContext.get());
    
    u = std::uniform_real_distribution<float>(0, 1);
    _scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
    
    auto rootEntity = _scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
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
    
    // create sphere test entity
    float radius = 1.25;
    auto sphereEntity = rootEntity->createChild("SphereEntity");
    sphereEntity->transform->setPosition(Point3F(-5, 0, 0));
    auto sphereRenderer = sphereEntity->addComponent<MeshRenderer>();
    auto sphereMtl = std::make_shared<BlinnPhongMaterial>(_device);
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    sphereMtl->setBaseColor(Color(u(e), u(e), u(e), 1));
    sphereRenderer->setMesh(PrimitiveMesh::createSphere(_device, radius));
    sphereRenderer->setMaterial(sphereMtl);
}

void FramebufferPickerApp::pickFunctor(Renderer *renderer, MeshPtr mesh) {
    if (renderer && mesh) {
        static_cast<BlinnPhongMaterial *>(renderer->getMaterial().get())->setBaseColor(Color(u(e), u(e), u(e), 1));
    }
}

void FramebufferPickerApp::editorUpdate(const wgpu::TextureView& view) {
    ImGui::NewFrame();
    ImGui::Begin("Panel");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Image((ImTextureID)view.Get(), ImGui::GetWindowSize());
    ImGui::End();
    
    ImGui::Render();
}

void FramebufferPickerApp::inputEvent(const InputEvent &inputEvent) {
    EditorApplication::inputEvent(inputEvent);
    
    if (inputEvent.source() == EventSource::Mouse) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(inputEvent);
        if (mouse_button.action() == MouseAction::Down) {
            pick(mouse_button.pos_x(), mouse_button.pos_y());
        }
    }
}

}
