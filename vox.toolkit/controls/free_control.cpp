//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/controls/free_control.h"

#include "vox.render/entity.h"

namespace vox::control {
std::string FreeControl::name() { return "FreeControl"; }

FreeControl::FreeControl(Entity *entity) : Script(entity) {
    // init spherical
    updateSpherical();
}

void FreeControl::onDisable() { _enableEvent = false; }

void FreeControl::onEnable() { _enableEvent = true; }

void FreeControl::onDestroy() { onDisable(); }

void FreeControl::resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    _width = win_width;
    _height = win_height;
}

void FreeControl::inputEvent(const InputEvent &inputEvent) {
    if (_enableEvent) {
        if (inputEvent.GetSource() == EventSource::KEYBOARD) {
            const auto &key_event = static_cast<const KeyInputEvent &>(inputEvent);
            if (key_event.GetAction() == KeyAction::DOWN) {
                onKeyDown(key_event.GetCode());
            } else if (key_event.GetAction() == KeyAction::UP) {
                onKeyUp(key_event.GetCode());
            }
        } else if (inputEvent.GetSource() == EventSource::MOUSE) {
            const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(inputEvent);
            if (mouse_button.GetAction() == MouseAction::DOWN) {
                onMouseDown(mouse_button.GetPosX(), mouse_button.GetPosY());
            } else if (mouse_button.GetAction() == MouseAction::UP) {
                onMouseUp();
            } else if (mouse_button.GetAction() == MouseAction::MOVE) {
                onMouseMove(mouse_button.GetPosX(), mouse_button.GetPosY());
            }
        } else if (inputEvent.GetSource() == EventSource::SCROLL) {
        } else if (inputEvent.GetSource() == EventSource::TOUCHSCREEN) {
            // TODO
        }
    }
}

void FreeControl::onKeyDown(KeyCode key) {
    switch (key) {
        case KeyCode::W:
        case KeyCode::UP:
            _moveForward = true;
            break;

        case KeyCode::S:
        case KeyCode::DOWN:
            _moveBackward = true;
            break;

        case KeyCode::A:
        case KeyCode::LEFT:
            _moveLeft = true;
            break;

        case KeyCode::D:
        case KeyCode::RIGHT:
            _moveRight = true;
            break;

        default:
            break;
    }
}

void FreeControl::onKeyUp(KeyCode key) {
    switch (key) {
        case KeyCode::W:
        case KeyCode::UP:
            _moveForward = false;
            break;

        case KeyCode::S:
        case KeyCode::DOWN:
            _moveBackward = false;
            break;

        case KeyCode::A:
        case KeyCode::LEFT:
            _moveLeft = false;
            break;

        case KeyCode::D:
        case KeyCode::RIGHT:
            _moveRight = false;
            break;

        default:
            break;
    }
}

void FreeControl::onMouseDown(double xpos, double ypos) {
    press = true;
    _rotateOri[0] = xpos;
    _rotateOri[1] = ypos;
}

void FreeControl::onMouseUp() { press = false; }

void FreeControl::onMouseMove(double clientX, double clientY) {
    if (!press) return;
    if (!enabled()) return;

    const auto movementX = clientX - _rotateOri[0];
    const auto movementY = clientY - _rotateOri[1];
    _rotateOri[0] = clientX;
    _rotateOri[1] = clientY;
    const auto factorX = 180.0 / _width;
    const auto factorY = 180.0 / _height;
    const auto actualX = movementX * factorX;
    const auto actualY = movementY * factorY;

    rotate(-actualX, actualY);
}

void FreeControl::rotate(float alpha, float beta) {
    _theta += degreesToRadians(alpha);
    _phi += degreesToRadians(beta);
    _phi = clamp<float>(_phi, 1e-6, M_PI - 1e-6);
    _spherical._theta = _theta;
    _spherical._phi = _phi;
    _spherical.setToVec3(_v3Cache);
    Point3F offset = entity()->transform->position() + _v3Cache;
    _v3Cache = Vector3F(offset.x, offset.y, offset.y);
    entity()->transform->lookAt(offset, Vector3F(0, 1, 0));
}

void FreeControl::onUpdate(float delta) {
    if (enabled() == false) return;

    const auto actualMoveSpeed = delta * movementSpeed;
    _forward = entity()->transform->worldForward();
    _right = entity()->transform->worldRight();

    if (_moveForward) {
        entity()->transform->translate(_forward * actualMoveSpeed, false);
    }
    if (_moveBackward) {
        entity()->transform->translate(_forward * (-actualMoveSpeed), false);
    }
    if (_moveLeft) {
        entity()->transform->translate(_right * (-actualMoveSpeed), false);
    }
    if (_moveRight) {
        entity()->transform->translate(_right * actualMoveSpeed, false);
    }

    if (floorMock) {
        const auto position = entity()->transform->position();
        if (position.y != floorY) {
            entity()->transform->setPosition(position.x, floorY, position.z);
        }
    }
}

void FreeControl::updateSpherical() {
    _v3Cache = entity()->transform->rotationQuaternion() * Vector3F(0, 0, -1);
    _spherical.setFromVec3(_v3Cache);
    _theta = _spherical._theta;
    _phi = _spherical._phi;
}

}  // namespace vox::control
