//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "capsule_character_controller.h"
#include "../../entity.h"
#include "../../scene.h"

namespace vox {
namespace physics {
CapsuleCharacterController::CapsuleCharacterController(Entity *entity) :
CharacterController(entity) {
}

void CapsuleCharacterController::setDesc(const PxCapsuleControllerDesc &desc) {
    _nativeController = entity()->scene()->_physicsManager._nativeCharacterControllerManager->createController(desc);
}

float CapsuleCharacterController::radius() const {
    return static_cast<PxCapsuleController *>(_nativeController)->getRadius();
}

bool CapsuleCharacterController::setRadius(float radius) {
    return static_cast<PxCapsuleController *>(_nativeController)->setRadius(radius);
}

float CapsuleCharacterController::height() const {
    return static_cast<PxCapsuleController *>(_nativeController)->getHeight();
}

bool CapsuleCharacterController::setHeight(float height) {
    return static_cast<PxCapsuleController *>(_nativeController)->setHeight(height);
}

PxCapsuleClimbingMode::Enum CapsuleCharacterController::climbingMode() const {
    return static_cast<PxCapsuleController *>(_nativeController)->getClimbingMode();
}

bool CapsuleCharacterController::setClimbingMode(PxCapsuleClimbingMode::Enum mode) {
    return static_cast<PxCapsuleController *>(_nativeController)->setClimbingMode(mode);
}

}
}
