//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "application.h"
#include "window.h"
#include "rendering/render_context.h"

namespace vox {
enum class ExitCode {
    Success = 0, /* App executed as expected */
    Help,        /* App should show help */
    Close,       /* App has been requested to close at initialization */
    FatalError   /* App encountered an unexpected error */
};

class Engine {
public:
    Engine() = default;
    
    virtual ~Engine() = default;
    
    /**
     * @brief Initialize the engine
     * @return An exit code representing the outcome of initialization
     */
    virtual ExitCode initialize();
    
    /**
     * @brief Handles the main loop of the engine
     * This should be overriden if a engine requires a specific main loop setup.
     * @return An exit code representing the outcome of the loop
     */
    ExitCode mainLoop();
    
    /**
     * @brief Runs the application for one frame
     */
    void update();
    
    /**
     * @brief Terminates the engine and the application
     * @param code Determines how the engine should exit
     */
    virtual void terminate(ExitCode code);
    
    /**
     * @brief Requests to close the engine at the next available point
     */
    virtual void close();
    
    virtual void resize(uint32_t win_width, uint32_t win_height,
                        uint32_t fb_width, uint32_t fb_height);
        
    virtual void inputEvent(const InputEvent &inputEvent);
    
    virtual std::unique_ptr<RenderContext> createRenderContext(wgpu::Device& device);
        
public:
    void setFocus(bool focused);
    
    void forceSimulationFPS(float fps);
    
    void disableInputProcessing();
    
public:
    Window &window() const;
    
    Application &app() const;
    
    Application &app();
    
    void setApp(std::unique_ptr<Application>&& active_app);
    
    bool startApp();
    
public:
    void setWindowProperties(const Window::OptionalProperties &properties);
    
    //    void on_post_draw(RenderView &view) const;
    
    static const uint32_t MIN_WINDOW_WIDTH;
    static const uint32_t MIN_WINDOW_HEIGHT;
    
protected:
    std::unique_ptr<Window> _window{nullptr};
    
    std::unique_ptr<Application> _activeApp{nullptr};
    
    std::unique_ptr<BackendBinding> _binding{nullptr};
    
    /**
     * @brief Handles the creation of the window
     *
     * @param properties Preferred window configuration
     */
    virtual void createWindow(const Window::Properties &properties) = 0;
    
    /* Source of truth for window state */
    Window::Properties _windowProperties;
    /* Delta time should be fixed with a fabricated value */
    bool _fixedSimulationFPS{false};
    /* A fabricated delta time */
    float _simulationFrameTime = 0.016f;
    /* App should continue processing input events */
    bool _processInputEvents{true};
    /* App is currently in focus at an operating system level */
    bool _focused;
    
private:
    Timer _timer;
};

}        // namespace vox
