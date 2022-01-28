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
enum class UniformType {
    Vec2f32,
    Vec2i32,
    Vec2u32,
    Vec3f32,
    Vec3i32,
    Vec3u32,
    Vec4f32,
    Vec4i32,
    Vec4u32,
    Mat2x2f32,
    Mat3x2f32,
    Mat4x2f32,
    Mat2x3f32,
    Mat3x3f32,
    Mat4x3f32,
    Mat2x4f32,
    Mat3x4f32,
    Mat4x4f32,
};

std::string uniformTypeToString(UniformType type);


class WGSLUniformBinding {
public:
    WGSLUniformBinding(const std::string& name, UniformType type,
                       uint32_t group = 0);
    
    std::string operator()();
    
private:
    std::string _name;
    UniformType _type;
    uint32_t _group;
    uint32_t _binding;
};

class WGSLPatchTest {
public:
    WGSLPatchTest(std::vector<std::string>& uniform,
                  std::vector<std::string>& input,
                  std::vector<std::string>& output,
                  std::vector<std::string>& entry,
                  std::vector<std::string>& function);
    
    void operator()();
    
private:
    WGSLUniformBinding _uPMatirx;
    WGSLUniformBinding _uMVMatrix;
    
    std::vector<std::string> &_uniform;
    std::vector<std::string> &_input;
    std::vector<std::string> &_output;
    std::vector<std::string> &_entry;
    std::vector<std::string> &_function;
};


}

#endif /* wgsl_library_hpp */
