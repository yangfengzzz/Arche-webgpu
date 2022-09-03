//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/input_events.h"

namespace vox {
InputEvent::InputEvent(EventSource source) : source_{source} {}

EventSource InputEvent::GetSource() const { return source_; }

// MARK: - Key Event
KeyInputEvent::KeyInputEvent(KeyCode code, KeyAction action)
    : InputEvent{EventSource::KEYBOARD}, code_{code}, action_{action} {}

KeyCode KeyInputEvent::GetCode() const { return code_; }

KeyAction KeyInputEvent::GetAction() const { return action_; }

// MARK: - Mouse Event
MouseButtonInputEvent::MouseButtonInputEvent(MouseButton button, MouseAction action, float pos_x, float pos_y)
    : InputEvent{EventSource::MOUSE}, button_{button}, action_{action}, pos_x_{pos_x}, pos_y_{pos_y} {}

MouseButton MouseButtonInputEvent::GetButton() const { return button_; }

MouseAction MouseButtonInputEvent::GetAction() const { return action_; }

float MouseButtonInputEvent::GetPosX() const { return pos_x_; }

float MouseButtonInputEvent::GetPosY() const { return pos_y_; }

// MARK: - Scroll Event
ScrollInputEvent::ScrollInputEvent(float offset_x, float offset_y)
    : InputEvent{EventSource::SCROLL}, offset_x_(offset_x), offset_y_(offset_y) {}

float ScrollInputEvent::OffsetX() const { return offset_x_; }

float ScrollInputEvent::OffsetY() const { return offset_y_; }

// MARK: - Touch Event
TouchInputEvent::TouchInputEvent(
        int32_t pointer_id, std::size_t touch_points, TouchAction action, float pos_x, float pos_y)
    : InputEvent{EventSource::TOUCHSCREEN},
      action_{action},
      pointer_id_{pointer_id},
      touch_points_{touch_points},
      pos_x_{pos_x},
      pos_y_{pos_y} {}

TouchAction TouchInputEvent::GetAction() const { return action_; }

int32_t TouchInputEvent::GetPointerId() const { return pointer_id_; }

std::size_t TouchInputEvent::GetTouchPoints() const { return touch_points_; }

float TouchInputEvent::GetPosX() const { return pos_x_; }

float TouchInputEvent::GetPosY() const { return pos_y_; }

}  // namespace vox
