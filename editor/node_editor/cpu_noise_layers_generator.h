//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpu_noise_layers_generator_hpp
#define cpu_noise_layers_generator_hpp

#include <string>
#include "generator_mask.h"

namespace vox {
namespace editor {
class CPUNoiseLayersGenerator {
public:
    void showSetting(int id);

    void update();

    float evaluateAt(float x, float y, float z);

    bool windowStat = false;
    bool uiActive = false;
    bool enabled = true;
    double time = 0;
    std::string uid;
    std::string name;
//    LayeredNoiseManager *noiseManager;
    GeneratorMaskManager *maskManager;
};


}
}

#endif /* cpu_noise_layers_generator_hpp */
