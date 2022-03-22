//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "physx_dynamic_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "physics/static_collider.h"
#include "physics/dynamic_collider.h"
#include "physics/character_controller/capsule_character_controller.h"
#include "physics/joint/fixed_joint.h"
#include "physics/shape/box_collider_shape.h"
#include "physics/shape/sphere_collider_shape.h"
#include "physics/shape/plane_collider_shape.h"
#include "physics/shape/capsule_collider_shape.h"
#include "lighting/point_light.h"
#include "lighting/direct_light.h"
#include "controls/orbit_control.h"

namespace vox {
namespace {
class ControllerScript : public Script {
private:
    EntityPtr _camera{nullptr};
    physics::CharacterController* _character = nullptr;
    Vector3F _displacement = Vector3F();
    
public:
    ControllerScript(Entity* entity):Script(entity) {
        _character = entity->getComponent<physics::CapsuleCharacterController>();
    }
    
    void targetCamera(EntityPtr camera) {
        _camera = camera;
    }
    
    void inputEvent(const InputEvent &inputEvent) override {
        if (inputEvent.source() == EventSource::Keyboard) {
            const auto &key_event = static_cast<const KeyInputEvent &>(inputEvent);
            
            Vector3F forward = entity()->transform->position() - _camera->transform->position();
            forward.y = 0;
            forward.normalize();
            Vector3F cross = Vector3F(forward.z, 0, -forward.x);
            
            switch (key_event.code()) {
                case KeyCode::W:
                    _displacement = forward * 0.3f;
                    break;
                case KeyCode::S:
                    _displacement = -forward * 0.3f;
                    break;
                case KeyCode::A:
                    _displacement = cross * 0.3f;
                    break;
                case KeyCode::D:
                    _displacement = -cross * 0.3f;
                    break;
                case KeyCode::Space:
                    _displacement.x = 0;
                    _displacement.y = 2;
                    _displacement.z = 0;
                    break;
                default:
                    break;
            }
        }
    }
    
    void onUpdate(float deltaTime) override {
        auto flags = _character->move(_displacement, 0.1, deltaTime);
        _displacement = Vector3F();
        if (!flags.isSet(physx::PxControllerCollisionFlag::Enum::eCOLLISION_DOWN)) {
            _character->move(Vector3F(0, -0.2, 0), 0.1, deltaTime);
        }
    }
};

}

void PhysXDynamicApp::loadScene() {
    u = std::uniform_real_distribution<float>(0, 1);
    auto scene = _sceneManager->currentScene();
    scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
    
    _rootEntity = scene->createRootEntity();
    auto cameraEntity = _rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(20, 20, 20);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
    auto addPlane = [&](const Vector3F &size, const Point3F &position, const QuaternionF &rotation) {
        auto mtl = std::make_shared<BlinnPhongMaterial>(_device);
        mtl->setBaseColor(Color(0.03179807202597362, 0.3939682161541871, 0.41177952549087604, 1.0));
        auto planeEntity = _rootEntity->createChild();
        planeEntity->layer = Layer::Layer1;
        
        auto renderer = planeEntity->addComponent<MeshRenderer>();
        renderer->receiveShadow = true;
        renderer->setMesh(PrimitiveMesh::createCuboid(_device, size.x, size.y, size.z));
        renderer->setMaterial(mtl);
        planeEntity->transform->setPosition(position);
        planeEntity->transform->setRotationQuaternion(rotation);
        
        auto physicsPlane = std::make_shared<physics::PlaneColliderShape>();
        auto planeCollider = planeEntity->addComponent<physics::StaticCollider>();
        planeCollider->addShape(physicsPlane);
        
        return planeEntity;
    };
    
    auto addBox = [&](const Vector3F &size, const Point3F &position, const QuaternionF &rotation) {
        auto boxMtl = std::make_shared<BlinnPhongMaterial>(_device);
        boxMtl->setBaseColor(Color(u(e), u(e), u(e), 1.0));
        auto boxEntity = _rootEntity->createChild("BoxEntity");
        auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
        boxRenderer->castShadow = true;
        boxRenderer->setMesh(PrimitiveMesh::createCuboid(_device, size.x, size.y, size.z));
        boxRenderer->setMaterial(boxMtl);
        boxEntity->transform->setPosition(position);
        boxEntity->transform->setRotationQuaternion(rotation);
        
        auto physicsBox = std::make_shared<physics::BoxColliderShape>();
        physicsBox->setSize(Vector3F(size.x, size.y, size.z));
        physicsBox->material()->setStaticFriction(1);
        physicsBox->material()->setDynamicFriction(2);
        physicsBox->material()->setRestitution(0.1);
        physicsBox->setTrigger(false);
        
        auto boxCollider = boxEntity->addComponent<physics::DynamicCollider>();
        boxCollider->addShape(physicsBox);
        
        return boxEntity;
    };
    
    auto addPlayer = [&](float radius, float height, const Point3F &position, const QuaternionF &rotation) {
        auto mtl = std::make_shared<BlinnPhongMaterial>(_device);
        mtl->setBaseColor(Color(u(e), u(e), u(e), 1.0));
        auto capsuleEntity = _rootEntity->createChild();
        auto renderer = capsuleEntity->addComponent<MeshRenderer>();
        renderer->castShadow = true;
        renderer->setMesh(PrimitiveMesh::createCapsule(_device, radius, height, 20));
        renderer->setMaterial(mtl);
        capsuleEntity->transform->setPosition(position);
        capsuleEntity->transform->setRotationQuaternion(rotation);
        
        auto characterController = capsuleEntity->addComponent<physics::CapsuleCharacterController>();
        physx::PxCapsuleControllerDesc characterControllerDesc;
        characterControllerDesc.radius = radius;
        characterControllerDesc.height = height;
        characterControllerDesc.material = physics::PhysicsManager::_nativePhysics()->createMaterial(0,0,0);
        auto worldPos = capsuleEntity->transform->worldPosition();
        characterControllerDesc.position = physx::PxExtendedVec3(worldPos.x, worldPos.y, worldPos.z);
        characterController->setDesc(characterControllerDesc);
        
        return capsuleEntity;
    };
    
    auto transform = [&](const Point3F &position, const QuaternionF &rotation,
                         Point3F &outPosition, QuaternionF &outRotation) {
        outRotation = rotation * outRotation;
        outPosition = rotation * outPosition;
        outPosition = outPosition + Vector3F(position.x, position.y, position.z);
    };
    
    auto createChain = [&](const Point3F &position, const QuaternionF &rotation, size_t length, float separation) {
        auto offset = Vector3F(0, -separation / 2, 0);
        physics::DynamicCollider *prevCollider = nullptr;
        for (size_t i = 0; i < length; i++) {
            auto localTm_pos = Point3F(0, -separation / 2 * (2 * float(i) + 1), 0);
            auto localTm_quat = QuaternionF();
            transform(position, rotation, localTm_pos, localTm_quat);
            
            auto currentEntity = addBox(Vector3F(2.0, 2.0, 0.5), localTm_pos, localTm_quat);
            auto currentCollider = currentEntity->getComponent<physics::DynamicCollider>();
            
            auto joint = physics::FixedJoint(prevCollider, currentCollider);
            Transform3F localPose;
            localPose.setTranslation(prevCollider != nullptr ? offset : Vector3F(position.x, position.y, position.z));
            localPose.setOrientation(prevCollider != nullptr ? QuaternionF() : rotation);
            joint.setLocalPose(physx::PxJointActorIndex::Enum::eACTOR0, localPose);
            localPose.setTranslation(Vector3F(0, separation / 2, 0));
            localPose.setOrientation(QuaternionF());
            joint.setLocalPose(physx::PxJointActorIndex::Enum::eACTOR1, localPose);
            prevCollider = currentCollider;
        }
    };
    
    auto light = _rootEntity->createChild("light");
    light->transform->setPosition(10, 10, 0);
    light->transform->lookAt(Point3F());
    auto directLight = light->addComponent<DirectLight>();
    directLight->intensity = 0.5;
    directLight->setEnableShadow(true);
    
    auto player = addPlayer(1, 3, Point3F(0, 6.5, 0), QuaternionF());
    auto controller = player->addComponent<ControllerScript>();
    controller->targetCamera(cameraEntity);
    
    addPlane(Vector3F(30, 0.1, 30), Point3F(), QuaternionF());
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            addBox(Vector3F(1, 1, 1),
                   Point3F(-2.5 + i + 0.1 * i, u(e) * 6.f + 1, -2.5 + j + 0.1 * j),
                   QuaternionF(0, 0, 0.3, 0.7));
        }
    }
    createChain(Point3F(0.0, 25.0, -10.0), QuaternionF(), 10, 2.0);
}

void PhysXDynamicApp::inputEvent(const InputEvent &inputEvent) {
    ForwardApplication::inputEvent(inputEvent);
    
    if (inputEvent.source() == EventSource::Mouse) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(inputEvent);
        if (mouse_button.action() == MouseAction::Down) {
            Ray ray = _mainCamera->screenPointToRay(Vector2F(mouse_button.pos_x(), mouse_button.pos_y()));
            
            physics::HitResult hit;
            auto result = _sceneManager->currentScene()->_physicsManager.raycast(ray, std::numeric_limits<float>::max(), Layer::Layer0, hit);
            if (result) {
                auto mtl = std::make_shared<BlinnPhongMaterial>(_device);
                mtl->setBaseColor(Color(u(e), u(e), u(e), 1));
                
                auto meshes = hit.entity->getComponentsIncludeChildren<MeshRenderer>();
                for (auto& mesh : meshes) {
                    mesh->setMaterial(mtl);
                }
            }
        }
    } else if (inputEvent.source() == EventSource::Keyboard) {
        const auto &key_event = static_cast<const KeyInputEvent &>(inputEvent);
        if (key_event.action() == KeyAction::Up) {
            Vector3F dir = _mainCamera->entity()->transform->worldForward();
            dir = dir * 50.0f;
            
            switch (key_event.code()) {
                case KeyCode::Enter:
                    addSphere(0.5, _mainCamera->entity()->transform->position(),
                              _mainCamera->entity()->transform->rotationQuaternion(), dir);
                    break;
                default:
                    break;
            }
        }
    }
}

EntityPtr PhysXDynamicApp::addSphere(float radius, const Point3F &position,
                                     const QuaternionF &rotation, const Vector3F &velocity) {
    auto mtl = std::make_shared<BlinnPhongMaterial>(_device);
    mtl->setBaseColor(Color(u(e), u(e), u(e), 1.0));
    auto sphereEntity = _rootEntity->createChild();
    auto renderer = sphereEntity->addComponent<MeshRenderer>();
    renderer->castShadow = true;
    renderer->setMesh(PrimitiveMesh::createSphere(_device, radius));
    renderer->setMaterial(mtl);
    sphereEntity->transform->setPosition(position);
    sphereEntity->transform->setRotationQuaternion(rotation);
    
    auto physicsSphere = std::make_shared<physics::SphereColliderShape>();
    physicsSphere->setRadius(radius);
    physicsSphere->material()->setStaticFriction(0.1);
    physicsSphere->material()->setDynamicFriction(0.2);
    physicsSphere->material()->setRestitution(1);
    physicsSphere->material()->setRestitutionCombineMode(physx::PxCombineMode::Enum::eMIN);
    
    auto sphereCollider = sphereEntity->addComponent<physics::DynamicCollider>();
    sphereCollider->addShape(physicsSphere);
    sphereCollider->setLinearVelocity(velocity);
    sphereCollider->setAngularDamping(0.5);
    
    return sphereEntity;
};

EntityPtr PhysXDynamicApp::addCapsule(float radius, float height,
                                      const Point3F &position, const QuaternionF &rotation) {
    auto mtl = std::make_shared<BlinnPhongMaterial>(_device);
    mtl->setBaseColor(Color(u(e), u(e), u(e), 1.0));
    auto capsuleEntity = _rootEntity->createChild();
    auto renderer = capsuleEntity->addComponent<MeshRenderer>();
    renderer->castShadow = true;
    renderer->setMesh(PrimitiveMesh::createCapsule(_device, radius, height));
    renderer->setMaterial(mtl);
    capsuleEntity->transform->setPosition(position);
    capsuleEntity->transform->setRotationQuaternion(rotation);
    
    auto physicsCapsule = std::make_shared<physics::CapsuleColliderShape>();
    physicsCapsule->setRadius(radius);
    physicsCapsule->setHeight(height);
    
    auto capsuleCollider = capsuleEntity->addComponent<physics::DynamicCollider>();
    capsuleCollider->addShape(physicsCapsule);
    
    return capsuleEntity;
};

}
