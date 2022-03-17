//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef node_editor_hpp
#define node_editor_hpp

#include "imgui_node_editor.h"

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <iterator>
#include <functional>
#include <mutex>

namespace vox {
namespace editor {
#define MAKE_IMGUI_ID(x) ("##" + std::to_string(x)).c_str()
#define MAKE_IMGUI_LABEL(x, y) (y + std::string("##") + std::to_string(x)).c_str()

namespace ImGuiNodeEditor = NodeEditor;

enum class NodeEditorPinType : uint32_t {
    Output = 0,
    Input,
    PinTypeCount
};

struct NodeOutput {
    float value;
};

struct NodeInputParam {
    float x;
    float y;
    float z;

    float texX;
    float texY;

    float minX;
    float minY;
    float minZ;

    float maxX;
    float maxY;
    float maxZ;

    NodeInputParam();

    NodeInputParam(float *pos, float *texCoord, float *minPos, float *maxPos);
};

int generateUID();

void seUIDSeed(int seed);

class NodeEditorNode;

class NodeEditorPin;

//MARK: - NodeEditorConfig
struct NodeEditorConfig {
    std::string saveFile;
    std::function<void(void)> updateFunc;
    std::function<void(void)> makeNodeFunc;

    NodeEditorConfig(std::string saveFile = "NodeEditor.terr3d");
};

//MARK: - NodeEditorLink
class NodeEditorLink {
public:
    int id;
    ImGuiNodeEditor::LinkId _id;
    NodeEditorPin *from;
    NodeEditorPin *to;
    ImVec4 color = ImVec4(1, 1, 1, 1);
    float thickness = 1.0f;

    NodeEditorLink(int id = generateUID());
};

//MARK: - NodeEditorPin
class NodeEditorPin {
public:
    int id;
    ImGuiNodeEditor::PinId _id;
    NodeEditorLink *_link;
    NodeEditorPin *other;
    NodeEditorNode *parent;
    NodeEditorPinType type;
    std::mutex mutex;
    ImU32 color = ImColor(94, 95, 191);
    char userData[128];
    
    NodeEditorPin(NodeEditorPinType type = NodeEditorPinType::Input, int id = generateUID());

    ~NodeEditorPin();

    virtual void begin();

    virtual void end();

    bool validateLink(NodeEditorLink *link);

    void link(NodeEditorLink *link);

    bool isLinked();

    void unlink();

    void render();

    NodeOutput evaluate(NodeInputParam input);
};

//MARK: - NodeEditorNode
class NodeEditorNode {
public:
    ImVec2 nodePosition;
    bool reqNodePosLoad;
    int id;
    ImGuiNodeEditor::NodeId _id;
    std::vector<NodeEditorPin *> outputPins;
    std::vector<NodeEditorPin *> inputPins;
    char userData[128];
    std::string name;
    ImU32 headerColor = ImColor(59, 29, 209);
    std::mutex m;
    
    NodeEditorNode(int id = generateUID());

    ~NodeEditorNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin) = 0;

    virtual std::vector<NodeEditorPin *> getPins();

    virtual bool onLink(NodeEditorPin *pin, NodeEditorLink *link);

    virtual void onDelete();

    virtual void onRender() = 0;

    void render();

    void setup();

    void drawHeader(std::string text);
};

//MARK: - NodeEditor
class NodeEditor {
public:
    ImGuiNodeEditor::EditorContext *context;
    std::string name = "Node Editor";
    NodeEditorConfig config;
    std::unordered_map<uintptr_t, NodeEditorLink *> links;
    std::unordered_map<uintptr_t, NodeEditorNode *> nodes;
    std::unordered_map<uintptr_t, NodeEditorPin *> pins;
    NodeEditorNode *outputNode = nullptr;
    std::mutex mutex;
    
    NodeEditor(NodeEditorConfig config = NodeEditorConfig());

    ~NodeEditor();

    void render();

    void addNode(NodeEditorNode *node);

    void deleteNode(NodeEditorNode *node);

    void deleteLink(NodeEditorLink *link);

    void reset();

    void setOutputNode(NodeEditorNode *node);

private:
    ImGuiNodeEditor::NodeId lastNodeId;
};



}
}
#endif /* node_editor_hpp */
