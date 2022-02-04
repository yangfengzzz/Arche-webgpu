//
//  box_character_controller.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#include "box_character_controller.h"
#include "../../entity.h"
#include "../../scene.h"

namespace vox {
namespace physics {
BoxCharacterController::BoxCharacterController(Entity *entity) :
CharacterController(entity) {
}

void BoxCharacterController::setDesc(const PxBoxControllerDesc &desc) {
    _nativeController = entity()->scene()->_physicsManager._nativeCharacterControllerManager->createController(desc);
}

float BoxCharacterController::halfHeight() const {
    return static_cast<PxBoxController *>(_nativeController)->getHalfHeight();
}

float BoxCharacterController::halfSideExtent() const {
    return static_cast<PxBoxController *>(_nativeController)->getHalfSideExtent();
}

float BoxCharacterController::halfForwardExtent() const {
    return static_cast<PxBoxController *>(_nativeController)->getHalfForwardExtent();
}

bool BoxCharacterController::setHalfHeight(float halfHeight) {
    return static_cast<PxBoxController *>(_nativeController)->setHalfHeight(halfHeight);
}

bool BoxCharacterController::setHalfSideExtent(float halfSideExtent) {
    return static_cast<PxBoxController *>(_nativeController)->setHalfSideExtent(halfSideExtent);
}

bool BoxCharacterController::setHalfForwardExtent(float halfForwardExtent) {
    return static_cast<PxBoxController *>(_nativeController)->setHalfForwardExtent(halfForwardExtent);
}

}
}
