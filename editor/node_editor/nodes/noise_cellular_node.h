//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef noise_cellular_node_hpp
#define noise_cellular_node_hpp

#include "../node_editor.h"
#include <FastNoiseLite.h>

namespace vox {
namespace editor {
class NoiseCellularNode : public NodeEditorNode {
public:
    NoiseCellularNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
    
    int seed, octaves;
    float frequency, lacunarity, gain, weightedStrength, pingPongStrength, strength, cellularJitter;
    int fractalType, distanceFunc;
    std::unique_ptr<FastNoiseLite> noiseGen;
};


}
}
#endif /* noise_cellular_node_hpp */
