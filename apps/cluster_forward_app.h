//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cluster_forward_app_hpp
#define cluster_forward_app_hpp

#include "forward_application.h"

namespace vox {
class ClusterForwardApp : public ForwardApplication {
public:
    void loadScene() override;
};
 
}
#endif /* cluster_forward_app_hpp */
