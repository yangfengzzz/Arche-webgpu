#  Copyright (c) 2022 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.12)

project(vox.editor LANGUAGES C CXX)

file(GLOB UI_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cpp
        size_converter.cpp
        editor_utils.cpp
        editor_actions.cpp
        editor_resources.cpp
        panels_manager.cpp
        entity_creation_menu.cpp
        editor_application.cpp
        demo_application.cpp)

file(GLOB VIEW_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/view/*.cpp)

file(GLOB IMGUI_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp)

set(PROFILING
        profiling/profiler.cpp
        profiling/profiler.h
        profiling/profiler_report.h
        profiling/profiler_spy.cpp
        profiling/profiler_spy.h)

set(FRAMEWORK_FILES
        ini_file.cpp
        ${UI_FILES}
        ${VIEW_FILES}
        ${IMGUI_FILES}
        ${PROFILING}
        )

set(LOADER_SOURCE ../third_party/assimp/include
        ../third_party/assimp/build_release/include)

set(PROJECT_FILES
        ${FRAMEWORK_FILES}
        )

# No need for explict casts from vk::HandleType to VkHandleType on ANDROID
if (ANDROID)
    add_definitions(-DVULKAN_HPP_TYPESAFE_CONVERSION=1)
endif ()

# Add files based on platform
if (ANDROID)
    list(APPEND PROJECT_FILES ${ANDROID_FILES})
    message(STATUS "Android platform detected")
else ()
    if (DIRECT_TO_DISPLAY)
        list(APPEND PROJECT_FILES ${LINUX_D2D_FILES})
        message(STATUS "Unix d2d platform detected")
    else ()
        list(APPEND PROJECT_FILES ${GLFW_FILES})
        if (WIN32)
            list(APPEND PROJECT_FILES ${WINDOWS_FILES})
            message(STATUS "Windows platform detected")
        else ()
            list(APPEND PROJECT_FILES ${UNIX_FILES})
            message(STATUS "Unix platform detected")
        endif ()
    endif ()
endif ()

# Pre compiled headers
vulkan_samples_pch(PROJECT_FILES pch.cpp)

add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})

# compiler flags based on compiler type
if (NOT MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC -fexceptions)
endif ()

if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /MP)
endif ()

if (${VKB_VALIDATION_LAYERS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VKB_VALIDATION_LAYERS)
endif ()

# GPU assisted validation layers are not available on macOS.
if (${VKB_VALIDATION_LAYERS_GPU_ASSISTED})
    if (APPLE)
        message(WARNING "GPU assisted validation layers are not currently available on macOS.")
    else ()
        target_compile_definitions(${PROJECT_NAME} PUBLIC VKB_VALIDATION_LAYERS_GPU_ASSISTED)
    endif ()
endif ()

if (${VKB_VULKAN_DEBUG})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VKB_VULKAN_DEBUG)
endif ()

if (${VKB_WARNINGS_AS_ERRORS})
    message(STATUS "Warnings as Errors Enabled")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)
    endif ()
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ../
        ${CMAKE_CURRENT_SOURCE_DIR} ${LOADER_SOURCE}
        ../third_party/nlohmann/include
        ../third_party/dawn/cmake-release/gen/include
        ../third_party/dawn/include
        ../third_party/dawn
        ../third_party/jolt)

target_compile_definitions(${PROJECT_NAME} PUBLIC DAWN_ENABLE_BACKEND_METAL DAWN_ENABLE_BACKEND_NULL TINT_BUILD_SPV_READER JPH_PROFILE_ENABLED JPH_DEBUG_RENDERER)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        ktx
        stb
        astc
        imgui
        glslang
        SPIRV
        hwcpipe
        spirv-cross-glsl
        glslang-default-resource-limits
        spdlog
        CLI11::CLI11)

if (${NEED_LINK_ATOMIC})
    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
endif ()

# Link platform specific libraries
if (ANDROID)
    target_link_libraries(${PROJECT_NAME} PUBLIC log android native_app_glue)
else ()
    if (DIRECT_TO_DISPLAY)
        target_link_libraries(${PROJECT_NAME} PRIVATE dl)
    else ()
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
    endif ()
endif ()
