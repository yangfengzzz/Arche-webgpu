//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef math_function_node_hpp
#define math_function_node_hpp

#include "../node_editor.h"

namespace mu {
    class Parser;
}

namespace vox {
namespace editor {
class MathFunctionNode : public NodeEditorNode {
public:
    MathFunctionNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
    
    char inputExpression[1024 * 4];
    mu::Parser *parser;
    double x, y, z;
    float factor;
    bool compiled;
    int mathInputWidth;
    std::vector <std::pair<std::string, double>> vars;
    char varname[4096];
    int tid = generateUID(); // temp
};


}
}
#endif /* math_function_node_hpp */
