//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shadowmap_app_hpp
#define shadowmap_app_hpp

#include "forward_application.h"

namespace vox {
class ShadowMapApp : public ForwardApplication {
public:
    void loadScene() override;
};
}

#endif /* shadowmap_app_hpp */
