//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ui_manager.h"
#include <GLFW/glfw3.h>

namespace vox {
namespace ui {
UIManager::UIManager(GLFWwindow *p_glfwWindow, Style p_style) {
    ImGui::CreateContext();
    
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true; /* Disable moving windows by dragging another thing than the title bar */
    enableDocking(false);
    
    applyStyle(p_style);
    
    //    ImGui_ImplGlfw_InitForOpenGL(p_glfwWindow, true);
    //    ImGui_ImplOpenGL3_Init(p_glslVersion.c_str());
}

UIManager::~UIManager() {
    //    ImGui_ImplOpenGL3_Shutdown();
    //    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::applyStyle(Style p_style) {
    ImGuiStyle *style = &ImGui::GetStyle();
    
    switch (p_style) {
        case Style::IM_CLASSIC_STYLE:
            ImGui::StyleColorsClassic();
            break;
        case Style::IM_DARK_STYLE:
            ImGui::StyleColorsDark();
            break;
        case Style::IM_LIGHT_STYLE:
            ImGui::StyleColorsLight();
            break;
        default:
            break;
    }
    
    if (p_style == Style::DUNE_DARK) {
        style->WindowPadding = ImVec2(15, 15);
        style->WindowRounding = 5.0f;
        style->FramePadding = ImVec2(5, 5);
        style->FrameRounding = 4.0f;
        style->ItemSpacing = ImVec2(12, 8);
        style->ItemInnerSpacing = ImVec2(8, 6);
        style->IndentSpacing = 25.0f;
        style->ScrollbarSize = 15.0f;
        style->ScrollbarRounding = 9.0f;
        style->GrabMinSize = 5.0f;
        style->GrabRounding = 3.0f;
        
        style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
        style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 0.88f);
        style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
        style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.3f, 0.3f, 0.3f, 0.75f);
        style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
        style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
        
        style->Colors[ImGuiCol_Tab] = style->Colors[ImGuiCol_TabUnfocused];
    } else if (p_style == Style::ALTERNATIVE_DARK) {
        style->WindowPadding = ImVec2(15, 15);
        style->WindowRounding = 0.0f;
        style->FramePadding = ImVec2(5, 5);
        style->FrameRounding = 0.0f;
        style->ItemSpacing = ImVec2(12, 8);
        style->ItemInnerSpacing = ImVec2(8, 6);
        style->IndentSpacing = 25.0f;
        style->ScrollbarSize = 15.0f;
        style->ScrollbarRounding = 0.0f;
        style->GrabMinSize = 5.0f;
        style->GrabRounding = 0.0f;
        style->TabRounding = 0.0f;
        style->ChildRounding = 0.0f;
        style->PopupRounding = 0.0f;
        
        style->WindowBorderSize = 1.0f;
        style->FrameBorderSize = 0.0f;
        style->PopupBorderSize = 1.0f;
        
        ImVec4 *colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.27f, 0.27f, 0.29f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.32f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.42f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.53f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.44f, 0.44f, 0.47f, 0.59f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.22f, 0.39f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.91f, 0.62f, 0.00f, 0.78f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.91f, 0.62f, 0.00f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }
}

bool UIManager::loadFont(const std::string &p_id, const std::string &p_path, float p_fontSize) {
    if (_fonts.find(p_id) == _fonts.end()) {
        auto &io = ImGui::GetIO();
        ImFont *fontInstance = io.Fonts->AddFontFromFileTTF(p_path.c_str(), p_fontSize);
        
        if (fontInstance) {
            _fonts[p_id] = fontInstance;
            return true;
        }
    }
    
    return false;
}

bool UIManager::unloadFont(const std::string &p_id) {
    if (_fonts.find(p_id) != _fonts.end()) {
        _fonts.erase(p_id);
        return true;
    }
    
    return false;
}

bool UIManager::useFont(const std::string &p_id) {
    auto foundFont = _fonts.find(p_id);
    
    if (foundFont != _fonts.end()) {
        ImGui::GetIO().FontDefault = foundFont->second;
        return true;
    }
    
    return false;
}

void UIManager::useDefaultFont() {
    ImGui::GetIO().FontDefault = nullptr;
}

void UIManager::enableEditorLayoutSave(bool p_value) {
    if (p_value)
        ImGui::GetIO().IniFilename = _layoutSaveFilename.c_str();
    else
        ImGui::GetIO().IniFilename = nullptr;
}

bool UIManager::isEditorLayoutSaveEnabled() const {
    return ImGui::GetIO().IniFilename != nullptr;
}

void UIManager::setEditorLayoutSaveFilename(const std::string &p_filename) {
    _layoutSaveFilename = p_filename;
    if (isEditorLayoutSaveEnabled())
        ImGui::GetIO().IniFilename = _layoutSaveFilename.c_str();
}

void UIManager::setEditorLayoutAutosaveFrequency(float p_frequency) {
    ImGui::GetIO().IniSavingRate = p_frequency;
}

float UIManager::editorLayoutAutosaveFrequency(float p_frequeny) {
    return ImGui::GetIO().IniSavingRate;
}

void UIManager::enableDocking(bool p_value) {
    _dockingState = p_value;
    
    if (p_value)
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    else
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
}

void UIManager::resetLayout(const std::string &p_config) const {
    ImGui::LoadIniSettingsFromDisk(p_config.c_str());
}

bool UIManager::isDockingEnabled() const {
    return _dockingState;
}

void UIManager::setCanvas(Canvas &p_canvas) {
    removeCanvas();
    
    _currentCanvas = &p_canvas;
}

void UIManager::removeCanvas() {
    _currentCanvas = nullptr;
}

void UIManager::render() {
    if (_currentCanvas) {
        _currentCanvas->draw();
        //        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void UIManager::pushCurrentFont() {
    
}

void UIManager::popCurrentFont() {
    
}

}
}
