//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef texture_node_hpp
#define texture_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class TextureNode : public NodeEditorNode {
public:
    TextureNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
    
    float scale;
    bool isDefault;
    bool autoTiled;
    bool inv;
    bool npScale;
    float numTiles;
    float posi[2];
    float rota;
    
private:
    void changeTexture();

    std::mutex mutex;
};


}
}
#endif /* texture_node_hpp */
