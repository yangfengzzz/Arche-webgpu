//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/unix/direct_window.h"

#include "vox.render/core/instance.h"
#include "vox.render/platform/headless_window.h"
#include "vox.render/platform/unix/direct_window.h"

namespace vox {
namespace {
static KeyCode key_map[] = {KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::BACKSPACE,
                            KeyCode::TAB,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::ENTER,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::ESCAPE,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::UNKNOWN,
                            KeyCode::SPACE,
                            KeyCode::_1,
                            KeyCode::APOSTROPHE,
                            KeyCode::BACKSLASH,
                            KeyCode::_4,
                            KeyCode::_5,
                            KeyCode::_7,
                            KeyCode::APOSTROPHE,
                            KeyCode::_9,
                            KeyCode::_0,
                            KeyCode::_8,
                            KeyCode::EQUAL,
                            KeyCode::COMMA,
                            KeyCode::MINUS,
                            KeyCode::PERIOD,
                            KeyCode::SLASH,
                            KeyCode::_0,
                            KeyCode::_1,
                            KeyCode::_2,
                            KeyCode::_3,
                            KeyCode::_4,
                            KeyCode::_5,
                            KeyCode::_6,
                            KeyCode::_7,
                            KeyCode::_8,
                            KeyCode::_9,
                            KeyCode::SEMICOLON,
                            KeyCode::SEMICOLON,
                            KeyCode::COMMA,
                            KeyCode::EQUAL,
                            KeyCode::PERIOD,
                            KeyCode::SLASH,
                            KeyCode::_2,
                            KeyCode::A,
                            KeyCode::B,
                            KeyCode::C,
                            KeyCode::D,
                            KeyCode::E,
                            KeyCode::F,
                            KeyCode::G,
                            KeyCode::H,
                            KeyCode::I,
                            KeyCode::J,
                            KeyCode::K,
                            KeyCode::L,
                            KeyCode::M,
                            KeyCode::N,
                            KeyCode::O,
                            KeyCode::P,
                            KeyCode::Q,
                            KeyCode::R,
                            KeyCode::S,
                            KeyCode::T,
                            KeyCode::U,
                            KeyCode::V,
                            KeyCode::W,
                            KeyCode::X,
                            KeyCode::Y,
                            KeyCode::Z,
                            KeyCode::LEFT_BRACKET,
                            KeyCode::BACKSLASH,
                            KeyCode::RIGHT_BRACKET,
                            KeyCode::_6,
                            KeyCode::MINUS,
                            KeyCode::GRAVE_ACCENT,
                            KeyCode::A,
                            KeyCode::B,
                            KeyCode::C,
                            KeyCode::D,
                            KeyCode::E,
                            KeyCode::F,
                            KeyCode::G,
                            KeyCode::H,
                            KeyCode::I,
                            KeyCode::J,
                            KeyCode::K,
                            KeyCode::L,
                            KeyCode::M,
                            KeyCode::N,
                            KeyCode::O,
                            KeyCode::P,
                            KeyCode::Q,
                            KeyCode::R,
                            KeyCode::S,
                            KeyCode::T,
                            KeyCode::U,
                            KeyCode::V,
                            KeyCode::W,
                            KeyCode::X,
                            KeyCode::Y,
                            KeyCode::Z,
                            KeyCode::LEFT_BRACKET,
                            KeyCode::BACKSLASH,
                            KeyCode::RIGHT_BRACKET,
                            KeyCode::GRAVE_ACCENT,
                            KeyCode::BACKSPACE};

static KeyCode map_multichar_key(int tty_fd, KeyCode initial) {
    static std::map<std::string, KeyCode> mc_map;  // Static for one-time-init
    if (mc_map.size() == 0) {
        mc_map["[A"] = KeyCode::UP;
        mc_map["[B"] = KeyCode::DOWN;
        mc_map["[C"] = KeyCode::RIGHT;
        mc_map["[D"] = KeyCode::LEFT;
        mc_map["[2~"] = KeyCode::INSERT;
        mc_map["[3~"] = KeyCode::DEL_KEY;
        mc_map["[5~"] = KeyCode::PAGE_UP;
        mc_map["[6~"] = KeyCode::PAGE_DOWN;
        mc_map["[H"] = KeyCode::HOME;
        mc_map["[F"] = KeyCode::END;
        mc_map["OP"] = KeyCode::F1;
        mc_map["OQ"] = KeyCode::F2;
        mc_map["OR"] = KeyCode::F3;
        mc_map["OS"] = KeyCode::F4;
        mc_map["[15~"] = KeyCode::F5;
        mc_map["[17~"] = KeyCode::F6;
        mc_map["[18~"] = KeyCode::F7;
        mc_map["[19~"] = KeyCode::F8;
        mc_map["[20~"] = KeyCode::F9;
        mc_map["[21~"] = KeyCode::F10;
        mc_map["[23~"] = KeyCode::F11;
        mc_map["[24~"] = KeyCode::F12;
    }

    char key;
    std::string buf;

    while (::read(tty_fd, &key, 1) == 1) {
        buf += key;
    }

    if (buf.size() == 0) {
        return initial;  // Nothing new read, just return the initial char
    }

    // Is it a code we recognise?
    auto iter = mc_map.find(buf);
    if (iter != mc_map.end()) {
        return iter->second;
    }

    return KeyCode::UNKNOWN;
}

}  // namespace
DirectWindow::DirectWindow(Platform *platform, const Window::Properties &properties)
    : Window(properties), platform{platform} {
    // Setup tty for reading keyboard from console
    if ((tty_fd = open("/dev/tty", O_RDWR | O_NDELAY)) > 0) {
        tcgetattr(tty_fd, &termio_prev);
        tcgetattr(tty_fd, &termio);
        cfmakeraw(&termio);
        termio.c_lflag |= ISIG;
        termio.c_oflag |= OPOST | ONLCR;
        termio.c_cc[VMIN] = 1;
        termio.c_cc[VTIME] = 0;

        if (tcsetattr(tty_fd, TCSANOW, &termio) == -1) LOGW("Failed to set attribs for '/dev/tty'");
    }

    platform->SetFocus(true);
}

DirectWindow::~DirectWindow() {
    if (tty_fd > 0) {
        tcsetattr(tty_fd, TCSANOW, &termio_prev);
    }
}

VkSurfaceKHR DirectWindow::CreateSurface(Instance &instance) {
    return CreateSurface(instance.GetHandle(), instance.get_first_gpu().GetHandle());
}

VkSurfaceKHR DirectWindow::CreateSurface(VkInstance instance, VkPhysicalDevice phys_dev) {
    if (instance == VK_NULL_HANDLE || phys_dev == VK_NULL_HANDLE) {
        return VK_NULL_HANDLE;
    }

    // Query the display properties
    uint32_t num_displays;
    VK_CHECK(vkGetPhysicalDeviceDisplayPropertiesKHR(phys_dev, &num_displays, nullptr));

    if (num_displays == 0) {
        LOGE("Direct-to-display: No displays found");
        return VK_NULL_HANDLE;
    }

    VkDisplayPropertiesKHR display_properties;
    num_displays = 1;
    VK_CHECK(vkGetPhysicalDeviceDisplayPropertiesKHR(phys_dev, &num_displays, &display_properties));

    VkDisplayKHR display = display_properties.display;

    // Calculate the display DPI
    dpi = 25.4f * display_properties.physicalResolution.width / display_properties.physicalDimensions.width;

    // Query display mode properties
    uint32_t num_modes;
    VK_CHECK(vkGetDisplayModePropertiesKHR(phys_dev, display, &num_modes, nullptr));

    if (num_modes == 0) {
        LOGE("Direct-to-display: No display modes found");
        return VK_NULL_HANDLE;
    }

    VkDisplayModePropertiesKHR mode_props;
    num_modes = 1;
    VK_CHECK(vkGetDisplayModePropertiesKHR(phys_dev, display, &num_modes, &mode_props));

    // Get the list of planes
    uint32_t num_planes;
    VK_CHECK(vkGetPhysicalDeviceDisplayPlanePropertiesKHR(phys_dev, &num_planes, nullptr));

    if (num_planes == 0) {
        LOGE("Direct-to-display: No display planes found");
        return VK_NULL_HANDLE;
    }

    std::vector<VkDisplayPlanePropertiesKHR> plane_properties(num_planes);

    VK_CHECK(vkGetPhysicalDeviceDisplayPlanePropertiesKHR(phys_dev, &num_planes, plane_properties.data()));

    // Find a compatible plane index
    uint32_t plane_index = FindCompatiblePlane(phys_dev, display, plane_properties);
    if (plane_index == ~0U) {
        return VK_NULL_HANDLE;
    }

    VkExtent2D image_extent;
    image_extent.width = mode_props.parameters.visibleRegion.width;
    image_extent.height = mode_props.parameters.visibleRegion.height;

    VkDisplaySurfaceCreateInfoKHR surface_create_info{};
    surface_create_info.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
    surface_create_info.displayMode = mode_props.displayMode;
    surface_create_info.planeIndex = plane_index;
    surface_create_info.planeStackIndex = plane_properties[plane_index].currentStackIndex;
    surface_create_info.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    surface_create_info.alphaMode = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR;
    surface_create_info.imageExtent = image_extent;

    VkSurfaceKHR surface;
    VK_CHECK(vkCreateDisplayPlaneSurfaceKHR(instance, &surface_create_info, nullptr, &surface));

    return surface;
}

uint32_t DirectWindow::FindCompatiblePlane(VkPhysicalDevice phys_dev,
                                           VkDisplayKHR display,
                                           const std::vector<VkDisplayPlanePropertiesKHR> &plane_properties) {
    // Find a plane compatible with the display
    for (uint32_t pi = 0; pi < plane_properties.size(); ++pi) {
        if ((plane_properties[pi].currentDisplay != VK_NULL_HANDLE) &&
            (plane_properties[pi].currentDisplay != display)) {
            continue;
        }

        uint32_t num_supported;
        VK_CHECK(vkGetDisplayPlaneSupportedDisplaysKHR(phys_dev, pi, &num_supported, nullptr));

        if (num_supported == 0) {
            continue;
        }

        std::vector<VkDisplayKHR> supported_displays(num_supported);

        VK_CHECK(vkGetDisplayPlaneSupportedDisplaysKHR(phys_dev, pi, &num_supported, supported_displays.data()));

        for (uint32_t i = 0; i < num_supported; ++i) {
            if (supported_displays[i] == display) {
                return pi;
            }
        }
    }

    LOGE("Direct-to-display: No plane found compatible with the display");
    return ~0U;
}

void DirectWindow::ProcessEvents() {
    if (tty_fd > 0) {
        if (key_down != KeyCode::UNKNOWN) {
            // Signal release for the key we previously reported as down
            // (we don't get separate press & release from the terminal)
            platform->InputEvent(KeyInputEvent{key_down, KeyAction::UP});
            key_down = KeyCode::UNKNOWN;
        }

        // See if there is a new keypress
        uint8_t key = 0;
        int bytes = ::read(tty_fd, &key, 1);

        if (bytes > 0 && key > 0 && (key < sizeof(key_map) / sizeof(KeyCode))) {
            // Map the key
            key_down = key_map[key];

            // Is this potentially a multi-character code?
            if (key_down == KeyCode::ESCAPE) {
                key_down = map_multichar_key(tty_fd, key_down);
            }

            // Signal the press
            platform->InputEvent(KeyInputEvent{key_down, KeyAction::DOWN});
        }
    }
}

bool DirectWindow::ShouldClose() { return !keep_running; }

void DirectWindow::Close() { keep_running = false; }

float DirectWindow::GetDpiFactor() const {
    const float win_base_density = 96.0f;
    return dpi / win_base_density;
}

}  // namespace vox
