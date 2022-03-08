//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef omni_shadowMap_app_hpp
#define omni_shadowMap_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class OminiShadowMapApp : public ForwardApplication {
public:
    void loadScene() override;
};
}

#endif /* omni_shadowMap_app_hpp */
