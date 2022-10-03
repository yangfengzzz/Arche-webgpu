#  Copyright (c) 2022 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.#

cmake_minimum_required(VERSION 3.12)
include(android_package)

# create sample app project
project(app LANGUAGES C CXX)

add_subdirectory(plugins)

set(SRC
        atomic_compute_app.cpp
        shadowmap_app.cpp
        cascade_shadowmap_app.cpp
        omni_shadowmap_app.cpp
        cluster_forward_app.cpp
        ibl_app.cpp
        irradiance_app.cpp
        main.cpp
        multi_light_app.cpp
#        particle_app.cpp
        pbr_app.cpp
        primitive_app.cpp
        skybox_app.cpp
        assimp_app.cpp
        skinning_app.cpp
        animation_create_app.cpp
        animation_attach_app.cpp
        animation_blend_app.cpp
        animation_partial_blend_app.cpp
        animation_additive_app.cpp
        animation_ik_lookat_app.cpp
        animation_ik_hand_app.cpp
        animation_ik_floor_app.cpp
        gui_app.cpp
        )

source_group("\\" FILES ${SRC})

# select target type based on platform
if (ANDROID)
    if (CMAKE_VS_NsightTegra_VERSION)
        list(APPEND SRC ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)
    endif ()

    add_library(${PROJECT_NAME} SHARED ${SRC})
else ()
    add_executable(${PROJECT_NAME} WIN32 ${SRC})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        ../
        ../third_party/nlohmann/include
        ../third_party/dawn/cmake-release/gen/include
        ../third_party/dawn/include
        ../third_party/dawn
        ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(${PROJECT_NAME} PUBLIC DAWN_ENABLE_BACKEND_METAL DAWN_ENABLE_BACKEND_NULL TINT_BUILD_SPV_READER)

find_library(Cocoa_LIBRARY Cocoa)
find_library(Metal_LIBRARY Metal)
find_library(IOKit_LIBRARY IOKit)
find_library(IOSurface_LIBRARY IOSurface)
find_library(QuartzCore_LIBRARY QuartzCore)

target_link_libraries(${PROJECT_NAME} PRIVATE vox.render vox.math vox.base vox.simd_math vox.animation vox.toolkit plugins spdlog
        absl_str_format_internal
        absl_strings_internal
        absl_strings -labsl_base
        absl_spinlock_wait
        absl_raw_logging_internal
        absl_throw_delegate
        absl_log_severity
        absl_int128
        dawn_common
        dawn_headers
        dawncpp_headers
        dawncpp
        dawn_proc
        dawn_platform
        dawn_wire
        dawn_native
        tint
        tint_diagnostic_utils
        SPIRV-Tools
        SPIRV-Tools-opt
        ${Cocoa_LIBRARY}
        ${Metal_LIBRARY}
        ${IOKit_LIBRARY}
        ${IOSurface_LIBRARY}
        ${QuartzCore_LIBRARY})

if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/assimp/build_release/bin
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/src/dawn
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/src/dawn/common
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/src/dawn/native
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/src/dawn/platform
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/src/dawn/wire
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/src/tint
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/third_party/spirv-tools/source
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/third_party/spirv-tools/source/opt
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/third_party/abseil/absl/strings
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/third_party/abseil/absl/base
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/third_party/abseil/absl/numeric
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-release/third_party/glfw/src)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/assimp/build_release/bin
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/src/dawn
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/src/dawn/common
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/src/dawn/native
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/src/dawn/platform
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/src/dawn/wire
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/src/tint
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/third_party/spirv-tools/source
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/third_party/spirv-tools/source/opt
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/third_party/abseil/absl/strings
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/third_party/abseil/absl/base
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/third_party/abseil/absl/numeric
            ${CMAKE_SOURCE_DIR}/third_party/dawn/cmake-debug/third_party/glfw/src)
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC
        # Assimp
        assimp)

# Create android project
if (ANDROID)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-u ANativeActivity_onCreate")

    if (CMAKE_VS_NsightTegra_VERSION)
        set_property(TARGET ${PROJECT_NAME} PROPERTY ANDROID_GUI ON)
        set_property(TARGET ${PROJECT_NAME} PROPERTY ANDROID_ASSETS_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../assets)
        set_property(TARGET ${PROJECT_NAME} PROPERTY ANDROID_JAVA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../bldsys/android/java)
    endif ()

    # Add packaging project only if not using CMake's toolchain
    if (CMAKE_SYSTEM_VERSION GREATER 1)
        add_android_package_project(
                NAME ${PROJECT_NAME}_package
                DEPENDS ${PROJECT_NAME}
                ASSET_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../assets
                JAVA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/java
                RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/res
                MANIFEST_FILE ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)
    endif ()

    # Sync assets and shaders
    android_sync_folder(PATH ${CMAKE_CURRENT_SOURCE_DIR}/../assets)
    android_sync_folder(PATH ${CMAKE_CURRENT_SOURCE_DIR}/../shaders)
endif ()

# Create MSVC project
if (MSVC)
    #Set the working directory to the source of the project so developer dont have to
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

    #Configure output paths
    foreach (CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG_TYPE} SUFFIX)
        string(TOLOWER ${CONFIG_TYPE} CONFIG_DIR)
        set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/bin/${CONFIG_DIR}/${TARGET_ARCH})
        set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/lib/${CONFIG_DIR}/${TARGET_ARCH})
        set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/lib/${CONFIG_DIR}/${TARGET_ARCH})
    endforeach ()
endif ()
