//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "node_editor.h"
#include "imgui_shapes.h"

namespace vox {
namespace editor {
static int uidSeed = 1;

int generateUID() {
    return uidSeed++;
}

void seUIDSeed(int seed) {
    uidSeed = seed;
}

NodeEditorLink::NodeEditorLink(int id)
        : id(id) {
    _id = id;
}

void NodeEditorPin::begin() {
    ImGuiNodeEditor::PinKind pinKind =
    type == NodeEditorPinType::Input ? ImGuiNodeEditor::PinKind::Input : ImGuiNodeEditor::PinKind::Output;
    
    ImGuiNodeEditor::BeginPin(id, pinKind);
}

void NodeEditorPin::end() {
    ImGuiNodeEditor::EndPin();
}

bool NodeEditorPin::validateLink(NodeEditorLink *lnk) {
    if (_link) {
        return false;
    }

    if (lnk->from->parent->id == lnk->to->parent->id) {
        return false;
    }

    if (lnk->from->type == lnk->to->type) {
        return false;
    }

    return parent->onLink(this, lnk);
}

void NodeEditorPin::link(NodeEditorLink *lnk) {
    _link = lnk;

    if (_link->from->id == id) {
        other = _link->to;
    } else {
        other = _link->from;
    }
}

bool NodeEditorPin::isLinked() {
    return _link;
}

NodeEditorPin::NodeEditorPin(NodeEditorPinType type, int id)
        : type(type), id(id), _link(nullptr) {
    _id = id;
}

void NodeEditorPin::render() {
    if (type == NodeEditorPinType::Output) {
        ImGui::SameLine();
    }

    begin();

    if (isLinked()) {
        ImGui::DrawFilledCircle(10, color);
    } else {
        ImGui::DrawCircle(10, color);
    }

    ImGui::Dummy(ImVec2(20, 20));
    end();

    if (type == NodeEditorPinType::Input) {
        ImGui::SameLine();
    }
}

NodeEditorPin::~NodeEditorPin() {
}

void NodeEditorPin::unlink() {
    _link = nullptr;
}

NodeOutput NodeEditorPin::evaluate(NodeInputParam input) {
    mutex.lock();
    NodeOutput o = parent->evaluate(input, this);
    mutex.unlock();
    return o;
}

std::vector<NodeEditorPin *> NodeEditorNode::getPins() {
    std::vector<NodeEditorPin *> pins;
    pins.insert(
            pins.end(),
            std::make_move_iterator(inputPins.begin()),
            std::make_move_iterator(inputPins.end())
    );
    pins.insert(
            pins.end(),
            std::make_move_iterator(outputPins.begin()),
            std::make_move_iterator(outputPins.end())
    );
    return pins;
}

bool NodeEditorNode::onLink(NodeEditorPin *pin, NodeEditorLink *link) {
    return true;
}

void NodeEditorNode::onDelete() {
}

void NodeEditorNode::render() {
    ImGuiNodeEditor::BeginNode(id);
    onRender();
    ImGuiNodeEditor::EndNode();
}

NodeEditorNode::NodeEditorNode(int id)
        : id(id) {
    _id = id;
    nodePosition = ImVec2(0, 0);
}

NodeEditorNode::~NodeEditorNode() {
}

void NodeEditorNode::drawHeader(std::string text) {
    ImVec2 pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(pos.x - ImGuiNodeEditor::GetStyle().NodePadding.x, pos.y - ImGuiNodeEditor::GetStyle().NodePadding.y));
    ImVec2 start = ImGuiNodeEditor::GetNodeSize(_id);
    ImGui::DrawFilledRect(ImVec2(start.x, 60), headerColor, 13);
    ImGui::SetCursorPos(ImVec2(pos.x + ImGuiNodeEditor::GetStyle().NodePadding.x, pos.y + ImGuiNodeEditor::GetStyle().NodePadding.x));
//    ImGui::PushFont(GetUIFont("OpenSans-Bold"));
//    ImGui::Text(text.c_str());
//    ImGui::PopFont();
    ImGui::NewLine();
}

void NodeEditorNode::setup() {
    for (NodeEditorPin *pin: inputPins) {
        pin->parent = this;
    }

    for (NodeEditorPin *pin: outputPins) {
        pin->parent = this;
    }
}

void NodeEditor::render() {
    bool windowFocused = ImGui::IsWindowFocused();
    ImGuiNodeEditor::SetCurrentEditor(context);
    ImGuiNodeEditor::Begin(name.c_str());
    ImGuiNodeEditor::EnableShortcuts(windowFocused);

    for (auto &it: nodes) {
        it.second->render();
    }

    outputNode->render();

    for (auto &it: links) {
        ImGuiNodeEditor::Link(it.second->id, it.second->from->id, it.second->to->id, it.second->color, it.second->thickness);
    }

    if (ImGuiNodeEditor::BeginCreate()) {
        ImGuiNodeEditor::PinId iPid, oPid;

        if (ImGuiNodeEditor::QueryNewLink(&iPid, &oPid)) {
            if (iPid && oPid) {
                if (ImGuiNodeEditor::AcceptNewItem()) {
                    NodeEditorLink *link = new NodeEditorLink();
                    link->from = pins[iPid.Get()];
                    link->to = pins[oPid.Get()];
                    bool lnkFrm = link->from->validateLink(link);
                    bool lnkTo = link->to->validateLink(link);

                    if (lnkFrm && lnkTo) {
                        link->from->link(link);
                        link->to->link(link);
                        links[link->_id.Get()] = link;
                        ImGuiNodeEditor::Link(link->id, link->from->id, link->to->id);
                    }
                }
            } else if (iPid) {
                NodeEditorPin *pin = pins[iPid.Get()];

                if (pin->isLinked()) {
                    deleteLink(pin->_link);
                }
            } else if (oPid) {
                NodeEditorPin *pin = pins[oPid.Get()];

                if (pin->isLinked()) {
                    deleteLink(pin->_link);
                }
            }
        }

        ImGuiNodeEditor::PinId nPid;

        if (config.makeNodeFunc && ImGuiNodeEditor::QueryNewNode(&nPid)) {
            if (ImGuiNodeEditor::AcceptNewItem()) {
                NodeEditorPin *pin = pins[nPid.Get()];

                if (pin->isLinked()) {
                    deleteLink(pin->_link);
                } else {
                    config.makeNodeFunc();
                }
            }
        }
    }

    ImGuiNodeEditor::EndCreate();

    if (ImGuiNodeEditor::BeginDelete()) {
        ImGuiNodeEditor::NodeId nId;

        if (ImGuiNodeEditor::QueryDeletedNode(&nId)) {
            if (nId && ImGuiNodeEditor::AcceptDeletedItem()) {
                if (!(outputNode && nId == outputNode->_id)) {
                    NodeEditorNode *node = nodes[nId.Get()];
                    deleteNode(node);
                }
            }
        } else {
            ImGuiNodeEditor::LinkId lId;

            if (ImGuiNodeEditor::QueryDeletedLink(&lId)) {
                if (lId && ImGuiNodeEditor::AcceptDeletedItem()) {
                    NodeEditorLink *link = links[lId.Get()];
                    deleteLink(link);
                }
            }
        }
    }

    ImGuiNodeEditor::EndDelete();
    ImGuiNodeEditor::NodeId sNode;
    ImGuiNodeEditor::GetSelectedNodes(&sNode, 1);

    if (ImGui::IsWindowFocused()) {
//        GLFWwindow *wind = Application::Get()->GetWindow()->GetNativeWindow();
//
//        if (glfwGetKey(wind, GLFW_KEY_RIGHT_SHIFT) || glfwGetKey(wind, GLFW_KEY_LEFT_SHIFT)) {
//            if (glfwGetKey(wind, GLFW_KEY_D) && sNode) {
//                if (!(sNode.Get() == outputNode->_id.Get())) {
//                    NodeEditorNode *node = (config.insNodeFunc(nodes[sNode.Get()]->Save()));
//                    ImVec2 prePos = ImGuiNodeEditor::GetNodePosition(sNode);
//                    ImGuiNodeEditor::SetNodePosition(node->_id, ImVec2(prePos.x + 10, prePos.y + 10));
//                    ImGuiNodeEditor::DeselectNode(sNode);
//                    AddNode(node);
//                }
//            }
//        }
    }

    for (auto &it: nodes) {
        if (it.second->reqNodePosLoad) {
            ImGuiNodeEditor::SetNodePosition(it.second->_id, it.second->nodePosition);
            it.second->reqNodePosLoad = false;
        } else {
            it.second->nodePosition = ImGuiNodeEditor::GetNodePosition(it.second->_id);
        }
    }

    if (config.updateFunc) {
        config.updateFunc();
    }

    ImGuiNodeEditor::End();
    ImGuiNodeEditor::SetCurrentEditor(nullptr);
}

void NodeEditor::deleteLink(NodeEditorLink *link) {
    link->from->_link = nullptr;
    link->to->_link = nullptr;
    links.erase(links.find(link->_id.Get()));
    delete link;
}

void NodeEditor::addNode(NodeEditorNode *node) {
    nodes[node->_id.Get()] = node;

    for (auto &it: node->getPins()) {
        pins[it->_id.Get()] = it;
    }

    node->setup();

    if (nodes.size() > 2 && nodes.find(lastNodeId.Get()) != nodes.end() && !node->reqNodePosLoad) {
        NodeEditorNode *lastNode = nodes[lastNodeId.Get()];
        node->nodePosition = lastNode->nodePosition;
        node->reqNodePosLoad = true;
    }

    lastNodeId = node->_id;
}

NodeEditor::NodeEditor(NodeEditorConfig aconfig) {
    config = aconfig;
    ImGuiNodeEditor::Config iconfig;
    iconfig.SettingsFile = config.saveFile.c_str();
    context = ImGuiNodeEditor::CreateEditor(&iconfig);
}

void NodeEditor::deleteNode(NodeEditorNode *node) {
    mutex.lock();

    if (nodes.find(node->_id.Get()) != nodes.end()) {
#ifdef TERR3D_WIN32
        using namespace std::chrono_literals; // This Line is temporary
        std::this_thread::sleep_for(500ms); // This Line is temporary
#endif
        node->onDelete();
        std::vector<NodeEditorPin *> mPins = node->getPins();

        for (NodeEditorPin *pin: mPins) {
            if (pin->isLinked()) {
                links.erase(links.find(pin->_link->_id.Get()));
                pin->other->unlink();
                delete pin->_link;
            }

            pins.erase(pins.find(pin->_id.Get()));
            delete pin;
        }

        nodes.erase(nodes.find(node->_id.Get()));
        delete node;
    }

    mutex.unlock();
}

NodeEditor::~NodeEditor() {
    ImGuiNodeEditor::DestroyEditor(context);
}

void NodeEditor::reset() {
    // Clear Up Node Editor
    for (auto &it: nodes) {
        deleteNode(it.second);
    }

    NodeEditorNode *node = outputNode;

    for (auto &it: node->getPins()) {
        pins[it->_id.Get()] = it;
    }

    node->setup();
}

void NodeEditor::setOutputNode(NodeEditorNode *node) {
    outputNode = node;

    for (auto &it: node->getPins()) {
        pins[it->_id.Get()] = it;
    }

    node->setup();
}


NodeEditorConfig::NodeEditorConfig(std::string saveFile)
        : saveFile(saveFile) {
}

NodeInputParam::NodeInputParam() {
}

NodeInputParam::NodeInputParam(float *pos, float *texCoord, float *minPos, float *maxPos) {
    x = pos[0];
    y = pos[1];
    z = pos[2];
    texX = texCoord[0];
    texY = texCoord[1];
    minX = minPos[0];
    minY = minPos[1];
    minZ = minPos[2];
    maxX = maxPos[0];
    maxY = maxPos[1];
    maxZ = maxPos[2];
}



}
}
