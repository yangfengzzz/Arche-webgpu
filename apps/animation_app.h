//
//  animation_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/21.
//

#ifndef animation_app_hpp
#define animation_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class AnimationApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* animation_app_hpp */
