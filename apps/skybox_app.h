//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef skybox_app_hpp
#define skybox_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class SkyboxApp : public ForwardApplication {
public:
    bool prepare(Engine &engine) override;

    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* skybox_app_hpp */
