//
//  wgsl.h
//  DigitalArche
//
//  Created by 杨丰 on 2022/1/27.
//

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
