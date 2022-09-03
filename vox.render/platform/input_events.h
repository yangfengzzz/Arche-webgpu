//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstddef>
#include <cstdint>

namespace vox {
class Platform;

enum class EventSource { KEYBOARD, MOUSE, SCROLL, TOUCHSCREEN };

class InputEvent {
public:
    explicit InputEvent(EventSource source);

    [[nodiscard]] EventSource GetSource() const;

private:
    EventSource source_;
};

// MARK: - Key Event
enum class KeyCode {
    UNKNOWN,
    SPACE,
    APOSTROPHE, /* ' */
    COMMA,      /* , */
    MINUS,      /* - */
    PERIOD,     /* . */
    SLASH,      /* / */
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    SEMICOLON, /* ; */
    EQUAL,     /* = */
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LEFT_BRACKET,  /* [ */
    BACKSLASH,     /* \ */
    RIGHT_BRACKET, /* ] */
    GRAVE_ACCENT,  /* ` */
    ESCAPE,
    ENTER,
    TAB,
    BACKSPACE,
    INSERT,
    DEL_KEY,
    RIGHT,
    LEFT,
    DOWN,
    UP,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    BACK,
    CAPS_LOCK,
    SCROLL_LOCK,
    NUM_LOCK,
    PRINT_SCREEN,
    PAUSE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    KP_0,
    KP_1,
    KP_2,
    KP_3,
    KP_4,
    KP_5,
    KP_6,
    KP_7,
    KP_8,
    KP_9,
    KP_DECIMAL,
    KP_DIVIDE,
    KP_MULTIPLY,
    KP_SUBTRACT,
    KP_ADD,
    KP_ENTER,
    KP_EQUAL,
    LEFT_SHIFT,
    LEFT_CONTROL,
    LEFT_ALT,
    RIGHT_SHIFT,
    RIGHT_CONTROL,
    RIGHT_ALT
};

enum class KeyAction { DOWN, UP, REPEAT, UNKNOWN };

class KeyInputEvent : public InputEvent {
public:
    KeyInputEvent(KeyCode code, KeyAction action);

    [[nodiscard]] KeyCode GetCode() const;

    [[nodiscard]] KeyAction GetAction() const;

private:
    KeyCode code_;

    KeyAction action_;
};

// MARK: - Mouse Event
enum class MouseButton { LEFT, RIGHT, MIDDLE, BACK, FORWARD, UNKNOWN };

enum class MouseAction { DOWN, UP, MOVE, UNKNOWN };

class MouseButtonInputEvent : public InputEvent {
public:
    MouseButtonInputEvent(MouseButton button, MouseAction action, float pos_x, float pos_y);

    [[nodiscard]] MouseButton GetButton() const;

    [[nodiscard]] MouseAction GetAction() const;

    [[nodiscard]] float GetPosX() const;

    [[nodiscard]] float GetPosY() const;

private:
    MouseButton button_;

    MouseAction action_;

    float pos_x_;

    float pos_y_;
};

// MARK: - Scroll Event
class ScrollInputEvent : public InputEvent {
public:
    ScrollInputEvent(float offset_x, float offset_y);

    [[nodiscard]] float OffsetX() const;

    [[nodiscard]] float OffsetY() const;

private:
    float offset_x_;

    float offset_y_;
};

// MARK: - Touch Event
enum class TouchAction { DOWN, UP, MOVE, CANCEL, POINTER_DOWN, POINTER_UP, UNKNOWN };

class TouchInputEvent : public InputEvent {
public:
    TouchInputEvent(int32_t pointer_id, size_t pointer_count, TouchAction action, float pos_x, float pos_y);

    [[nodiscard]] TouchAction GetAction() const;

    [[nodiscard]] int32_t GetPointerId() const;

    [[nodiscard]] size_t GetTouchPoints() const;

    [[nodiscard]] float GetPosX() const;

    [[nodiscard]] float GetPosY() const;

private:
    TouchAction action_;

    int32_t pointer_id_;

    size_t touch_points_;

    float pos_x_;

    float pos_y_;
};

}  // namespace vox
