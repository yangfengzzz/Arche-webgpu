//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cascade_shadowmap_app_hpp
#define cascade_shadowmap_app_hpp

#include "forward_application.h"

namespace vox {
class CascadeShadowMapApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* cascade_shadowmap_app_hpp */
