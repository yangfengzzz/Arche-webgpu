//
//  wgsl_library.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/28.
//

#include "wgsl_library.h"
#include "shader/shader.h"

namespace vox {
//MARK: - WGSLUniformBinding
WGSLUniformBinding::WGSLUniformBinding(const std::string& name, uint32_t group):
_name(name),
_group(group) {
    auto property = Shader::getPropertyByName(name);
    if (property.has_value()) {
        _binding = property.value().uniqueId;
    } else {
        assert(false);
    }
}

std::string WGSLUniformBinding::operator()() {
    std::string source =
    "@group(0) @binding(0)\n"
    "var<uniform> uPMatrix: mat4x4<f32>;\n";
    
    return source;
}

//MARK: -

}
