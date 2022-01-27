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
#include "camera.h"
#include "controls/orbit_control.h"

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

void PrimitiveApp::loadScene(uint32_t width, uint32_t height) {
    auto rootEntity = _scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    auto cubeEntity = rootEntity->createChild();
    cubeEntity->addComponent<MoveScript>();
    auto renderer = cubeEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(_device, 1));
    auto material = std::make_shared<UnlitMaterial>(_device);
    renderer->setMaterial(material);
    
    auto sphereEntity = rootEntity->createChild();
    sphereEntity->transform->setPosition(0, 5, 0);
    sphereEntity->addComponent<MoveScript>();
    auto sphereRenderer = sphereEntity->addComponent<MeshRenderer>();
    sphereRenderer->setMesh(PrimitiveMesh::createSphere(_device, 1));
    sphereRenderer->setMaterial(material);
}

}
