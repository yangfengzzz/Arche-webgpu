//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gpu_noise_layer_generator_hpp
#define gpu_noise_layer_generator_hpp

namespace vox {
namespace editor {
class GPUNoiseLayerGenerator {
public:
    virtual void showSetting(int i);

    void update();
};

}
}
#endif /* gpu_noise_layer_generator_hpp */
