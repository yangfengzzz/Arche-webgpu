//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef noise_open_simplex2s_node_hpp
#define noise_open_simplex2s_node_hpp

#include "../node_editor.h"
#include <FastNoiseLite.h>

namespace vox {
namespace editor {
class NoiseOpenSimplex2SNode : public NodeEditorNode {
public:
    NoiseOpenSimplex2SNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
    
    int seed, octaves;
    float frequency, lacunarity, gain, weightedStrength, pingPongStrength, strength;
    int fractalType;
    std::unique_ptr<FastNoiseLite> noiseGen;
};


}
}
#endif /* noise_open_simplex2s_node_hpp */
