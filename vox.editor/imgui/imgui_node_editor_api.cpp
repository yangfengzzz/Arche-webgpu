//------------------------------------------------------------------------------
// LICENSE
//   This software is dual-licensed to the public domain and under the following
//   license: you are granted a perpetual, irrevocable license to copy, modify,
//   publish, and distribute this file as you see fit.
//
// CREDITS
//   Written by Michal Cichon
//------------------------------------------------------------------------------
#include <algorithm>

#include "vox.editor/imgui/imgui_node_editor_internal.h"

using namespace vox;
//------------------------------------------------------------------------------
static NodeEditor::Detail::EditorContext *s_Editor = nullptr;

//------------------------------------------------------------------------------
template <typename C, typename I, typename F>
static int BuildIdList(C &container, I *list, int listSize, F &&accept) {
    if (list != nullptr) {
        int count = 0;
        for (auto object : container) {
            if (listSize <= 0) break;

            if (accept(object)) {
                list[count] = I(object->ID().AsPointer());
                ++count;
                --listSize;
            }
        }

        return count;
    } else
        return static_cast<int>(std::count_if(container.begin(), container.end(), accept));
}

//------------------------------------------------------------------------------
NodeEditor::EditorContext *NodeEditor::CreateEditor(const Config *config) {
    return reinterpret_cast<NodeEditor::EditorContext *>(new NodeEditor::Detail::EditorContext(config));
}

void NodeEditor::DestroyEditor(EditorContext *ctx) {
    if (GetCurrentEditor() == ctx) SetCurrentEditor(nullptr);

    auto editor = reinterpret_cast<NodeEditor::Detail::EditorContext *>(ctx);

    delete editor;
}

void NodeEditor::SetCurrentEditor(EditorContext *ctx) {
    s_Editor = reinterpret_cast<NodeEditor::Detail::EditorContext *>(ctx);
}

NodeEditor::EditorContext *NodeEditor::GetCurrentEditor() {
    return reinterpret_cast<NodeEditor::EditorContext *>(s_Editor);
}

NodeEditor::Style &NodeEditor::GetStyle() { return s_Editor->GetStyle(); }

const char *NodeEditor::GetStyleColorName(StyleColor colorIndex) {
    return s_Editor->GetStyle().GetColorName(colorIndex);
}

void NodeEditor::PushStyleColor(StyleColor colorIndex, const ImVec4 &color) {
    s_Editor->GetStyle().PushColor(colorIndex, color);
}

void NodeEditor::PopStyleColor(int count) { s_Editor->GetStyle().PopColor(count); }

void NodeEditor::PushStyleVar(StyleVar varIndex, float value) { s_Editor->GetStyle().PushVar(varIndex, value); }

void NodeEditor::PushStyleVar(StyleVar varIndex, const ImVec2 &value) { s_Editor->GetStyle().PushVar(varIndex, value); }

void NodeEditor::PushStyleVar(StyleVar varIndex, const ImVec4 &value) { s_Editor->GetStyle().PushVar(varIndex, value); }

void NodeEditor::PopStyleVar(int count) { s_Editor->GetStyle().PopVar(count); }

void NodeEditor::Begin(const char *id, const ImVec2 &size) { s_Editor->Begin(id, size); }

void NodeEditor::End() { s_Editor->End(); }

void NodeEditor::BeginNode(NodeId id) { s_Editor->GetNodeBuilder().Begin(id); }

void NodeEditor::BeginPin(PinId id, PinKind kind) { s_Editor->GetNodeBuilder().BeginPin(id, kind); }

void NodeEditor::PinRect(const ImVec2 &a, const ImVec2 &b) { s_Editor->GetNodeBuilder().PinRect(a, b); }

void NodeEditor::PinPivotRect(const ImVec2 &a, const ImVec2 &b) { s_Editor->GetNodeBuilder().PinPivotRect(a, b); }

void NodeEditor::PinPivotSize(const ImVec2 &size) { s_Editor->GetNodeBuilder().PinPivotSize(size); }

void NodeEditor::PinPivotScale(const ImVec2 &scale) { s_Editor->GetNodeBuilder().PinPivotScale(scale); }

void NodeEditor::PinPivotAlignment(const ImVec2 &alignment) { s_Editor->GetNodeBuilder().PinPivotAlignment(alignment); }

void NodeEditor::EndPin() { s_Editor->GetNodeBuilder().EndPin(); }

void NodeEditor::Group(const ImVec2 &size) { s_Editor->GetNodeBuilder().Group(size); }

void NodeEditor::EndNode() { s_Editor->GetNodeBuilder().End(); }

bool NodeEditor::BeginGroupHint(NodeId nodeId) { return s_Editor->GetHintBuilder().Begin(nodeId); }

ImVec2 NodeEditor::GetGroupMin() { return s_Editor->GetHintBuilder().GetGroupMin(); }

ImVec2 NodeEditor::GetGroupMax() { return s_Editor->GetHintBuilder().GetGroupMax(); }

ImDrawList *NodeEditor::GetHintForegroundDrawList() { return s_Editor->GetHintBuilder().GetForegroundDrawList(); }

ImDrawList *NodeEditor::GetHintBackgroundDrawList() { return s_Editor->GetHintBuilder().GetBackgroundDrawList(); }

void NodeEditor::EndGroupHint() { s_Editor->GetHintBuilder().End(); }

ImDrawList *NodeEditor::GetNodeBackgroundDrawList(NodeId nodeId) {
    if (auto node = s_Editor->FindNode(nodeId))
        return s_Editor->GetNodeBuilder().GetUserBackgroundDrawList(node);
    else
        return nullptr;
}

bool NodeEditor::Link(LinkId id,
                      PinId startPinId,
                      PinId endPinId,
                      const ImVec4 &color /* = ImVec4(1, 1, 1, 1)*/,
                      float thickness /* = 1.0f*/) {
    return s_Editor->DoLink(id, startPinId, endPinId, ImColor(color), thickness);
}

void NodeEditor::Flow(LinkId linkId) {
    if (auto link = s_Editor->FindLink(linkId)) s_Editor->Flow(link);
}

bool NodeEditor::BeginCreate(const ImVec4 &color, float thickness) {
    auto &context = s_Editor->GetItemCreator();

    if (context.Begin()) {
        context.SetStyle(ImColor(color), thickness);
        return true;
    } else
        return false;
}

bool NodeEditor::QueryNewLink(PinId *startId, PinId *endId) {
    using Result = NodeEditor::Detail::CreateItemAction::Result;

    auto &context = s_Editor->GetItemCreator();

    return context.QueryLink(startId, endId) == Result::True;
}

bool NodeEditor::QueryNewLink(PinId *startId, PinId *endId, const ImVec4 &color, float thickness) {
    using Result = NodeEditor::Detail::CreateItemAction::Result;

    auto &context = s_Editor->GetItemCreator();

    auto result = context.QueryLink(startId, endId);
    if (result != Result::Indeterminate) context.SetStyle(ImColor(color), thickness);

    return result == Result::True;
}

bool NodeEditor::QueryNewNode(PinId *pinId) {
    using Result = NodeEditor::Detail::CreateItemAction::Result;

    auto &context = s_Editor->GetItemCreator();

    return context.QueryNode(pinId) == Result::True;
}

bool NodeEditor::QueryNewNode(PinId *pinId, const ImVec4 &color, float thickness) {
    using Result = NodeEditor::Detail::CreateItemAction::Result;

    auto &context = s_Editor->GetItemCreator();

    auto result = context.QueryNode(pinId);
    if (result != Result::Indeterminate) context.SetStyle(ImColor(color), thickness);

    return result == Result::True;
}

bool NodeEditor::AcceptNewItem() {
    using Result = NodeEditor::Detail::CreateItemAction::Result;

    auto &context = s_Editor->GetItemCreator();

    return context.AcceptItem() == Result::True;
}

bool NodeEditor::AcceptNewItem(const ImVec4 &color, float thickness) {
    using Result = NodeEditor::Detail::CreateItemAction::Result;

    auto &context = s_Editor->GetItemCreator();

    auto result = context.AcceptItem();
    if (result != Result::Indeterminate) context.SetStyle(ImColor(color), thickness);

    return result == Result::True;
}

void NodeEditor::RejectNewItem() {
    auto &context = s_Editor->GetItemCreator();

    context.RejectItem();
}

void NodeEditor::RejectNewItem(const ImVec4 &color, float thickness) {
    using Result = NodeEditor::Detail::CreateItemAction::Result;

    auto &context = s_Editor->GetItemCreator();

    if (context.RejectItem() != Result::Indeterminate) context.SetStyle(ImColor(color), thickness);
}

void NodeEditor::EndCreate() {
    auto &context = s_Editor->GetItemCreator();

    context.End();
}

bool NodeEditor::BeginDelete() {
    auto &context = s_Editor->GetItemDeleter();

    return context.Begin();
}

bool NodeEditor::QueryDeletedLink(LinkId *linkId, PinId *startId, PinId *endId) {
    auto &context = s_Editor->GetItemDeleter();

    return context.QueryLink(linkId, startId, endId);
}

bool NodeEditor::QueryDeletedNode(NodeId *nodeId) {
    auto &context = s_Editor->GetItemDeleter();

    return context.QueryNode(nodeId);
}

bool NodeEditor::AcceptDeletedItem() {
    auto &context = s_Editor->GetItemDeleter();

    return context.AcceptItem();
}

void NodeEditor::RejectDeletedItem() {
    auto &context = s_Editor->GetItemDeleter();

    context.RejectItem();
}

void NodeEditor::EndDelete() {
    auto &context = s_Editor->GetItemDeleter();

    context.End();
}

void NodeEditor::SetNodePosition(NodeId nodeId, const ImVec2 &position) { s_Editor->SetNodePosition(nodeId, position); }

ImVec2 NodeEditor::GetNodePosition(NodeId nodeId) { return s_Editor->GetNodePosition(nodeId); }

ImVec2 NodeEditor::GetNodeSize(NodeId nodeId) { return s_Editor->GetNodeSize(nodeId); }

void NodeEditor::CenterNodeOnScreen(NodeId nodeId) {
    if (auto node = s_Editor->FindNode(nodeId)) node->CenterOnScreenInNextFrame();
}

void NodeEditor::RestoreNodeState(NodeId nodeId) {
    if (auto node = s_Editor->FindNode(nodeId)) s_Editor->MarkNodeToRestoreState(node);
}

void NodeEditor::Suspend() { s_Editor->Suspend(); }

void NodeEditor::Resume() { s_Editor->Resume(); }

bool NodeEditor::IsSuspended() { return s_Editor->IsSuspended(); }

bool NodeEditor::IsActive() { return s_Editor->IsActive(); }

bool NodeEditor::HasSelectionChanged() { return s_Editor->HasSelectionChanged(); }

int NodeEditor::GetSelectedObjectCount() { return (int)s_Editor->GetSelectedObjects().size(); }

int NodeEditor::GetSelectedNodes(NodeId *nodes, int size) {
    return BuildIdList(s_Editor->GetSelectedObjects(), nodes, size,
                       [](auto object) { return object->AsNode() != nullptr; });
}

int NodeEditor::GetSelectedLinks(LinkId *links, int size) {
    return BuildIdList(s_Editor->GetSelectedObjects(), links, size,
                       [](auto object) { return object->AsLink() != nullptr; });
}

void NodeEditor::ClearSelection() { s_Editor->ClearSelection(); }

void NodeEditor::SelectNode(NodeId nodeId, bool append) {
    if (auto node = s_Editor->FindNode(nodeId)) {
        if (append)
            s_Editor->SelectObject(node);
        else
            s_Editor->SetSelectedObject(node);
    }
}

void NodeEditor::SelectLink(LinkId linkId, bool append) {
    if (auto link = s_Editor->FindLink(linkId)) {
        if (append)
            s_Editor->SelectObject(link);
        else
            s_Editor->SetSelectedObject(link);
    }
}

void NodeEditor::DeselectNode(NodeId nodeId) {
    if (auto node = s_Editor->FindNode(nodeId)) s_Editor->DeselectObject(node);
}

void NodeEditor::DeselectLink(LinkId linkId) {
    if (auto link = s_Editor->FindLink(linkId)) s_Editor->DeselectObject(link);
}

bool NodeEditor::DeleteNode(NodeId nodeId) {
    if (auto node = s_Editor->FindNode(nodeId))
        return s_Editor->GetItemDeleter().Add(node);
    else
        return false;
}

bool NodeEditor::DeleteLink(LinkId linkId) {
    if (auto link = s_Editor->FindLink(linkId))
        return s_Editor->GetItemDeleter().Add(link);
    else
        return false;
}

void NodeEditor::NavigateToContent(float duration) {
    s_Editor->NavigateTo(s_Editor->GetContentBounds(), true, duration);
}

void NodeEditor::NavigateToSelection(bool zoomIn, float duration) {
    s_Editor->NavigateTo(s_Editor->GetSelectionBounds(), zoomIn, duration);
}

bool NodeEditor::ShowNodeContextMenu(NodeId *nodeId) { return s_Editor->GetContextMenu().ShowNodeContextMenu(nodeId); }

bool NodeEditor::ShowPinContextMenu(PinId *pinId) { return s_Editor->GetContextMenu().ShowPinContextMenu(pinId); }

bool NodeEditor::ShowLinkContextMenu(LinkId *linkId) { return s_Editor->GetContextMenu().ShowLinkContextMenu(linkId); }

bool NodeEditor::ShowBackgroundContextMenu() { return s_Editor->GetContextMenu().ShowBackgroundContextMenu(); }

void NodeEditor::EnableShortcuts(bool enable) { s_Editor->EnableShortcuts(enable); }

bool NodeEditor::AreShortcutsEnabled() { return s_Editor->AreShortcutsEnabled(); }

bool NodeEditor::BeginShortcut() { return s_Editor->GetShortcut().Begin(); }

bool NodeEditor::AcceptCut() { return s_Editor->GetShortcut().AcceptCut(); }

bool NodeEditor::AcceptCopy() { return s_Editor->GetShortcut().AcceptCopy(); }

bool NodeEditor::AcceptPaste() { return s_Editor->GetShortcut().AcceptPaste(); }

bool NodeEditor::AcceptDuplicate() { return s_Editor->GetShortcut().AcceptDuplicate(); }

bool NodeEditor::AcceptCreateNode() { return s_Editor->GetShortcut().AcceptCreateNode(); }

int NodeEditor::GetActionContextSize() { return static_cast<int>(s_Editor->GetShortcut().m_Context.size()); }

int NodeEditor::GetActionContextNodes(NodeId *nodes, int size) {
    return BuildIdList(s_Editor->GetSelectedObjects(), nodes, size,
                       [](auto object) { return object->AsNode() != nullptr; });
}

int NodeEditor::GetActionContextLinks(LinkId *links, int size) {
    return BuildIdList(s_Editor->GetSelectedObjects(), links, size,
                       [](auto object) { return object->AsLink() != nullptr; });
}

void NodeEditor::EndShortcut() { return s_Editor->GetShortcut().End(); }

float NodeEditor::GetCurrentZoom() { return s_Editor->GetView().InvScale; }

NodeEditor::NodeId NodeEditor::GetDoubleClickedNode() { return s_Editor->GetDoubleClickedNode(); }

NodeEditor::PinId NodeEditor::GetDoubleClickedPin() { return s_Editor->GetDoubleClickedPin(); }

NodeEditor::LinkId NodeEditor::GetDoubleClickedLink() { return s_Editor->GetDoubleClickedLink(); }

bool NodeEditor::IsBackgroundClicked() { return s_Editor->IsBackgroundClicked(); }

bool NodeEditor::IsBackgroundDoubleClicked() { return s_Editor->IsBackgroundDoubleClicked(); }

bool NodeEditor::PinHadAnyLinks(PinId pinId) { return s_Editor->PinHadAnyLinks(pinId); }

ImVec2 NodeEditor::GetScreenSize() { return s_Editor->GetRect().GetSize(); }

ImVec2 NodeEditor::ScreenToCanvas(const ImVec2 &pos) { return s_Editor->ToCanvas(pos); }

ImVec2 NodeEditor::CanvasToScreen(const ImVec2 &pos) { return s_Editor->ToScreen(pos); }
