//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef layered_noise_manager_hpp
#define layered_noise_manager_hpp

#include "noise_layer.h"
#include <vector>
#include <mutex>

namespace vox {
namespace editor {
class LayeredNoiseManager {
public:
    LayeredNoiseManager();

    void updateLayers();

    void render();

    float evaluate(float x, float y, float z);

    float offset[3];
    float strength;
    bool absv; // Temporary
    bool sq;  // Temporary
    std::vector<std::unique_ptr<NoiseLayer>> noiseLayers, toAdd;
    std::vector<int> toDelete;

    std::mutex mutex;
};

}
}
#endif /* layered_noise_manager_hpp */
