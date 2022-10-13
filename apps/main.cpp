//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation/animation_sample.h"
#include "apps/assimp_app.h"
#include "apps/atomic_compute_app.h"
#include "apps/gui_app.h"
#include "apps/navigation_app.h"
#include "apps/physics_collision_app.h"
#include "apps/physics_samples/physics_samples.h"
#include "apps/plugins/plugins.h"
#include "apps/primitive_app.h"
#include "apps/rendering/rendering_sample.h"
#include "apps/scoped_autorelease_pool.h"
#include "apps/toolkit/toolkit_sample.h"
#include "vox.render/platform/platform.h"

// MARK: - Entry
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#include "vox.render/platform/android/android_platform.h"
void android_main(android_app *state) {
    vox::AndroidPlatform platform{state};
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
#include "vox.render/platform/windows/windows_platform.h"
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    vox::WindowsPlatform platform{hInstance, hPrevInstance, lpCmdLine, nCmdShow};
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
#include "vox.render/platform/unix/unix_d2d_platform.h"
int main(int argc, char *argv[]) {
    vox::UnixD2DPlatform platform{argc, argv};
#else

#include "vox.render/platform/unix/unix_platform.h"

int main(int argc, char* argv[]) {
#if defined(VK_USE_PLATFORM_METAL_EXT)
    vox::UnixPlatform platform{vox::UnixType::MAC, argc, argv};
#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
    vox::UnixPlatform platform{vox::UnixType::Linux, argc, argv};
#endif
#endif

    auto code = platform.Initialize(plugins::GetAll());
    if (code == vox::ExitCode::SUCCESS) {
        platform.SetApp(std::make_unique<vox::GridApp>());
        code = platform.MainLoop();
    }
    platform.Terminate(code);

#ifndef VK_USE_PLATFORM_ANDROID_KHR
    return EXIT_SUCCESS;
#endif
}
