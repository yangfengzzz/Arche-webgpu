//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef ui_manager_hpp
#define ui_manager_hpp

#include <string>
#include <unordered_map>
#include "ui/widgets/canvas.h"

struct GLFWwindow;

namespace vox {
namespace ui {
/**
 * Some styles that you can use to modify the UI looks
 */
enum class Style {
    IM_CLASSIC_STYLE,
    IM_DARK_STYLE,
    IM_LIGHT_STYLE,
    // https://www.unknowncheats.me/forum/direct3d/189635-imgui-style-settings.html
    DUNE_DARK,
    ALTERNATIVE_DARK
};

/**
 * Handle the creation and drawing of the UI
 */
class UIManager {
public:
    /**
     * Create the UI manager. Will setup ImGui internally\
     * @param p_glfwWindow p_glfwWindow
     * @param p_style p_style
     */
    UIManager(GLFWwindow *p_glfwWindow, Style p_style = Style::IM_DARK_STYLE);
    
    /**
     * Destroy the UI manager. Will handle ImGui destruction internally
     */
    ~UIManager();
    
    /**
     * Apply a new style to the UI elements
     * @param p_style p_style
     */
    void applyStyle(Style p_style);
    
    /**
     * Load a font (Returns true on success)
     * @param p_id p_id
     * @param p_path p_path
     * @param p_fontSize p_fontSize
     */
    bool loadFont(const std::string &p_id, const std::string &p_path, float p_fontSize);
    
    /**
     * Unload a font (Returns true on success)
     * @param p_id p_id
     */
    bool unloadFont(const std::string &p_id);
    
    /**
     * Set the given font as the current one (Returns true on success)
     */
    bool useFont(const std::string &p_id);
    
    /**
     * Use the default font (ImGui default font)
     */
    void useDefaultFont();
    
    /**
     * Allow the user to enable/disable .ini generation to save his editor layout
     * @param p_value p_value
     */
    void enableEditorLayoutSave(bool p_value);
    
    /**
     *  Return true if the editor layout save system is on
     */
    bool isEditorLayoutSaveEnabled() const;
    
    /**
     * Defines a filename for the editor layout save file
     */
    void setEditorLayoutSaveFilename(const std::string &p_filename);
    
    /**
     * Defines a frequency (in seconds) for the auto saving system of the editor layout
     * @param p_frequency p_frequency
     */
    void setEditorLayoutAutosaveFrequency(float p_frequency);
    
    /**
     * Returns the current frequency (in seconds) for the auto saving system of the editor layout
     */
    float editorLayoutAutosaveFrequency(float p_frequeny);
    
    /**
     * Enable the docking system
     * @param p_value p_value
     */
    void enableDocking(bool p_value);
    
    /**
     * Reset the UI layout to the given configuration file
     * @param p_config p_config
     */
    void resetLayout(const std::string &p_config) const;
    
    /**
     * Return true if the docking system is enabled
     */
    bool isDockingEnabled() const;
    
    /**
     * Defines the canvas to use
     * @param p_canvas p_canvas
     */
    void setCanvas(Canvas &p_canvas);
    
    /**
     * Stop considering the current canvas (if any)
     */
    void removeCanvas();
    
    /**
     * Render ImGui current frane
     * @note Should be called once per frame
     */
    void render();
    
private:
    void pushCurrentFont();
    
    void popCurrentFont();
    
private:
    bool _dockingState;
    Canvas *_currentCanvas{nullptr};
    std::unordered_map<std::string, ImFont *> _fonts;
    std::string _layoutSaveFilename = "imgui.ini";
};


}
}
#endif /* ui_manager_hpp */
