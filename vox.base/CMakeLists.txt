cmake_minimum_required(VERSION 3.12)

project(vox.base LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/containers/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/io/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/memory/*.cpp)

add_library(${PROJECT_NAME} STATIC ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC ../
        ${CMAKE_CURRENT_SOURCE_DIR})

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