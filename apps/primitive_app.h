//
//  primitive_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/27.
//

#ifndef primitive_app_hpp
#define primitive_app_hpp

#include "forward_application.h"

namespace vox {
class PrimitiveApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* primitive_app_hpp */
