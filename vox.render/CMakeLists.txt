#  Copyright (c) 2022 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.12)

project(vox.render LANGUAGES C CXX)

set(FRAMEWORK_FILES
        # Header Files
        buffer_pool.h
        debug_info.h
        fence_pool.h
        semaphore_pool.h
        resource_binding_state.h
        resource_cache.h
        resource_record.h
        resource_replay.h
        graphics_application.h
        forward_application.h
        camera.h
        scene_animator.h
        scene_animation_clip.h
        # Source Files
        debug_info.cpp
        buffer_pool.cpp
        fence_pool.cpp
        semaphore_pool.cpp
        resource_binding_state.cpp
        resource_cache.cpp
        resource_record.cpp
        resource_replay.cpp
        graphics_application.cpp
        forward_application.cpp
        camera.cpp
        scene_animator.cpp
        scene_animation_clip.cpp
        )

set(COMMON_FILES
        # Header Files
        event.h
        event-inl.h
        vk_common.h
        vk_initializers.h
        resource_caching.h
        error.h
        utils.h
        strings.h
        tags.h
        # Source Files
        error.cpp
        vk_common.cpp
        utils.cpp
        strings.cpp)

set(RENDERING_FILES
        # Header files
        rendering/render_element.h
        rendering/pipeline_state.h
        rendering/postprocessing_pipeline.h
        rendering/postprocessing_pass.h
        rendering/postprocessing_renderpass.h
        rendering/postprocessing_computepass.h
        rendering/render_context.h
        rendering/render_frame.h
        rendering/render_pipeline.h
        rendering/render_target.h
        rendering/subpass.h
        # Source files
        rendering/render_element.cpp
        rendering/pipeline_state.cpp
        rendering/postprocessing_pipeline.cpp
        rendering/postprocessing_pass.cpp
        rendering/postprocessing_renderpass.cpp
        rendering/postprocessing_computepass.cpp
        rendering/render_context.cpp
        rendering/render_frame.cpp
        rendering/render_pipeline.cpp
        rendering/render_target.cpp
        rendering/subpass.cpp)

set(SCENE_RESOURCE_FILES
        # Header Files
        renderer.h
        texture_manager.h
        texture.h
        texture/astc_tex.h
        texture/ktx_tex.h
        texture/stb_tex.h
        # Source Files
        renderer.cpp
        texture_manager.cpp
        texture.cpp
        texture/astc_tex.cpp
        texture/ktx_tex.cpp
        texture/stb_tex.cpp)

set(RENDERING_SUBPASSES_FILES
        # Header files
        rendering/subpasses/color_picker_subpass.h
        rendering/subpasses/skybox_subpass.h
        rendering/subpasses/geometry_subpass.h
        # Source files
        rendering/subpasses/color_picker_subpass.cpp
        rendering/subpasses/skybox_subpass.cpp
        rendering/subpasses/geometry_subpass.cpp)

set(STATS_FILES
        # Header Files
        stats/stats.h
        stats/stats_common.h
        stats/stats_provider.h
        stats/frame_time_stats_provider.h
        stats/hwcpipe_stats_provider.h

        # Source Files
        stats/stats.cpp
        stats/stats_provider.cpp
        stats/frame_time_stats_provider.cpp
        stats/hwcpipe_stats_provider.cpp)

set(CORE_FILES
        # Header Files
        core/instance.h
        core/physical_device.h
        core/device.h
        core/debug.h
        core/pipeline_layout.h
        core/pipeline.h
        core/descriptor_set_layout.h
        core/descriptor_pool.h
        core/descriptor_set.h
        core/queue.h
        core/command_pool.h
        core/swapchain.h
        core/command_buffer.h
        core/buffer.h
        core/image.h
        core/image_view.h
        core/sampled_image.h
        core/instance.h
        core/sampler.h
        core/framebuffer.h
        core/render_pass.h
        core/query_pool.h
        core/scratch_buffer.h
        core/acceleration_structure.h
        core/shader_binding_table.h
        core/vulkan_resource.h
        # Source Files
        core/instance.cpp
        core/physical_device.cpp
        core/device.cpp
        core/debug.cpp
        core/image.cpp
        core/pipeline_layout.cpp
        core/pipeline.cpp
        core/descriptor_set_layout.cpp
        core/descriptor_pool.cpp
        core/descriptor_set.cpp
        core/queue.cpp
        core/command_pool.cpp
        core/swapchain.cpp
        core/command_buffer.cpp
        core/buffer.cpp
        core/image.cpp
        core/image_view.cpp
        core/sampled_image.cpp
        core/instance.cpp
        core/sampler.cpp
        core/framebuffer.cpp
        core/render_pass.cpp
        core/query_pool.cpp
        core/scratch_buffer.cpp
        core/acceleration_structure.cpp
        core/shader_binding_table.cpp
        core/vulkan_resource.cpp)

set(DAWN_FILES
        utils/backend_binding.h
        utils/backend_binding.cpp
        utils/metal_binding.mm
        utils/null_binding.cpp)

set(PLATFORM_FILES
        # Header Files
        platform/application.h
        platform/platform.h
        platform/window.h
        platform/filesystem.h
        platform/input_events.h
        platform/configuration.h
        platform/parser.h
        platform/headless_window.h
        platform/plugins/plugin.h
        platform/plugins/plugin_base.h

        # Source Files
        platform/headless_window.cpp
        platform/application.cpp
        platform/platform.cpp
        platform/window.cpp
        platform/filesystem.cpp
        platform/input_events.cpp
        platform/configuration.cpp
        platform/parser.cpp
        platform/plugins/plugin.cpp)

set(GLFW_FILES
        # Header Files
        platform/glfw_window.h
        # Source Files
        platform/glfw_window.cpp
        )

set(PARSER_FILES
        # Header Files
        platform/parsers/CLI11.h
        platform/parsers/help_formatter.h
        # Source Files
        platform/parsers/CLI11.cpp
        platform/parsers/help_formatter.cpp
        )

set(UI_FILES ui/canvas.h
        ui/canvas.cpp
        ui/drawable.h
        ui/gui_drawer.h
        ui/gui_drawer-inl.h
        ui/gui_drawer.cpp
        ui/imgui_impl_glfw.h
        ui/imgui_impl_glfw.cpp
        ui/imgui_impl_wgpu.h
        ui/imgui_impl_wgpu.cpp
        ui/ui_manager.h
        ui/ui_manager.cpp

        ui/plugins/contextual_menu.h
        ui/plugins/contextual_menu.cpp
        ui/plugins/data_dispatcher.h
        ui/plugins/drag_drop_source.h
        ui/plugins/drag_drop_target.h
        ui/plugins/plugin.h
        ui/plugins/pluginable.h

        ui/widgets/alignment.h
        ui/widgets/converter.h
        ui/widgets/converter.cpp
        ui/widgets/data_widget.h
        ui/widgets/panel.h
        ui/widgets/panel.cpp
        ui/widgets/panel_transformable.h
        ui/widgets/panel_transformable.cpp
        ui/widgets/panel_window_settings.h
        ui/widgets/widget.h
        ui/widgets/widget.cpp
        ui/widgets/widget_container.h
        ui/widgets/widget_container.cpp

        ui/widgets/buttons/button.h
        ui/widgets/buttons/button_arrow.h
        ui/widgets/buttons/button_arrow.cpp
        ui/widgets/buttons/button_colored.h
        ui/widgets/buttons/button_colored.cpp
        ui/widgets/buttons/button_image.h
        ui/widgets/buttons/button_image.cpp
        ui/widgets/buttons/button_simple.h
        ui/widgets/buttons/button_simple.cpp
        ui/widgets/buttons/button_small.h
        ui/widgets/buttons/button_small.cpp

        ui/widgets/drags/drag_double.cpp
        ui/widgets/drags/drag_double.h
        ui/widgets/drags/drag_float.cpp
        ui/widgets/drags/drag_float.h
        ui/widgets/drags/drag_int.cpp
        ui/widgets/drags/drag_int.h
        ui/widgets/drags/drag_multiple_doubles.h
        ui/widgets/drags/drag_multiple_floats.h
        ui/widgets/drags/drag_multiple_ints.h
        ui/widgets/drags/drag_multiple_scalars.h
        ui/widgets/drags/drag_single_scalar.h

        ui/widgets/input_fields/input_double.cpp
        ui/widgets/input_fields/input_double.h
        ui/widgets/input_fields/input_float.cpp
        ui/widgets/input_fields/input_float.h
        ui/widgets/input_fields/input_int.cpp
        ui/widgets/input_fields/input_int.h
        ui/widgets/input_fields/input_multiple_doubles.h
        ui/widgets/input_fields/input_multiple_floats.h
        ui/widgets/input_fields/input_multiple_ints.h
        ui/widgets/input_fields/input_multiple_scalars.h
        ui/widgets/input_fields/input_text.cpp
        ui/widgets/input_fields/input_text.h

        ui/widgets/layout/columns.h
        ui/widgets/layout/dummy.cpp
        ui/widgets/layout/dummy.h
        ui/widgets/layout/group.cpp
        ui/widgets/layout/group.h
        ui/widgets/layout/group_collapsable.cpp
        ui/widgets/layout/group_collapsable.h
        ui/widgets/layout/new_line.cpp
        ui/widgets/layout/new_line.h
        ui/widgets/layout/spacing.cpp
        ui/widgets/layout/spacing.h
        ui/widgets/layout/tree_node.cpp
        ui/widgets/layout/tree_node.h

        ui/widgets/menu/menu_item.cpp
        ui/widgets/menu/menu_item.h
        ui/widgets/menu/menu_list.cpp
        ui/widgets/menu/menu_list.h

        ui/widgets/panel_transformables/panel_undecorated.cpp
        ui/widgets/panel_transformables/panel_undecorated.h
        ui/widgets/panel_transformables/panel_window.cpp
        ui/widgets/panel_transformables/panel_window.h

        ui/widgets/panels/panel_menu_bar.cpp
        ui/widgets/panels/panel_menu_bar.h

        ui/widgets/plot/plot.cpp
        ui/widgets/plot/plot.h
        ui/widgets/plot/plot_histogram.cpp
        ui/widgets/plot/plot_histogram.h
        ui/widgets/plot/plot_lines.cpp
        ui/widgets/plot/plot_lines.h

        ui/widgets/selection/check_box.cpp
        ui/widgets/selection/check_box.h
        ui/widgets/selection/color_edit.cpp
        ui/widgets/selection/color_edit.h
        ui/widgets/selection/color_picker.cpp
        ui/widgets/selection/color_picker.h
        ui/widgets/selection/combo_box.cpp
        ui/widgets/selection/combo_box.h
        ui/widgets/selection/radio_button.cpp
        ui/widgets/selection/radio_button.h
        ui/widgets/selection/radio_button_linker.cpp
        ui/widgets/selection/radio_button_linker.h

        ui/widgets/sliders/slider_double.cpp
        ui/widgets/sliders/slider_double.h
        ui/widgets/sliders/slider_float.cpp
        ui/widgets/sliders/slider_float.h
        ui/widgets/sliders/slider_int.cpp
        ui/widgets/sliders/slider_int.h
        ui/widgets/sliders/slider_multiple_doubles.h
        ui/widgets/sliders/slider_multiple_floats.h
        ui/widgets/sliders/slider_multiple_ints.h
        ui/widgets/sliders/slider_multiple_scalars.h
        ui/widgets/sliders/slider_single_scalar.h

        ui/widgets/texts/text.cpp
        ui/widgets/texts/text.h
        ui/widgets/texts/text_clickable.cpp
        ui/widgets/texts/text_clickable.h
        ui/widgets/texts/text_colored.cpp
        ui/widgets/texts/text_colored.h
        ui/widgets/texts/text_disabled.cpp
        ui/widgets/texts/text_disabled.h
        ui/widgets/texts/text_labelled.cpp
        ui/widgets/texts/text_labelled.h
        ui/widgets/texts/text_selectable.cpp
        ui/widgets/texts/text_selectable.h
        ui/widgets/texts/text_wrapped.cpp
        ui/widgets/texts/text_wrapped.h

        ui/widgets/visual/bullet.cpp
        ui/widgets/visual/bullet.h
        ui/widgets/visual/image.cpp
        ui/widgets/visual/image.h
        ui/widgets/visual/progress_bar.cpp
        ui/widgets/visual/progress_bar.h
        ui/widgets/visual/separator.cpp
        ui/widgets/visual/separator.h)

set(SCENE_FILES
        background.h
        layer.h
        update_flag.h
        update_flag.cpp
        update_flag_manager.h
        update_flag_manager.cpp
        vobject.h
        inspector_item.h
        inspector_item.cpp
        scene_forward.h
        scene_manager.h
        scene_manager.cpp
        scene.h
        scene.cpp
        entity.h
        entity.cpp
        component.h
        component.cpp
        components_manager.h
        components_manager.cpp
        transform.h
        transform.cpp
        camera.h
        camera.cpp
        script.h
        script.cpp
        behaviour.h
        behaviour-inl.h
        behaviour.cpp
        )

set(LUA_FILES
        lua/lua_binder.cpp
        lua/lua_binder.h
        lua/lua_component_binder.cpp
        lua/lua_component_binder.h
        lua/lua_entity_binder.cpp
        lua/lua_entity_binder.h
        lua/lua_global_binder.cpp
        lua/lua_global_binder.h
        lua/lua_math_binder.cpp
        lua/lua_math_binder.h
        lua/script_interpreter.cpp
        lua/script_interpreter.h)

set(LUA_SOURCE
        ../third_party/sol2/include
        /opt/homebrew/include/lua
        )

set(CONTROL_FILES controls/spherical.h
        controls/spherical.cpp
        controls/orbit_control.h
        controls/orbit_control.cpp
        controls/free_control.h
        controls/free_control.cpp)

set(MESH_FILES mesh/index_buffer_binding.h
        mesh/index_buffer_binding.cpp
        mesh/sub_mesh.h
        mesh/sub_mesh.cpp
        mesh/mesh.h
        mesh/mesh.cpp
        mesh/buffer_mesh.h
        mesh/buffer_mesh.cpp
        mesh/model_mesh.h
        mesh/model_mesh.cpp
        mesh/mesh_renderer.h
        mesh/mesh_renderer.cpp
        mesh/gpu_skinned_mesh_renderer.h
        mesh/gpu_skinned_mesh_renderer.cpp
        mesh/primitive_mesh.h
        mesh/primitive_mesh.cpp
        mesh/mesh_manager.h
        mesh/mesh_manager.cpp)

file(GLOB WIREFRAME_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/wireframe/*.cpp)

set(SHADER_FILES shader/glsl_compiler.h
        shader/glsl_compiler.cpp
        shader/shader_common.h
        shader/shader_data.h
        shader/shader_data.cpp
        shader/shader_module.h
        shader/shader_module.cpp
        shader/shader_source.h
        shader/shader_source.cpp
        shader/shader_variant.h
        shader/shader_variant.cpp
        shader/spirv_reflection.h
        shader/spirv_reflection.cpp
        shader/shader_manager.h
        shader/shader_manager.cpp)

set(MATERIAL_FILES material/material.h
        material/material.cpp
        material/base_material.h
        material/base_material.cpp
        material/unlit_material.h
        material/unlit_material.cpp
        material/pbr_base_material.h
        material/pbr_base_material.cpp
        material/pbr_material.h
        material/pbr_material.cpp
        material/pbr_specular_material.h
        material/pbr_specular_material.cpp
        material/blinn_phong_material.h
        material/blinn_phong_material.cpp)

set(LOADER_FILES assimp_parser.h
        assimp_parser.cpp)
set(LOADER_SOURCE ../third_party/assimp/include
        ../third_party/assimp/build_release/include)

set(LIGHT_FILE
        lighting/light.h
        lighting/light.cpp
        lighting/point_light.h
        lighting/point_light.cpp
        lighting/spot_light.h
        lighting/spot_light.cpp
        lighting/direct_light.h
        lighting/direct_light.cpp
        lighting/light_manager.h
        lighting/light_manager.cpp
        lighting/ambient_light.h
        lighting/ambient_light.cpp
        lighting/debug/cluster_debug_material.h
        lighting/debug/cluster_debug_material.cpp
        lighting/debug/sprite_debug.h
        lighting/debug/sprite_debug.cpp)

set(SHADOW_FILE
        shadow/shadow_manager.cpp
        shadow/shadow_manager.h
        shadow/shadow_subpass.cpp
        shadow/shadow_subpass.h)

set(PARTICLE_FILE
        particle/particle_material.h
        particle/particle_material.cpp
        particle/particle_renderer.h
        particle/particle_renderer.cpp
        particle/particle_manager.h
        particle/particle_manager.cpp)

set(PLUGINS_FILE
        plugins/force_close.h
        plugins/force_close.cpp)

set(ANDROID_FILES
        # Header Files
        platform/android/android_platform.h
        platform/android/android_window.h
        # Source Files
        platform/android/android_platform.cpp
        platform/android/android_window.cpp)

set(WINDOWS_FILES
        # Header Files
        platform/windows/windows_platform.h
        # Source Files
        platform/windows/windows_platform.cpp)

set(UNIX_FILES
        # Header Files
        platform/unix/unix_platform.h
        # Source Files
        platform/unix/unix_platform.cpp)

set(LINUX_D2D_FILES
        # Header Files
        platform/unix/unix_d2d_platform.h
        platform/unix/direct_window.h
        # Source Files
        platform/unix/unix_d2d_platform.cpp
        platform/unix/direct_window.cpp)

source_group("\\" FILES ${FRAMEWORK_FILES})
source_group("common\\" FILES ${COMMON_FILES})
source_group("platform\\" FILES ${PLATFORM_FILES})
source_group("platform\\" FILES ${GLFW_FILES})
source_group("platform\\parsers" FILES ${PARSER_FILES})
source_group("platform\\windows" FILES ${WINDOWS_FILES})
source_group("platform\\android" FILES ${ANDROID_FILES})
source_group("platform\\unix" FILES ${UNIX_FILES})
source_group("platform\\unix" FILES ${LINUX_D2D_FILES})
source_group("core\\" FILES ${CORE_FILES})
source_group("rendering\\" FILES ${RENDERING_FILES})
source_group("rendering\\subpasses" FILES ${RENDERING_SUBPASSES_FILES})
source_group("\\" FILES ${SCENE_RESOURCE_FILES})
source_group("stats\\" FILES ${STATS_FILES})

set(PROJECT_FILES
        ${DAWN_FILES}
        ${PLATFORM_FILES}
#        ${PARSER_FILES}
#        ${COMMON_FILES}
#        ${FRAMEWORK_FILES}
#        ${CORE_FILES}
#        ${RENDERING_FILES}
#        ${RENDERING_SUBPASSES_FILES}
#        ${SCENE_RESOURCE_FILES}
        ${STATS_FILES}
        ${UI_FILES}
#        ${SCENE_FILES}
#        ${CONTROL_FILES}
#        ${MESH_FILES}
#        ${WIREFRAME_FILES}
#        ${SHADER_FILES}
#        ${MATERIAL_FILES}
#        ${LOADER_FILES}
#        ${LIGHT_FILE}
#        ${SHADOW_FILE}
#        ${PARTICLE_FILE}
        ${PLUGINS_FILE}
#        ${LUA_FILES}
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
        ${CMAKE_CURRENT_SOURCE_DIR} ${LOADER_SOURCE} ${LUA_SOURCE}
        ../third_party/nlohmann/include
        ../third_party/dawn/cmake-release/gen/include
        ../third_party/dawn/include
        ../third_party/dawn)

target_compile_definitions(${PROJECT_NAME} PUBLIC DAWN_ENABLE_BACKEND_METAL DAWN_ENABLE_BACKEND_NULL TINT_BUILD_WGSL_READER)

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
        ctpl
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
