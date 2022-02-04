//
//  box_collider_shape.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/2.
//

#include "box_collider_shape.h"
#include "../physics_manager.h"

namespace vox {
namespace physics {
BoxColliderShape::BoxColliderShape() : ColliderShape() {
    auto halfExtent = _half * _scale;
    _nativeGeometry = std::make_shared<PxBoxGeometry>(halfExtent.x, halfExtent.y, halfExtent.z);
    _nativeShape = PhysicsManager::_nativePhysics()->createShape(*_nativeGeometry, *_nativeMaterial, true);
    _nativeShape->setQueryFilterData(PxFilterData(PhysicsManager::_idGenerator++, 0, 0, 0));
    setLocalPose(_pose);
}

Vector3F BoxColliderShape::size() {
    return _half;
}

void BoxColliderShape::setSize(const Vector3F &size) {
    _half = size * 0.5f;
    auto halfExtent = _half * _scale;
    static_cast<PxBoxGeometry *>(_nativeGeometry.get())->halfExtents = PxVec3(halfExtent.x, halfExtent.y, halfExtent.z);
    _nativeShape->setGeometry(*_nativeGeometry);
}

void BoxColliderShape::setWorldScale(const Vector3F &scale) {
    _scale = scale;
    auto halfExtent = _half * _scale;
    static_cast<PxBoxGeometry *>(_nativeGeometry.get())->halfExtents = PxVec3(halfExtent.x, halfExtent.y, halfExtent.z);
    _nativeShape->setGeometry(*_nativeGeometry);
}

}
}
