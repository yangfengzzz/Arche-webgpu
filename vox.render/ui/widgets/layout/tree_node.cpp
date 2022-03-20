//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tree_node.h"

namespace vox {
namespace ui {
TreeNode::TreeNode(const std::string &p_name, bool p_arrowClickToOpen) :
DataWidget(name),
name(p_name),
_arrowClickToOpen(p_arrowClickToOpen) {
    _autoExecutePlugins = false;
}

void TreeNode::open() {
    _shouldOpen = true;
    _shouldClose = false;
}

void TreeNode::close() {
    _shouldClose = true;
    _shouldOpen = false;
}

bool TreeNode::isOpened() const {
    return _opened;
}

void TreeNode::_draw_Impl() {
    bool prevOpened = _opened;
    
    if (_shouldOpen) {
        ImGui::SetNextItemOpen(true);
        _shouldOpen = false;
    } else if (_shouldClose) {
        ImGui::SetNextItemOpen(false);
        _shouldClose = false;
    }
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
    if (_arrowClickToOpen) flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    if (selected) flags |= ImGuiTreeNodeFlags_Selected;
    if (leaf) flags |= ImGuiTreeNodeFlags_Leaf;
    
    bool opened = ImGui::TreeNodeEx((name + _widgetID).c_str(), flags);
    
    if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing()) {
        clickedEvent.invoke();
        
        if (ImGui::IsMouseDoubleClicked(0)) {
            doubleClickedEvent.invoke();
        }
    }
    
    if (opened) {
        if (!prevOpened)
            openedEvent.invoke();
        
        _opened = true;
        
        executePlugins(); // Manually execute plugins to make plugins considering the TreeNode and no childs
        
        drawWidgets();
        
        ImGui::TreePop();
    } else {
        if (prevOpened)
            closedEvent.invoke();
        
        _opened = false;
        
        executePlugins(); // Manually execute plugins to make plugins considering the TreeNode and no childs
    }
}


}
}
