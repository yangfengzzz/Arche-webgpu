//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "plane_collider_shape.h"
#include "../physics_manager.h"

namespace vox {
namespace physics {
PlaneColliderShape::PlaneColliderShape() {
    _nativeGeometry = std::make_shared<PxPlaneGeometry>();
    _nativeShape = PhysicsManager::_nativePhysics()->createShape(*_nativeGeometry, *_nativeMaterial, true);
    _nativeShape->setQueryFilterData(PxFilterData(PhysicsManager::_idGenerator++, 0, 0, 0));
    
    _pose.setOrientation(QuaternionF(0, 0, ColliderShape::halfSqrt, ColliderShape::halfSqrt));
    setLocalPose(_pose);
}

Vector3F PlaneColliderShape::rotation() {
    return _pose.orientation().toEuler();
}

void PlaneColliderShape::setRotation(const Vector3F &value) {
    auto rotation = QuaternionF::makeRotationEuler(value.x, value.y, value.z);
    rotation.rotateZ(M_PI * 0.5);
    rotation.normalize();
    _pose.setOrientation(rotation);
    setLocalPose(_pose);
}

}
}
