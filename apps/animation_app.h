//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef animation_app_hpp
#define animation_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class AnimationApp : public ForwardApplication {
public:
    void loadScene() override;
};
}

#endif /* animation_app_hpp */
