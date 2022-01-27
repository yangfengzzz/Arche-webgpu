//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_program.h"

namespace vox {
ShaderProgram::ShaderProgram(wgpu::Device& device,
                             const std::string& vertexSource,
                             const std::string& fragmentSource):
_device(device) {
    _createProgram(vertexSource, fragmentSource);
}

wgpu::ShaderModule& ShaderProgram::vertexShader() {
    return _vertexShader;
}

wgpu::ShaderModule& ShaderProgram::fragmentShader() {
    return _fragmentShader;
}

void ShaderProgram::_createProgram(const std::string& vertexSource,
                                   const std::string& fragmentSource) {
    wgpu::ShaderModuleDescriptor desc;
    wgpu::ShaderModuleWGSLDescriptor wgslDesc;
    desc.nextInChain = &wgslDesc;

    wgslDesc.source = vertexSource.c_str();
    _vertexShader = _device.CreateShaderModule(&desc);
    
    wgslDesc.source = fragmentSource.c_str();
    _fragmentShader = _device.CreateShaderModule(&desc);
}

}
