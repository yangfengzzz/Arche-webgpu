//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "collider_shape.h"
#include "../physics_manager.h"

namespace vox {
namespace physics {
ColliderShape::ColliderShape() :
_nativeMaterial(PhysicsManager::_nativePhysics()->createMaterial(0, 0, 0)) {
    _pose.setOrientation(QuaternionF(0, 0, halfSqrt, halfSqrt));
}

Collider *ColliderShape::collider() {
    return _collider;
}

void ColliderShape::setLocalPose(const Transform3F &pose) {
    _pose = pose;
    
    const auto &p = pose.translation();
    const auto &q = pose.orientation();
    _nativeShape->setLocalPose(PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
}

Transform3F ColliderShape::localPose() const {
    return _pose;
}

void ColliderShape::setPosition(const Vector3F &pos) {
    _pose.setTranslation(pos);
    setLocalPose(_pose);
}

Vector3F ColliderShape::position() const {
    return _pose.translation();
}

void ColliderShape::setMaterial(PxMaterial *material) {
    _nativeMaterial = material;
    
    std::vector<PxMaterial *> materials = {material};
    _nativeShape->setMaterials(materials.data(), 1);
}

PxMaterial *ColliderShape::material() {
    return _nativeMaterial;
}

//MARK: - QueryFilterData
PxFilterData ColliderShape::queryFilterData() {
    return _nativeShape->getQueryFilterData();
}

void ColliderShape::setQueryFilterData(const PxFilterData &data) {
    _nativeShape->setQueryFilterData(data);
}

uint32_t ColliderShape::uniqueID() {
    return _nativeShape->getQueryFilterData().word0;
}

//MARK: - ShapeFlag
void ColliderShape::setFlag(PxShapeFlag::Enum flag, bool value) {
    _nativeShape->setFlag(flag, value);
}

void ColliderShape::setFlags(PxShapeFlags inFlags) {
    _nativeShape->setFlags(inFlags);
}

PxShapeFlags ColliderShape::getFlags() const {
    return _nativeShape->getFlags();
}

bool ColliderShape::trigger() {
    return _nativeShape->getFlags().isSet(PxShapeFlag::Enum::eTRIGGER_SHAPE);
}

void ColliderShape::setTrigger(bool isTrigger) {
    _nativeShape->setFlag(PxShapeFlag::Enum::eSIMULATION_SHAPE, !isTrigger);
    _nativeShape->setFlag(PxShapeFlag::Enum::eTRIGGER_SHAPE, isTrigger);
}

bool ColliderShape::sceneQuery() {
    return _nativeShape->getFlags().isSet(PxShapeFlag::Enum::eSCENE_QUERY_SHAPE);
}

void ColliderShape::setSceneQuery(bool isQuery) {
    _nativeShape->setFlag(PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, isQuery);
}

}
}
