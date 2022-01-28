//
//  wgsl_library.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/28.
//

#ifndef wgsl_library_hpp
#define wgsl_library_hpp

#include <functional>
#include "shader/shader_macro_collection.h"

namespace vox {
class WGSLUniformBinding {
public:
    WGSLUniformBinding(const std::string& name, uint32_t group);
    
    std::string operator()();
    
private:
    std::string _name;
    uint32_t _group;
    uint32_t _binding;
};


}

#endif /* wgsl_library_hpp */
