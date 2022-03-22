//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "primitive_app.h"
#include "entity.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "image/stb.h"

namespace vox {
namespace {
class MoveScript : public Script {
public:
    MoveScript(Entity* entity): Script(entity) {}
    
    void onUpdate(float deltaTime) override {
        _rTri += 90 * deltaTime;
        entity()->transform->setRotation(0, _rTri, 0);
    }
    
private:
    float _rTri = 0;
};

}

void PrimitiveApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
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

    auto texture = Image::load("Textures/wood.png")->createSampledTexture(_device);
    texturedMaterial->setBaseTexture(texture);
}

}
