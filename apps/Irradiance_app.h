//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef Irradiance_app_hpp
#define Irradiance_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"
#include "texture/sampled_texturecube.h"

namespace vox {
class IrradianceApp : public ForwardApplication {
public:
    void loadScene() override;
    
private:
    SampledTextureCubePtr _cubeMap{nullptr};
};
}

#endif /* Irradiance_app_hpp */
