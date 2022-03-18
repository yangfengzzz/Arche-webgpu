//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpu_node_editor_hpp
#define cpu_node_editor_hpp

#include "node_editor.h"

namespace vox {
namespace editor {
#define DUMMY_NODE_COLOR          20, 31, 128
#define MATH_NODE_COLOR           11, 99, 20
#define VALUE_NODE_COLOR          11, 99, 90
#define OUTPUT_NODE_COLOR         79, 9, 32
#define OP_NODE_COLOR             39, 0, 87
#define IMAGE_NODE_COLOR          143, 95, 0
#define NOISE_NODE_COLOR          115, 90, 17

enum class CPUNodePinType : uint32_t {
    Float = 0,
    Boolean,
    Vec2,
    Vec3,
    MeshNodePinTypeCount
};

enum class CPUNodeType : uint32_t {
    Dummy = 0,
    Output,
    MeshCoordinates,
    MinMeshCoordinates,
    MaxMeshCoordinates,
    TextureCoordinates,
    TimeBasedSeed,
    RandomNumber,
    Duplicate,
    MinVal,
    Add,
    Sub,
    Mul,
    Div,
    Sin,
    Square,
    Cos,
    Tan,
    Abs,
    Blend,
    Curve,
    NoiseOpenSimplex2,
    NoiseOpenSimplex2S,
    NoiseCellular,
    NoisePerlin,
    NoiseValueCubic,
    NoiseValue,
    MathFunction,
    Pixelate,
    Texture,
    RectangleMask,
    Visualizer,
    Hill,
    Clamp,
    MeshNodeTypeCount
};

#define MeshNodeEditor CPUNodeEditorE
#define MeshNodeType CPUNodeType

class CPUNodeEditor {
public:
    CPUNodeEditor();

    void showSetting(int id);

    void update();

    float evaluateAt(NodeInputParam param);

    bool windowStat = false;
    bool uiActive = false;
    bool enabled = true;
    double time = 0;
    std::string uid;
    std::string name;
    std::unique_ptr<NodeEditor> editor{nullptr};

    std::mutex m;
};

}
}
#endif /* cpu_node_editor_hpp */
