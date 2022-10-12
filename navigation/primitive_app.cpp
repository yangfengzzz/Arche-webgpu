//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "navigation/primitive_app.h"

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"

namespace vox {
namespace {
class MoveScript : public Script {
public:
    explicit MoveScript(Entity* entity) : Script(entity) {}

    void onUpdate(float deltaTime) override {
        _rTri += 90 * deltaTime;
        entity()->transform->setRotation(0, _rTri, 0);
    }

private:
    float _rTri = 0;
};

}  // namespace

void PrimitiveApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;

    auto cubeEntity = rootEntity->createChild();
    cubeEntity->addComponent<MoveScript>();
    auto renderer = cubeEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(_device, 1));
    auto material = std::make_shared<BlinnPhongMaterial>(_device);
    material->setBaseColor(Color(0.4, 0.6, 0.6));
    renderer->setMaterial(material);

    auto planeEntity = rootEntity->createChild();
    planeEntity->transform->setPosition(0, 5, 0);
    auto planeRenderer = planeEntity->addComponent<MeshRenderer>();
    planeRenderer->setMesh(PrimitiveMesh::createSphere(_device, 1));
    auto texturedMaterial = std::make_shared<UnlitMaterial>(_device);
    planeRenderer->setMaterial(texturedMaterial);

    texturedMaterial->setBaseTexture(ImageManager::GetSingleton().loadTexture("Textures/wood.png"));

    scene->play();
}

}  // namespace vox
