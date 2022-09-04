//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <unordered_map>

#include "vox.render/rendering/render_context.h"
#include "vox.render/singleton.h"
#include "vox.render/ui/canvas.h"

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
class UiManager : public Singleton<UiManager> {
public:
    static UiManager &GetSingleton();

    static UiManager *GetSingletonPtr();

    /**
     * Create the UI manager. Will setup ImGui internally\
     */
    UiManager(GLFWwindow *glfw_window, RenderContext *context, Style style = Style::IM_DARK_STYLE);

    /**
     * Destroy the UI manager. Will handle_ ImGui destruction internally
     */
    ~UiManager();

    /**
     * Apply a new style to the UI elements
     */
    static void ApplyStyle(Style style);

    /**
     * Load a font (Returns true on success)
     */
    bool LoadFont(const std::string &id, const std::string &path, float font_size);

    /**
     * Unload a font (Returns true on success)
     */
    bool UnloadFont(const std::string &id);

    /**
     * Set the given font as the current one (Returns true on success)
     */
    bool UseFont(const std::string &id);

    /**
     * Use the default font (ImGui default font)
     */
    void UseDefaultFont();

    /**
     * Allow the user to enable/disable .ini generation to save his editor layout
     */
    void EnableEditorLayoutSave(bool value);

    /**
     *  Return true if the editor layout save system is on
     */
    [[nodiscard]] static bool IsEditorLayoutSaveEnabled();

    /**
     * Defines a filename for the editor layout save file
     */
    void SetEditorLayoutSaveFilename(const std::string &filename);

    /**
     * Defines a frequency (in seconds) for the auto saving system of the editor layout
     * @param frequency frequency
     */
    static void SetEditorLayoutAutosaveFrequency(float frequency);

    /**
     * Returns the current frequency (in seconds) for the auto saving system of the editor layout
     */
    static float EditorLayoutAutosaveFrequency(float frequency);

    /**
     * Enable the docking system
     * @param value value
     */
    void EnableDocking(bool value);

    /**
     * Reset the UI layout to the given configuration file
     * @param config config
     */
    static void ResetLayout(const std::string &config);

    /**
     * Return true if the docking system is enabled
     */
    [[nodiscard]] bool IsDockingEnabled() const;

    /**
     * Defines the canvas to use
     */
    void SetCanvas(Canvas &canvas);

    /**
     * Stop considering the current canvas (if any)
     */
    void RemoveCanvas();

    /**
     * @brief Draws the Gui
     * @param passEncoder Command buffer to register draw-commands
     */
    void Render(wgpu::RenderPassEncoder &passEncoder);

private:
    void PushCurrentFont();

    void PopCurrentFont();

private:
    bool docking_state_{};
    Canvas *current_canvas_{nullptr};
    std::unordered_map<std::string, ImFont *> fonts_;
    std::string layout_save_filename_ = "imgui.ini";
};

}  // namespace ui
template <>
inline ui::UiManager *Singleton<ui::UiManager>::ms_singleton{nullptr};
}  // namespace vox
