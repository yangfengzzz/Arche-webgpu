//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "behaviour.h"
#include "entity.h"
#include "scene.h"

namespace vox {
std::string Behaviour::name() {
    return "Behaviour";
}

Behaviour::Behaviour(Entity *entity) :
Script(entity) {
}

void Behaviour::onAwake() {
}

void Behaviour::onEnable() {
}

void Behaviour::onDisable() {
}

void Behaviour::onDestroy() {
}

void Behaviour::onStart() {
}

void Behaviour::onUpdate(float deltaTime) {
}

void Behaviour::onLateUpdate(float deltaTime) {
}

void Behaviour::onBeginRender(Camera *camera) {
}

void Behaviour::onEndRender(Camera *camera) {
}

void Behaviour::onTriggerEnter(physics::ColliderShapePtr other) {
}

void Behaviour::onTriggerExit(physics::ColliderShapePtr other) {
}

void Behaviour::onTriggerStay(physics::ColliderShapePtr other) {
}

void Behaviour::inputEvent(const InputEvent &inputEvent) {
}

void Behaviour::resize(uint32_t win_width, uint32_t win_height,
                       uint32_t fb_width, uint32_t fb_height)  {
}

}
