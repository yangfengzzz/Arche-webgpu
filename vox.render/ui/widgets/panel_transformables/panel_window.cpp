//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "panel_window.h"
#include "ui/widgets/converter.h"
#include "gui/imgui_internal.h"

namespace vox {
namespace ui {
PanelWindow::PanelWindow(const std::string &p_name, bool p_opened,
                         const PanelWindowSettings &p_floatingPanelSettings) :
name(p_name),
resizable(p_floatingPanelSettings.resizable),
closable(p_floatingPanelSettings.closable),
movable(p_floatingPanelSettings.movable),
scrollable(p_floatingPanelSettings.scrollable),
dockable(p_floatingPanelSettings.dockable),
hideBackground(p_floatingPanelSettings.hideBackground),
forceHorizontalScrollbar(p_floatingPanelSettings.forceHorizontalScrollbar),
forceVerticalScrollbar(p_floatingPanelSettings.forceVerticalScrollbar),
allowHorizontalScrollbar(p_floatingPanelSettings.allowHorizontalScrollbar),
bringToFrontOnFocus(p_floatingPanelSettings.bringToFrontOnFocus),
collapsable(p_floatingPanelSettings.collapsable),
allowInputs(p_floatingPanelSettings.allowInputs),
_opened(p_opened) {
    autoSize = p_floatingPanelSettings.autoSize;
}

void PanelWindow::open() {
    if (!_opened) {
        _opened = true;
        OpenEvent.invoke();
    }
}

void PanelWindow::close() {
    if (_opened) {
        _opened = false;
        CloseEvent.invoke();
    }
}

void PanelWindow::focus() {
    ImGui::SetWindowFocus((name + _panelID).c_str());
}

void PanelWindow::setOpened(bool p_value) {
    if (p_value != _opened) {
        _opened = p_value;
        
        if (_opened)
            OpenEvent.invoke();
        else
            CloseEvent.invoke();
    }
}

bool PanelWindow::isOpened() const {
    return _opened;
}

bool PanelWindow::isHovered() const {
    return _hovered;
}

bool PanelWindow::isFocused() const {
    return _focused;
}

bool PanelWindow::isAppearing() const {
    if (auto window = ImGui::FindWindowByName((name + panelID()).c_str()); window)
        return window->Appearing;
    else
        return false;
}

void PanelWindow::scrollToBottom() {
    _mustScrollToBottom = true;
}

void PanelWindow::scrollToTop() {
    _mustScrollToTop = true;
}

bool PanelWindow::isScrolledToBottom() const {
    return _scrolledToBottom;
}

bool PanelWindow::isScrolledToTop() const {
    return _scrolledToTop;
}

void PanelWindow::_draw_Impl() {
    if (_opened) {
        int windowFlags = ImGuiWindowFlags_None;
        
        if (!resizable) windowFlags |= ImGuiWindowFlags_NoResize;
        if (!movable) windowFlags |= ImGuiWindowFlags_NoMove;
        if (!dockable) windowFlags |= ImGuiWindowFlags_NoDocking;
        if (hideBackground) windowFlags |= ImGuiWindowFlags_NoBackground;
        if (forceHorizontalScrollbar) windowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
        if (forceVerticalScrollbar) windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
        if (allowHorizontalScrollbar) windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;
        if (!bringToFrontOnFocus) windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (!collapsable) windowFlags |= ImGuiWindowFlags_NoCollapse;
        if (!allowInputs) windowFlags |= ImGuiWindowFlags_NoInputs;
        if (!scrollable) windowFlags |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
        if (!titleBar) windowFlags |= ImGuiWindowFlags_NoTitleBar;
        
        ImVec2 minSizeConstraint = Converter::ToImVec2(minSize);
        ImVec2 maxSizeConstraint = Converter::ToImVec2(maxSize);
        
        /* Cancel constraint if x or y is <= 0.f */
        if (minSizeConstraint.x <= 0.f || minSizeConstraint.y <= 0.f)
            minSizeConstraint = {0.0f, 0.0f};
        
        if (maxSizeConstraint.x <= 0.f || maxSizeConstraint.y <= 0.f)
            maxSizeConstraint = {10000.f, 10000.f};
        
        ImGui::SetNextWindowSizeConstraints(minSizeConstraint, maxSizeConstraint);
        
        if (ImGui::Begin((name + _panelID).c_str(), closable ? &_opened : nullptr, windowFlags)) {
            _hovered = ImGui::IsWindowHovered();
            _focused = ImGui::IsWindowFocused();
            
            auto scrollY = ImGui::GetScrollY();
            
            _scrolledToBottom = scrollY == ImGui::GetScrollMaxY();
            _scrolledToTop = scrollY == 0.0f;
            
            if (!_opened)
                CloseEvent.invoke();
            
            update();
            
            if (_mustScrollToBottom) {
                ImGui::SetScrollY(ImGui::GetScrollMaxY());
                _mustScrollToBottom = false;
            }
            
            if (_mustScrollToTop) {
                ImGui::SetScrollY(0.0f);
                _mustScrollToTop = false;
            }
            
            drawWidgets();
        }
        
        ImGui::End();
    }
}


}
}
