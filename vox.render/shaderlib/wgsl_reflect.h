//
//  wgsl_reflect.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef wgsl_reflect_hpp
#define wgsl_reflect_hpp

#include "wgsl.h"

namespace vox {
class WGSLReflect : public WGSL {
public:
    WGSLReflect(const std::string& filename);
};

}

#endif /* wgsl_reflect_hpp */
