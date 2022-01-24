//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "window.h"

struct GLFWwindow;

namespace vox {
class Engine;

/**
 * @brief An implementation of GLFW, inheriting the behaviour of the Window interface
 */
class GlfwWindow : public Window {
public:
    GlfwWindow(Engine *engine, const Window::Properties &properties);
    
    virtual ~GlfwWindow();
        
    bool shouldClose() override;
    
    void processEvents() override;
    
    void close() override;
    
    float dpiFactor() const override;
    
    float contentScaleFactor() const override;
    
private:
    void _createGUIContext(const Window::Properties &properties);
    
private:
    GLFWwindow *_handle = nullptr;
};
}        // namespace vox
