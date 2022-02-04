//
//  sphere_collider_shape.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#include "sphere_collider_shape.h"
#include "../physics_manager.h"

namespace vox {
namespace physics {
SphereColliderShape::SphereColliderShape() {
    _nativeGeometry = std::make_shared<PxSphereGeometry>(_radius * std::max(std::max(_scale.x, _scale.y), _scale.z));
    _nativeShape = PhysicsManager::_nativePhysics()->createShape(*_nativeGeometry, *_nativeMaterial, true);
    _nativeShape->setQueryFilterData(PxFilterData(PhysicsManager::_idGenerator++, 0, 0, 0));
    setLocalPose(_pose);
}

float SphereColliderShape::radius() {
    return _radius;
}

void SphereColliderShape::setRadius(float value) {
    _radius = value;
    static_cast<PxSphereGeometry *>(_nativeGeometry.get())->radius = value * std::max(std::max(_scale.x, _scale.y), _scale.z);
    _nativeShape->setGeometry(*_nativeGeometry);
}

void SphereColliderShape::setWorldScale(const Vector3F &scale) {
    _scale = scale;
    static_cast<PxSphereGeometry *>(_nativeGeometry.get())->radius = _radius * std::max(std::max(_scale.x, _scale.y), _scale.z);
    _nativeShape->setGeometry(*_nativeGeometry);
}

}
}
