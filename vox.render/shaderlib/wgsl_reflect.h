//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
