//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shader_program_hpp
#define shader_program_hpp

#include <webgpu/webgpu_cpp.h>
#include <string>

namespace vox {
/**
 * Shader program, corresponding to the GPU shader program.
 */
class ShaderProgram {
public:
    ShaderProgram(wgpu::Device& device,
                  const std::string& vertexSource,
                  const std::string& fragmentSource);
    
    wgpu::ShaderModule& vertexShader();
    
    wgpu::ShaderModule& fragmentShader();
    
private:
    /**
     * init and link program with shader.
     */
    void _createProgram(const std::string& vertexSource,
                        const std::string& fragmentSource);
    
private:
    wgpu::ShaderModule _vertexShader;
    wgpu::ShaderModule _fragmentShader;
    wgpu::Device& _device;
};

}

#endif /* shader_program_hpp */
