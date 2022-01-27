//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_h
#define wgsl_h

#include "shader/shader_macro_collection.h"
#include <functional>

namespace vox {
class WGSL {
public:
    virtual ~WGSL() {}
    
    virtual const std::string& compile(const ShaderMacroCollection& macros) = 0;
};

using WGSLPtr = std::unique_ptr<WGSL>;
using WGSLCreator = std::function<WGSLPtr()>;

}
#endif /* wgsl_h */
