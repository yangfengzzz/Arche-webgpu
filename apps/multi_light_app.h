//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef multi_light_app_hpp
#define multi_light_app_hpp

#include "forward_application.h"

namespace vox {
class MultiLightApp : public ForwardApplication {
public:
    void loadScene() override;
};
 
}
#endif /* multi_light_app_hpp */
