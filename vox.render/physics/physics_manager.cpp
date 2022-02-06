//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "physics_manager.h"
#include "shape/collider_shape.h"
#include "character_controller/character_controller.h"
#include "collider.h"
#include "../entity.h"
#include "../script.h"

namespace vox {
namespace physics {
namespace {
class PxSimulationEventCallbackWrapper : public PxSimulationEventCallback {
public:
    std::function<void(PxShape *obj1, PxShape *obj2)> onContactEnter;
    std::function<void(PxShape *obj1, PxShape *obj2)> onContactExit;
    std::function<void(PxShape *obj1, PxShape *obj2)> onContactStay;
    
    std::function<void(PxShape *obj1, PxShape *obj2)> onTriggerEnter;
    std::function<void(PxShape *obj1, PxShape *obj2)> onTriggerExit;
    std::function<void(PxShape *obj1, PxShape *obj2)> onTriggerStay;
    
    PxSimulationEventCallbackWrapper(std::function<void(PxShape *obj1, PxShape *obj2)> onContactEnter,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> onContactExit,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> onContactStay,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> onTriggerEnter,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> onTriggerExit,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> onTriggerStay) :
    onContactEnter(onContactEnter), onContactExit(onContactExit), onContactStay(onContactStay),
    onTriggerEnter(onTriggerEnter), onTriggerExit(onTriggerExit), onTriggerStay(onTriggerStay) {
    }
    
    void onConstraintBreak(PxConstraintInfo *, PxU32) override {
    }
    
    void onWake(PxActor **, PxU32) override {
    }
    
    void onSleep(PxActor **, PxU32) override {
    }
    
    void onContact(const PxContactPairHeader &, const PxContactPair *pairs, PxU32 nbPairs) override {
        for (PxU32 i = 0; i < nbPairs; i++) {
            const PxContactPair &cp = pairs[i];
            
            if (cp.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD)) {
                onContactEnter(cp.shapes[0], cp.shapes[1]);
            } else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
                onContactExit(cp.shapes[0], cp.shapes[1]);
            } else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) {
                onContactStay(cp.shapes[0], cp.shapes[1]);
            }
        }
    }
    
    void onTrigger(PxTriggerPair *pairs, PxU32 count) override {
        for (PxU32 i = 0; i < count; i++) {
            const PxTriggerPair &tp = pairs[i];
            
            if (tp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                onTriggerEnter(tp.triggerShape, tp.otherShape);
            } else if (tp.status & PxPairFlag::eNOTIFY_TOUCH_LOST) {
                onTriggerExit(tp.triggerShape, tp.otherShape);
            }
        }
    }
    
    void onAdvance(const PxRigidBody *const *, const PxTransform *, const PxU32) override {
    }
};
} // namespace

uint32_t PhysicsManager::_idGenerator = 0;
Physics PhysicsManager::_nativePhysics = Physics();

PhysicsManager::PhysicsManager() {
    onContactEnter = [&](PxShape *obj1, PxShape *obj2) {
    };
    onContactExit = [&](PxShape *obj1, PxShape *obj2) {
    };
    onContactStay = [&](PxShape *obj1, PxShape *obj2) {
    };
    
    onTriggerEnter = [&](PxShape *obj1, PxShape *obj2) {
        const auto shape1 = _physicalObjectsMap[obj1->getQueryFilterData().word0];
        const auto shape2 = _physicalObjectsMap[obj2->getQueryFilterData().word0];
        
        auto scripts = shape1->collider()->entity()->scripts();
        for (const auto &script: scripts) {
            script->onTriggerEnter(shape2);
        }
        
        scripts = shape2->collider()->entity()->scripts();
        for (const auto &script: scripts) {
            script->onTriggerEnter(shape1);
        }
    };
    onTriggerExit = [&](PxShape *obj1, PxShape *obj2) {
        const auto shape1 = _physicalObjectsMap[obj1->getQueryFilterData().word0];
        const auto shape2 = _physicalObjectsMap[obj2->getQueryFilterData().word0];
        
        auto scripts = shape1->collider()->entity()->scripts();
        for (const auto &script: scripts) {
            script->onTriggerExit(shape2);
        }
        
        scripts = shape2->collider()->entity()->scripts();
        for (const auto &script: scripts) {
            script->onTriggerExit(shape1);
        }
    };
    onTriggerStay = [&](PxShape *obj1, PxShape *obj2) {
    };
    
    PxSimulationEventCallbackWrapper *simulationEventCallback =
    new PxSimulationEventCallbackWrapper(onContactEnter, onContactExit, onContactStay,
                                         onTriggerEnter, onTriggerExit, onTriggerStay);
    
    PxSceneDesc sceneDesc(_nativePhysics()->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.simulationEventCallback = simulationEventCallback;
    sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
    sceneDesc.staticKineFilteringMode = PxPairFilteringMode::eKEEP;
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    
    _nativePhysicsManager = _nativePhysics()->createScene(sceneDesc);
    _nativeCharacterControllerManager = PxCreateControllerManager(*_nativePhysicsManager);
}

void PhysicsManager::update(float deltaTime) {
    _nativePhysicsManager->simulate(deltaTime);
    _nativePhysicsManager->fetchResults(true);
}

void PhysicsManager::callColliderOnUpdate() {
    for (auto &collider: _colliders) {
        collider->_onUpdate();
    }
}

void PhysicsManager::callColliderOnLateUpdate() {
    for (auto &collider: _colliders) {
        collider->_onLateUpdate();
    }
}

void PhysicsManager::callCharacterControllerOnLateUpdate() {
    for (auto &controller: _controllers) {
        controller->_onLateUpdate();
    }
}

void PhysicsManager::_addColliderShape(const ColliderShapePtr &colliderShape) {
    _physicalObjectsMap[colliderShape->uniqueID()] = (colliderShape);
}

void PhysicsManager::_removeColliderShape(const ColliderShapePtr &colliderShape) {
    _physicalObjectsMap.erase(colliderShape->uniqueID());
}

void PhysicsManager::_addCollider(Collider *collider) {
    _colliders.push_back(collider);
    _nativePhysicsManager->addActor(*collider->_nativeActor);
}

void PhysicsManager::_removeCollider(Collider *collider) {
    auto iter = std::find(_colliders.begin(), _colliders.end(), collider);
    if (iter != _colliders.end()) {
        _colliders.erase(iter);
    }
    
    _nativePhysicsManager->removeActor(*collider->_nativeActor);
}

void PhysicsManager::_addCharacterController(CharacterController *characterController) {
    _controllers.push_back(characterController);
}

void PhysicsManager::_removeCharacterController(CharacterController *characterController) {
    auto iter = std::find(_controllers.begin(), _controllers.end(), characterController);
    if (iter != _controllers.end()) {
        _controllers.erase(iter);
    }
}

//MARK: - Raycast
bool PhysicsManager::raycast(const Ray3F &ray) {
    return _raycast(ray, std::numeric_limits<float>::infinity(), nullptr);
}

bool PhysicsManager::raycast(const Ray3F &ray, HitResult &outHitResult) {
    const auto layerMask = Layer::Everything;
    
    bool result = false;
    _raycast(ray, std::numeric_limits<float>::infinity(),
             [&](uint32_t idx, float distance, const Vector3F &normal, const Point3F &point) {
        if (_physicalObjectsMap[idx]->collider()->entity()->layer & layerMask) {
            result = true;
            
            outHitResult.entity = _physicalObjectsMap[idx]->collider()->entity();
            outHitResult.distance = distance;
            outHitResult.normal = normal;
            outHitResult.point = point;
        }
    });
    
    if (!result) {
        outHitResult.entity = nullptr;
        outHitResult.distance = 0;
        outHitResult.point = Point3F(0, 0, 0);
        outHitResult.normal = Vector3F(0, 0, 0);
    }
    
    return result;
}

bool PhysicsManager::raycast(const Ray3F &ray, float distance) {
    return _raycast(ray, distance, nullptr);
}

bool PhysicsManager::raycast(const Ray3F &ray, float distance, HitResult &outHitResult) {
    const auto layerMask = Layer::Everything;
    
    bool result = false;
    _raycast(ray, distance, [&](uint32_t idx, float distance, const Vector3F &normal, const Point3F &point) {
        if (_physicalObjectsMap[idx]->collider()->entity()->layer & layerMask) {
            result = true;
            
            outHitResult.entity = _physicalObjectsMap[idx]->collider()->entity();
            outHitResult.distance = distance;
            outHitResult.normal = normal;
            outHitResult.point = point;
        }
    });
    
    if (!result) {
        outHitResult.entity = nullptr;
        outHitResult.distance = 0;
        outHitResult.point = Point3F(0, 0, 0);
        outHitResult.normal = Vector3F(0, 0, 0);
    }
    
    return result;
}

bool PhysicsManager::raycast(const Ray3F &ray, float distance, Layer layerMask) {
    bool result = false;
    _raycast(ray, distance, [&](uint32_t idx, float, const Vector3F &, const Point3F &) {
        if (_physicalObjectsMap[idx]->collider()->entity()->layer & layerMask) {
            result = true;
        }
    });
    return result;
}

bool PhysicsManager::raycast(const Ray3F &ray, float distance, Layer layerMask, HitResult &outHitResult) {
    bool result = false;
    _raycast(ray, distance, [&](uint32_t idx, float distance, const Vector3F &normal, const Point3F &point) {
        if (_physicalObjectsMap[idx]->collider()->entity()->layer & layerMask) {
            result = true;
            
            outHitResult.entity = _physicalObjectsMap[idx]->collider()->entity();
            outHitResult.distance = distance;
            outHitResult.normal = normal;
            outHitResult.point = point;
        }
    });
    
    if (!result) {
        outHitResult.entity = nullptr;
        outHitResult.distance = 0;
        outHitResult.point = Point3F(0, 0, 0);
        outHitResult.normal = Vector3F(0, 0, 0);
    }
    
    return result;
}

bool PhysicsManager::_raycast(const Ray3F &ray, float distance,
                              std::function<void(uint32_t, float,
                                                 const Vector3F &,
                                                 const Point3F &)> outHitResult) {
    PxRaycastHit hit = PxRaycastHit();
    PxSceneQueryFilterData filterData = PxSceneQueryFilterData();
    filterData.flags = PxQueryFlags(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);
    
    const auto &origin = ray.origin;
    const auto &direction = ray.direction;
    bool result = PxSceneQueryExt::raycastSingle(*_nativePhysicsManager,
                                                 PxVec3(origin.x, origin.y, origin.z),
                                                 PxVec3(direction.x, direction.y, direction.z),
                                                 distance, PxHitFlags(PxHitFlag::eDEFAULT),
                                                 hit, filterData);
    
    if (result && outHitResult != nullptr) {
        outHitResult(hit.shape->getQueryFilterData().word0,
                     hit.distance,
                     Vector3F(hit.normal.x, hit.normal.y, hit.normal.z),
                     Point3F(hit.position.x, hit.position.y, hit.position.z));
    }
    
    return result;
}

}
}
