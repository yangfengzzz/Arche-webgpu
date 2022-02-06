//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "character_controller.h"
#include "../../entity.h"
#include "../../scene.h"

namespace vox {
namespace physics {
CharacterController::CharacterController(Entity *entity) :
Component(entity) {
}

PxControllerCollisionFlags CharacterController::move(const Vector3F &disp, float minDist, float elapsedTime) {
    return _nativeController->move(PxVec3(disp.x, disp.y, disp.z), minDist, elapsedTime, PxControllerFilters());
}

bool CharacterController::setPosition(const Point3F &position) {
    return _nativeController->setPosition(PxExtendedVec3(position.x, position.y, position.z));
}

Point3F CharacterController::position() const {
    auto pose = _nativeController->getPosition();
    return Point3F(pose.x, pose.y, pose.z);
}

bool CharacterController::setFootPosition(const Vector3F &position) {
    return _nativeController->setFootPosition(PxExtendedVec3(position.x, position.y, position.z));
}

Vector3F CharacterController::footPosition() const {
    auto pose = _nativeController->getFootPosition();
    return Vector3F(pose.x, pose.y, pose.z);
}

void CharacterController::setStepOffset(const float offset) {
    _nativeController->setStepOffset(offset);
}

float CharacterController::stepOffset() const {
    return _nativeController->getStepOffset();
}

void CharacterController::setNonWalkableMode(PxControllerNonWalkableMode::Enum flag) {
    _nativeController->setNonWalkableMode(flag);
}

PxControllerNonWalkableMode::Enum CharacterController::nonWalkableMode() const {
    return _nativeController->getNonWalkableMode();
}

float CharacterController::contactOffset() const {
    return _nativeController->getContactOffset();
}

void CharacterController::setContactOffset(float offset) {
    _nativeController->setContactOffset(offset);
}

Vector3F CharacterController::upDirection() const {
    auto dir = _nativeController->getUpDirection();
    return Vector3F(dir.x, dir.y, dir.z);
}

void CharacterController::setUpDirection(const Vector3F &up) {
    _nativeController->setUpDirection(PxVec3(up.x, up.y, up.z));
}

float CharacterController::slopeLimit() const {
    return _nativeController->getSlopeLimit();
}

void CharacterController::setSlopeLimit(float slopeLimit) {
    _nativeController->setSlopeLimit(slopeLimit);
}

void CharacterController::invalidateCache() {
    _nativeController->invalidateCache();
}

void CharacterController::state(PxControllerState &state) const {
    _nativeController->getState(state);
}

void CharacterController::stats(PxControllerStats &stats) const {
    _nativeController->getStats(stats);
}

void CharacterController::resize(float height) {
    _nativeController->resize(height);
}

void CharacterController::_onLateUpdate() {
    entity()->transform->setWorldPosition(position());
}

void CharacterController::_onEnable() {
    entity()->scene()->_physicsManager._addCharacterController(this);
}

void CharacterController::_onDisable() {
    entity()->scene()->_physicsManager._removeCharacterController(this);
}

}
}
