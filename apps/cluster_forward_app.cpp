//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cluster_forward_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "lighting/point_light.h"
#include "lighting/cluster_debug_material.h"
#include <random>

namespace vox {
namespace {
class MoveScript : public Script {
    Point3F pos = Point3F(-5, 0, 0);
    float vel = 4;
    int8_t velSign = -1;
    
public:
    MoveScript(Entity *entity) : Script(entity) {
    }
    
    void onUpdate(float deltaTime) override {
        if (pos.x >= 5) {
            velSign = -1;
        }
        if (pos.x <= -5) {
            velSign = 1;
        }
        pos.x += deltaTime * vel * float(velSign);
        
        entity()->transform->setPosition(pos);
    }
};

} // namespace

void ClusterForwardApp::loadScene() {
    _scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
    
    auto rootEntity = _scene->createRootEntity();
    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
//    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    // create box test entity
    float cubeSize = 20.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<ClusterDebugMaterial>(_device);
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxRenderer->setMesh(PrimitiveMesh::createPlane(_device, cubeSize, cubeSize));
    boxRenderer->setMaterial(boxMtl);
    
    // create sphere test entity
    float radius = 1.25;
    auto sphereEntity = rootEntity->createChild("SphereEntity");
    sphereEntity->transform->setPosition(Point3F(-5, 0, 0));
    auto sphereRenderer = sphereEntity->addComponent<MeshRenderer>();
    auto sphereMtl = std::make_shared<ClusterDebugMaterial>(_device);
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    sphereRenderer->setMesh(PrimitiveMesh::createSphere(_device, radius));
    sphereRenderer->setMaterial(sphereMtl);
    
    sphereEntity->addComponent<MoveScript>();
}

}
