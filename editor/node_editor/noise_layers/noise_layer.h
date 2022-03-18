//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef noise_layer_hpp
#define noise_layer_hpp

#include <string>
#include <FastNoiseLite.h>

namespace vox {
namespace editor {
struct NoiseLayerInput {
    float x;
    float y;
    float z;
};

class NoiseLayer {
public:
    NoiseLayer();

    float evaluate(NoiseLayerInput input);

    void render(int index);

    std::string name;
    const char *noiseTypeStr, *distFuncStr, *fractalTypeStr;
    int seed, octaves;
    float frequency, lacunarity, gain, weightedStrength, pingPongStrength, strength, cellularJitter;
    int fractalType, distanceFunc, noiseType;
    std::unique_ptr<FastNoiseLite> noiseGen;
    float offset[3];
    bool enabled;
};

}
}

#endif /* noise_layer_hpp */
