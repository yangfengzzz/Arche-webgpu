//
//  plane_collider_shape.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#include "plane_collider_shape.h"
#include "../physics_manager.h"

namespace vox {
namespace physics {
PlaneColliderShape::PlaneColliderShape() {
    _nativeGeometry = std::make_shared<PxPlaneGeometry>();
    _nativeShape = PhysicsManager::_nativePhysics()->createShape(*_nativeGeometry, *_nativeMaterial, true);
    _nativeShape->setQueryFilterData(PxFilterData(PhysicsManager::_idGenerator++, 0, 0, 0));
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
