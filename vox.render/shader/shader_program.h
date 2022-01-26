//
//  shader_program.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/26.
//

#ifndef shader_program_hpp
#define shader_program_hpp

#include <dawn/webgpu_cpp.h>
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
