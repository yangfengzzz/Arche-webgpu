//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gpu_noise_layer_generator_hpp
#define gpu_noise_layer_generator_hpp

#include <string>
#include <vector>

namespace vox {
namespace editor {
#pragma pack(push, 1)
struct GPUNoiseLayer {
    float octaves = 3.0f;
    float fractal = 0.0f;
    float frequency = 1.0f;
    float lacunarity = 2.0f;
    float gain = 0.5f;
    float weightedStrength = 0.2f;
    float pingPongStrength = 0.2f;
    float strength = 1.0f;
    float domainWrapDepth = 0.0f;
    float offsetX = 0.0f, offsetY = 0.0f, offsetZ = 0.0f, offsetW = 0.0f;

    float valueX, valueY, valueZ, valueW; // Ony for internal use on GPU side
};
#pragma pack(pop)

class GPUNoiseLayerGenerator {
public:
    GPUNoiseLayerGenerator();
    
    virtual void showSetting(int i);

    void update();
    
    bool windowStat = false;
    bool uiActive = false;
    bool enabled = true;
    double time = 0;
    int localSize = 256;
    int vc = 0;
    std::string uid = "";
    std::string name = "";
    std::vector<GPUNoiseLayer> noiseLayers;
};

}
}
#endif /* gpu_noise_layer_generator_hpp */
