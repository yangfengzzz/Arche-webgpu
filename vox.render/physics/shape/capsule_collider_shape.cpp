//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "capsule_collider_shape.h"
#include "../physics_manager.h"

#ifdef _DEBUG
#include "mesh/wireframe_primitive_mesh.h"
#include "scene.h"
#include "material/unlit_material.h"
#endif

namespace vox {
namespace physics {
CapsuleColliderShape::CapsuleColliderShape() {
    _nativeGeometry = std::make_shared<PxCapsuleGeometry>(_radius * std::max(_scale.x, _scale.z), _height * _scale.y);
    _nativeShape = PhysicsManager::_nativePhysics()->createShape(*_nativeGeometry, *_nativeMaterial, true);
    _nativeShape->setQueryFilterData(PxFilterData(PhysicsManager::_idGenerator++, 0, 0, 0));
    
    setUpAxis(ColliderShapeUpAxis::Enum::Y);
}

float CapsuleColliderShape::radius() {
    return _radius;
}

void CapsuleColliderShape::setRadius(float value) {
    _radius = value;
    switch (_upAxis) {
        case ColliderShapeUpAxis::Enum::X:
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->radius = _radius * std::max(_scale.y, _scale.z);
            break;
        case ColliderShapeUpAxis::Enum::Y:
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->radius = _radius * std::max(_scale.x, _scale.z);
            break;
        case ColliderShapeUpAxis::Enum::Z:
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->radius = _radius * std::max(_scale.x, _scale.y);
            break;
    }
    _nativeShape->setGeometry(*_nativeGeometry);
    
#ifdef _DEBUG
    _syncCapsuleGeometry();
#endif
}

float CapsuleColliderShape::height() {
    return _height * 2.0;
}

void CapsuleColliderShape::setHeight(float value) {
    _height = value * 0.5;
    switch (_upAxis) {
        case ColliderShapeUpAxis::Enum::X:
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->halfHeight = _height * _scale.x;
            break;
        case ColliderShapeUpAxis::Enum::Y:
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->halfHeight = _height * _scale.y;
            break;
        case ColliderShapeUpAxis::Enum::Z:
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->halfHeight = _height * _scale.z;
            break;
    }
    _nativeShape->setGeometry(*_nativeGeometry);
    
#ifdef _DEBUG
    _syncCapsuleGeometry();
#endif
}

ColliderShapeUpAxis::Enum CapsuleColliderShape::upAxis() {
    return _upAxis;
}

void CapsuleColliderShape::setUpAxis(ColliderShapeUpAxis::Enum value) {
    _upAxis = value;
    switch (_upAxis) {
        case ColliderShapeUpAxis::Enum::X:
            _pose.setOrientation(QuaternionF(0, 0, 0, 1));
            break;
        case ColliderShapeUpAxis::Enum::Y:
            _pose.setOrientation(QuaternionF(0, 0, ColliderShape::halfSqrt, ColliderShape::halfSqrt));
            break;
        case ColliderShapeUpAxis::Enum::Z:
            _pose.setOrientation(QuaternionF(0, ColliderShape::halfSqrt, 0, ColliderShape::halfSqrt));
            break;
    }
    setLocalPose(_pose);
    
#ifdef _DEBUG
    _syncCapsuleAxis(value);
#endif
}

void CapsuleColliderShape::setWorldScale(const Vector3F &scale) {
    ColliderShape::setWorldScale(scale);
    
    switch (_upAxis) {
        case ColliderShapeUpAxis::Enum::X:
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->radius = _radius * std::max(scale.y, scale.z);
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->halfHeight = _height * scale.x;
            break;
        case ColliderShapeUpAxis::Enum::Y:
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->radius = _radius * std::max(scale.x, scale.z);
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->halfHeight = _height * scale.y;
            break;
        case ColliderShapeUpAxis::Enum::Z:
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->radius = _radius * std::max(scale.x, scale.y);
            static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->halfHeight = _height * scale.z;
            break;
    }
    _nativeShape->setGeometry(*_nativeGeometry);
    
#ifdef _DEBUG
    _syncCapsuleGeometry();
#endif
}

#ifdef _DEBUG
void CapsuleColliderShape::setEntity(EntityPtr value) {
    ColliderShape::setEntity(value);
    
    _renderer = _entity->addComponent<MeshRenderer>();
    _renderer->setMaterial(std::make_shared<UnlitMaterial>(value->scene()->device()));
    _syncCapsuleGeometry();
}

void CapsuleColliderShape::_syncCapsuleGeometry() {
    if (_entity) {
        auto radius = static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->radius;
        auto halfHeight = static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->halfHeight;
        _renderer->setMesh(WireframePrimitiveMesh::createCapsuleWireFrame(_entity->scene()->device(), radius, halfHeight * 2.0));
    }
}

void CapsuleColliderShape::_syncCapsuleAxis(ColliderShapeUpAxis::Enum upAxis) {
    if (_entity) {
        switch (upAxis) {
            case ColliderShapeUpAxis::Enum::X:
                _entity->transform->setRotationQuaternion(0, ColliderShape::halfSqrt,
                                                          0, ColliderShape::halfSqrt);
                break;
            case ColliderShapeUpAxis::Enum::Y:
                _entity->transform->setRotationQuaternion(0, 0, 0, 1);
                break;
            case ColliderShapeUpAxis::Enum::Z:
                _entity->transform->setRotationQuaternion(0, 0, ColliderShape::halfSqrt,
                                                          ColliderShape::halfSqrt);
                break;
        }
    }
}
#endif

}
}
