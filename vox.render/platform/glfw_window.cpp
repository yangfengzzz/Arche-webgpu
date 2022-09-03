//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/glfw_window.h"

#include <unordered_map>

#include "vox.render/error.h"

VKBP_DISABLE_WARNINGS()

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

VKBP_ENABLE_WARNINGS()

#include "vox.render/platform/platform.h"

namespace vox {
namespace {
void ErrorCallback(int error, const char *description) { LOGE("GLFW Error (code {}): {}", error, description) }

void WindowCloseCallback(GLFWwindow *window) { glfwSetWindowShouldClose(window, GLFW_TRUE); }

void WindowSizeCallback(GLFWwindow *window, int width, int height) {
    if (auto platform = reinterpret_cast<Platform *>(glfwGetWindowUserPointer(window))) {
        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        platform->Resize(width, height, fb_width, fb_height);
    }
}

void WindowFocusCallback(GLFWwindow *window, int focused) {
    if (auto platform = reinterpret_cast<Platform *>(glfwGetWindowUserPointer(window))) {
        platform->SetFocus(focused);
    }
}

inline KeyCode TranslateKeyCode(int key) {
    static const std::unordered_map<int, KeyCode> kEyLookup = {
            {GLFW_KEY_SPACE, KeyCode::SPACE},
            {GLFW_KEY_APOSTROPHE, KeyCode::APOSTROPHE},
            {GLFW_KEY_COMMA, KeyCode::COMMA},
            {GLFW_KEY_MINUS, KeyCode::MINUS},
            {GLFW_KEY_PERIOD, KeyCode::PERIOD},
            {GLFW_KEY_SLASH, KeyCode::SLASH},
            {GLFW_KEY_0, KeyCode::_0},
            {GLFW_KEY_1, KeyCode::_1},
            {GLFW_KEY_2, KeyCode::_2},
            {GLFW_KEY_3, KeyCode::_3},
            {GLFW_KEY_4, KeyCode::_4},
            {GLFW_KEY_5, KeyCode::_5},
            {GLFW_KEY_6, KeyCode::_6},
            {GLFW_KEY_7, KeyCode::_7},
            {GLFW_KEY_8, KeyCode::_8},
            {GLFW_KEY_9, KeyCode::_9},
            {GLFW_KEY_SEMICOLON, KeyCode::SEMICOLON},
            {GLFW_KEY_EQUAL, KeyCode::EQUAL},
            {GLFW_KEY_A, KeyCode::A},
            {GLFW_KEY_B, KeyCode::B},
            {GLFW_KEY_C, KeyCode::C},
            {GLFW_KEY_D, KeyCode::D},
            {GLFW_KEY_E, KeyCode::E},
            {GLFW_KEY_F, KeyCode::F},
            {GLFW_KEY_G, KeyCode::G},
            {GLFW_KEY_H, KeyCode::H},
            {GLFW_KEY_I, KeyCode::I},
            {GLFW_KEY_J, KeyCode::J},
            {GLFW_KEY_K, KeyCode::K},
            {GLFW_KEY_L, KeyCode::L},
            {GLFW_KEY_M, KeyCode::M},
            {GLFW_KEY_N, KeyCode::N},
            {GLFW_KEY_O, KeyCode::O},
            {GLFW_KEY_P, KeyCode::P},
            {GLFW_KEY_Q, KeyCode::Q},
            {GLFW_KEY_R, KeyCode::R},
            {GLFW_KEY_S, KeyCode::S},
            {GLFW_KEY_T, KeyCode::T},
            {GLFW_KEY_U, KeyCode::U},
            {GLFW_KEY_V, KeyCode::V},
            {GLFW_KEY_W, KeyCode::W},
            {GLFW_KEY_X, KeyCode::X},
            {GLFW_KEY_Y, KeyCode::Y},
            {GLFW_KEY_Z, KeyCode::Z},
            {GLFW_KEY_LEFT_BRACKET, KeyCode::LEFT_BRACKET},
            {GLFW_KEY_BACKSLASH, KeyCode::BACKSLASH},
            {GLFW_KEY_RIGHT_BRACKET, KeyCode::RIGHT_BRACKET},
            {GLFW_KEY_GRAVE_ACCENT, KeyCode::GRAVE_ACCENT},
            {GLFW_KEY_ESCAPE, KeyCode::ESCAPE},
            {GLFW_KEY_ENTER, KeyCode::ENTER},
            {GLFW_KEY_TAB, KeyCode::TAB},
            {GLFW_KEY_BACKSPACE, KeyCode::BACKSPACE},
            {GLFW_KEY_INSERT, KeyCode::INSERT},
            {GLFW_KEY_DELETE, KeyCode::DEL_KEY},
            {GLFW_KEY_RIGHT, KeyCode::RIGHT},
            {GLFW_KEY_LEFT, KeyCode::LEFT},
            {GLFW_KEY_DOWN, KeyCode::DOWN},
            {GLFW_KEY_UP, KeyCode::UP},
            {GLFW_KEY_PAGE_UP, KeyCode::PAGE_UP},
            {GLFW_KEY_PAGE_DOWN, KeyCode::PAGE_DOWN},
            {GLFW_KEY_HOME, KeyCode::HOME},
            {GLFW_KEY_END, KeyCode::END},
            {GLFW_KEY_CAPS_LOCK, KeyCode::CAPS_LOCK},
            {GLFW_KEY_SCROLL_LOCK, KeyCode::SCROLL_LOCK},
            {GLFW_KEY_NUM_LOCK, KeyCode::NUM_LOCK},
            {GLFW_KEY_PRINT_SCREEN, KeyCode::PRINT_SCREEN},
            {GLFW_KEY_PAUSE, KeyCode::PAUSE},
            {GLFW_KEY_F1, KeyCode::F1},
            {GLFW_KEY_F2, KeyCode::F2},
            {GLFW_KEY_F3, KeyCode::F3},
            {GLFW_KEY_F4, KeyCode::F4},
            {GLFW_KEY_F5, KeyCode::F5},
            {GLFW_KEY_F6, KeyCode::F6},
            {GLFW_KEY_F7, KeyCode::F7},
            {GLFW_KEY_F8, KeyCode::F8},
            {GLFW_KEY_F9, KeyCode::F9},
            {GLFW_KEY_F10, KeyCode::F10},
            {GLFW_KEY_F11, KeyCode::F11},
            {GLFW_KEY_F12, KeyCode::F12},
            {GLFW_KEY_KP_0, KeyCode::KP_0},
            {GLFW_KEY_KP_1, KeyCode::KP_1},
            {GLFW_KEY_KP_2, KeyCode::KP_2},
            {GLFW_KEY_KP_3, KeyCode::KP_3},
            {GLFW_KEY_KP_4, KeyCode::KP_4},
            {GLFW_KEY_KP_5, KeyCode::KP_5},
            {GLFW_KEY_KP_6, KeyCode::KP_6},
            {GLFW_KEY_KP_7, KeyCode::KP_7},
            {GLFW_KEY_KP_8, KeyCode::KP_8},
            {GLFW_KEY_KP_9, KeyCode::KP_9},
            {GLFW_KEY_KP_DECIMAL, KeyCode::KP_DECIMAL},
            {GLFW_KEY_KP_DIVIDE, KeyCode::KP_DIVIDE},
            {GLFW_KEY_KP_MULTIPLY, KeyCode::KP_MULTIPLY},
            {GLFW_KEY_KP_SUBTRACT, KeyCode::KP_SUBTRACT},
            {GLFW_KEY_KP_ADD, KeyCode::KP_ADD},
            {GLFW_KEY_KP_ENTER, KeyCode::KP_ENTER},
            {GLFW_KEY_KP_EQUAL, KeyCode::KP_EQUAL},
            {GLFW_KEY_LEFT_SHIFT, KeyCode::LEFT_SHIFT},
            {GLFW_KEY_LEFT_CONTROL, KeyCode::LEFT_CONTROL},
            {GLFW_KEY_LEFT_ALT, KeyCode::LEFT_ALT},
            {GLFW_KEY_RIGHT_SHIFT, KeyCode::RIGHT_SHIFT},
            {GLFW_KEY_RIGHT_CONTROL, KeyCode::RIGHT_CONTROL},
            {GLFW_KEY_RIGHT_ALT, KeyCode::RIGHT_ALT},
    };

    auto key_it = kEyLookup.find(key);

    if (key_it == kEyLookup.end()) {
        return KeyCode::UNKNOWN;
    }

    return key_it->second;
}

inline KeyAction TranslateKeyAction(int action) {
    if (action == GLFW_PRESS) {
        return KeyAction::DOWN;
    } else if (action == GLFW_RELEASE) {
        return KeyAction::UP;
    } else if (action == GLFW_REPEAT) {
        return KeyAction::REPEAT;
    }

    return KeyAction::UNKNOWN;
}

void KeyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
    KeyCode key_code = TranslateKeyCode(key);
    KeyAction key_action = TranslateKeyAction(action);

    if (auto platform = reinterpret_cast<Platform *>(glfwGetWindowUserPointer(window))) {
        platform->InputEvent(KeyInputEvent{key_code, key_action});
    }
}

inline MouseButton TranslateMouseButton(int button) {
    if (button < GLFW_MOUSE_BUTTON_6) {
        return static_cast<MouseButton>(button);
    }

    return MouseButton::UNKNOWN;
}

inline MouseAction TranslateMouseAction(int action) {
    if (action == GLFW_PRESS) {
        return MouseAction::DOWN;
    } else if (action == GLFW_RELEASE) {
        return MouseAction::UP;
    }

    return MouseAction::UNKNOWN;
}

void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
    if (auto *platform = reinterpret_cast<Platform *>(glfwGetWindowUserPointer(window))) {
        platform->InputEvent(MouseButtonInputEvent{MouseButton::UNKNOWN, MouseAction::MOVE, static_cast<float>(xpos),
                                                   static_cast<float>(ypos)});
    }
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int /*mods*/) {
    MouseAction mouse_action = TranslateMouseAction(action);

    if (auto *platform = reinterpret_cast<Platform *>(glfwGetWindowUserPointer(window))) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        platform->InputEvent(MouseButtonInputEvent{TranslateMouseButton(button), mouse_action, static_cast<float>(xpos),
                                                   static_cast<float>(ypos)});
    }
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    if (auto *platform = reinterpret_cast<Platform *>(glfwGetWindowUserPointer(window))) {
        platform->InputEvent(ScrollInputEvent(xoffset, yoffset));
    }
}

}  // namespace

GlfwWindow::GlfwWindow(Platform *platform, const Window::Properties &properties) : Window(properties) {
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, false);
#endif

    if (!glfwInit()) {
        throw std::runtime_error("GLFW couldn't be initialized.");
    }

    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    switch (properties.mode) {
        case Window::Mode::FULLSCREEN: {
            auto *monitor = glfwGetPrimaryMonitor();
            const auto *mode = glfwGetVideoMode(monitor);
            handle_ = glfwCreateWindow(mode->width, mode->height, properties.title.c_str(), monitor, nullptr);
            break;
        }

        case Window::Mode::FULLSCREEN_BORDERLESS: {
            auto *monitor = glfwGetPrimaryMonitor();
            const auto *mode = glfwGetVideoMode(monitor);
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
            handle_ = glfwCreateWindow(mode->width, mode->height, properties.title.c_str(), monitor, nullptr);
            break;
        }

        default:
            handle_ = glfwCreateWindow(properties.extent.width, properties.extent.height, properties.title.c_str(),
                                       nullptr, nullptr);
            break;
    }

    Resize(Extent{properties.extent.width, properties.extent.height});

    if (!handle_) {
        throw std::runtime_error("Couldn't create glfw window.");
    }

    glfwSetWindowUserPointer(handle_, platform);

    glfwSetWindowCloseCallback(handle_, WindowCloseCallback);
    glfwSetWindowSizeCallback(handle_, WindowSizeCallback);
    glfwSetWindowFocusCallback(handle_, WindowFocusCallback);
    glfwSetKeyCallback(handle_, KeyCallback);
    glfwSetCursorPosCallback(handle_, CursorPositionCallback);
    glfwSetMouseButtonCallback(handle_, MouseButtonCallback);
    glfwSetScrollCallback(handle_, ScrollCallback);

    glfwSetInputMode(handle_, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(handle_, GLFW_STICKY_MOUSE_BUTTONS, 1);
}

GlfwWindow::~GlfwWindow() { glfwTerminate(); }

VkSurfaceKHR GlfwWindow::CreateSurface(Instance &instance) {
    return CreateSurface(instance.GetHandle(), VK_NULL_HANDLE);
}

VkSurfaceKHR GlfwWindow::CreateSurface(VkInstance instance, VkPhysicalDevice) {
    if (instance == VK_NULL_HANDLE || !handle_) {
        return VK_NULL_HANDLE;
    }

    VkSurfaceKHR surface;

    VkResult err_code = glfwCreateWindowSurface(instance, handle_, nullptr, &surface);

    if (err_code != VK_SUCCESS) {
        return nullptr;
    }

    return surface;
}

bool GlfwWindow::ShouldClose() { return glfwWindowShouldClose(handle_); }

void GlfwWindow::ProcessEvents() { glfwPollEvents(); }

void GlfwWindow::Close() { glfwSetWindowShouldClose(handle_, GLFW_TRUE); }

/// @brief It calculates the dpi factor using the density from GLFW physical size
/// <a href="https://www.glfw.org/docs/latest/monitor_guide.html#monitor_size">GLFW docs for dpi</a>
float GlfwWindow::GetDpiFactor() const {
    auto primary_monitor = glfwGetPrimaryMonitor();
    auto vidmode = glfwGetVideoMode(primary_monitor);

    int width_mm, height_mm;
    glfwGetMonitorPhysicalSize(primary_monitor, &width_mm, &height_mm);

    // As suggested by the GLFW monitor guide
    static const float kInchToMm = 25.0f;
    static const float kWinBaseDensity = 96.0f;

    auto dpi = static_cast<uint32_t>(vidmode->width / (width_mm / kInchToMm));
    auto dpi_factor = dpi / kWinBaseDensity;
    return dpi_factor;
}

float GlfwWindow::GetContentScaleFactor() const {
    int fb_width, fb_height;
    glfwGetFramebufferSize(handle_, &fb_width, &fb_height);
    int win_width, win_height;
    glfwGetWindowSize(handle_, &win_width, &win_height);

    // We could return a 2D result here instead of a scalar,
    // but non-uniform scaling is very unlikely, and would
    // require significantly more changes in the IMGUI integration
    return static_cast<float>(fb_width) / win_width;
}

GLFWwindow *GlfwWindow::Handle() { return handle_; }

}  // namespace vox
