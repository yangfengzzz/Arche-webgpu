//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shader_data_hpp
#define shader_data_hpp

#include "shader_data_group.h"
#include "shader_macro_collection.h"
#include "shader_property.h"
#include "shader.h"
#include "std_helpers.h"
#include <any>
#include <unordered_map>
#include <dawn/webgpu_cpp.h>

namespace vox {
/**
 * Shader data collection,Correspondence includes shader properties data and macros data.
 */
class ShaderData {
public:
    ShaderData(wgpu::Device& device);
    
    std::optional<wgpu::Buffer> getData(const std::string &property_name);
    
    std::optional<wgpu::Buffer> getData(const ShaderProperty &property);
    
    template<typename T>
    void setData(const std::string &property_name, const T& value) {
        auto property = Shader::getPropertyByName(property_name);
        if (property.has_value()) {
            setData(property.value(), value);
        } else {
            assert(false && "can't find property");
        }
    }
    
    template<typename T>
    void setData(ShaderProperty property, const T& value) {
        auto iter = _properties.find(property.uniqueId);
        if (iter == _properties.end()) {
            wgpu::BufferDescriptor desc;
            desc.size = sizeof(T);
            desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
            _properties[property.uniqueId] = _device.CreateBuffer(&desc);
        }
        
        std::vector<uint8_t> bytes = to_bytes(value);
        _device.GetQueue().WriteBuffer(_properties[property.uniqueId], 0, bytes.data(), sizeof(T));
    }
    
    const std::unordered_map<int, wgpu::Buffer> &properties() const;
    
public:
    /**
     * Enable macro.
     * @param macroName - Shader macro
     */
    void enableMacro(const std::string& macroName);

    /**
     * Enable macro.
     * @remarks Name and value will combine one macro, it's equal the macro of "name value".
     * @param macroName - Macro name
     * @param value - Macro value
     */
    void enableMacro(const std::string& macroName, double value);

    /**
     * Disable macro
     * @param macroName - Macro name
     */
    void disableMacro(const std::string& macroName);

    void mergeMacro(const ShaderMacroCollection &macros,
                    ShaderMacroCollection &result) const;
    
private:
    wgpu::Device& _device;
    std::unordered_map<int, wgpu::Buffer> _properties;
    
    ShaderMacroCollection _macroCollection;
};

}
#endif /* shader_data_hpp */
