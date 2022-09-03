cmake_minimum_required(VERSION 3.12)

project(test.math LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC ../
        ${CMAKE_SOURCE_DIR}/third_party/googletest/googlemock/include
        ${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)

# Link third party libraries
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/googletest/build/lib)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog vox.base vox.math
        libgmock.a libgmock_main.a libgtest.a libgtest_main.a)