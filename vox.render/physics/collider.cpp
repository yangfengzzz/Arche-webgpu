//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "collider.h"
#include "shape/collider_shape.h"
#include "../entity.h"
#include "../scene.h"

namespace vox {
namespace physics {
Collider::Collider(Entity *entity) :
Component(entity) {
    _updateFlag = entity->transform->registerWorldChangeFlag();
}

PxRigidActor *Collider::handle() {
    return _nativeActor;
}

void Collider::addShape(const ColliderShapePtr &shape) {
    const auto &oldCollider = shape->_collider;
    if (oldCollider != this) {
        if (oldCollider != nullptr) {
            oldCollider->removeShape(shape);
        }
        _shapes.push_back(shape);
        entity()->scene()->_physicsManager._addColliderShape(shape);
        _nativeActor->attachShape(*shape->_nativeShape);
        shape->_collider = this;
    }
    
#ifdef _DEBUG
    if (debugEntity) {
        shape->setEntity(debugEntity);
    }
#endif
}

void Collider::removeShape(const ColliderShapePtr &shape) {
    auto iter = std::find(_shapes.begin(), _shapes.end(), shape);
    
    if (iter != _shapes.end()) {
        _shapes.erase(iter);
        _nativeActor->detachShape(*shape->_nativeShape);
        entity()->scene()->_physicsManager._removeColliderShape(shape);
        shape->_collider = nullptr;
    }
    
#ifdef _DEBUG
    if (debugEntity) {
        shape->removeEntity(debugEntity);
    }
#endif
}

void Collider::clearShapes() {
    for (size_t i = 0; i < _shapes.size(); i++) {
        _nativeActor->detachShape(*_shapes[i]->_nativeShape);
        entity()->scene()->_physicsManager._removeColliderShape(_shapes[i]);
    }
    _shapes.clear();
}

void Collider::_onUpdate() {
    if (_updateFlag->flag) {
        const auto &transform = entity()->transform;
        const auto &p = transform->worldPosition();
        auto q = transform->worldRotationQuaternion();
        q.normalize();
        _nativeActor->setGlobalPose(PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
        _updateFlag->flag = false;
        
        const auto worldScale = transform->lossyWorldScale();
        for (auto &_shape: _shapes) {
            _shape->setWorldScale(worldScale);
        }
        
#ifdef _DEBUG
        if (debugEntity) {
            auto transform = _nativeActor->getGlobalPose();
            debugEntity->transform->setPosition(Point3F(transform.p.x, transform.p.y, transform.p.z));
            debugEntity->transform->setRotationQuaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
        }
#endif
    }
}

void Collider::_onEnable() {
    entity()->scene()->_physicsManager._addCollider(this);
}

void Collider::_onDisable() {
    entity()->scene()->_physicsManager._removeCollider(this);
}

void Collider::_onDestroy() {
    clearShapes();
}

}
}
