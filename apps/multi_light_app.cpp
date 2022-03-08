//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "multi_light_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "lighting/point_light.h"
#include "lighting/sprite/sprite_debug.h"
#include "material/blinn_phong_material.h"

#include <random>

namespace vox {
namespace {
class MoveScript : public Script {
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;
    
    Point3F pos;
    float vel;
    int8_t velSign = -1;
    
public:
    MoveScript(Entity *entity) :
    Script(entity),
    gen(rd()) {
        dis = std::uniform_real_distribution<float>(-1.0, 1.0);
        pos = Point3F(10 * dis(gen), 0, 10 * dis(gen));
        vel = std::abs(dis(gen) * 4);
    }
    
    void onUpdate(float deltaTime) override {
        if (pos.y >= 5) {
            velSign = -1;
        }
        if (pos.y <= -5) {
            velSign = 1;
        }
        pos.y += deltaTime * vel * float(velSign);
        
        entity()->transform->setPosition(pos);
        entity()->transform->lookAt(Point3F(0, 0, 0));
    }
};

}

void MultiLightApp::loadScene() {
    _scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
    
    auto rootEntity = _scene->createRootEntity();
    rootEntity->addComponent<SpriteDebug>();
    
    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    // init point light
    for (uint32_t i = 0; i < 50; i++) {
        auto light = rootEntity->createChild("light");
        light->addComponent<MoveScript>();
        auto pointLight = light->addComponent<PointLight>();
        pointLight->color = Color(u(e), u(e), u(e), 1);
    }
    
    // init spot light
    for (uint32_t i = 0; i < 50; i++) {
        auto light = rootEntity->createChild("light");
        light->addComponent<MoveScript>();
        auto spotLight = light->addComponent<SpotLight>();
        spotLight->color = Color(u(e), u(e), u(e), 1);
    }
    
    // create box test entity
    float cubeSize = 20.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<BlinnPhongMaterial>(_device);
    boxMtl->setBaseColor(Color(0.0, 0.6, 0.3, 1.0));
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxRenderer->setMesh(PrimitiveMesh::createPlane(_device, cubeSize, cubeSize, 100, 1000));
    boxRenderer->setMaterial(boxMtl);
}

}
