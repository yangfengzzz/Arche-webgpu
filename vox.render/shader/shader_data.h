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
#include "graphics/buffer.h"
#include "texture/sampled_texture.h"
#include "std_helpers.h"
#include <any>
#include <unordered_map>
#include <webgpu/webgpu_cpp.h>

namespace vox {
/**
 * Shader data collection,Correspondence includes shader properties data and macros data.
 */
class ShaderData {
public:
    ShaderData(wgpu::Device& device);
    
    const std::unordered_map<uint32_t, Buffer> &shaderBuffers() const;
    
    std::optional<Buffer> getData(const std::string &property_name);
    
    std::optional<Buffer> getData(const ShaderProperty &property);
    
    std::optional<Buffer> getData(uint32_t uniqueID);
    
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
        auto iter = _shaderBuffers.find(property.uniqueId);
        if (iter == _shaderBuffers.end()) {
            _shaderBuffers.insert(std::make_pair(property.uniqueId,
                                                 Buffer(_device, sizeof(T),
                                                        wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst)));
        }
        iter = _shaderBuffers.find(property.uniqueId);
        
        std::vector<uint8_t> bytes = to_bytes(value);
        _device.GetQueue().WriteBuffer(iter->second.handle(), 0, bytes.data(), sizeof(T));
    }
    
    template<typename T>
    void setData(ShaderProperty property, const std::vector<T>& value) {
        auto iter = _shaderBuffers.find(property.uniqueId);
        if (iter == _shaderBuffers.end()) {
            _shaderBuffers.insert(std::make_pair(property.uniqueId,
                                                 Buffer(_device, sizeof(T) * value.size(),
                                                        wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst)));
        }
        iter = _shaderBuffers.find(property.uniqueId);
        
        _device.GetQueue().WriteBuffer(iter->second.handle(), 0, value.data(), sizeof(T) * value.size());
    }
    
public:
    void setSampledTexture(const std::string &texture_name,
                           const std::string &sample_name,
                           const SampledTexturePtr& value);
    
    void setSampledTexture(const ShaderProperty &texture_prop,
                           const ShaderProperty &sample_prop,
                           const SampledTexturePtr& value);
    
    wgpu::TextureView getTextureView(uint32_t uniqueID);
    
    wgpu::Sampler& getSampler(uint32_t uniqueID);
    
public:
    /**
     * Enable macro.
     * @param macroName - Shader macro
     */
    void enableMacro(const std::string& macroName);
    
    void enableMacro(MacroName macroName);
    
    /**
     * Enable macro.
     * @remarks Name and value will combine one macro, it's equal the macro of "name value".
     * @param macroName - Macro name
     * @param value - Macro value
     */
    void enableMacro(const std::string& macroName, double value);
    
    void enableMacro(MacroName macroName, double value);
    
    /**
     * Disable macro
     * @param macroName - Macro name
     */
    void disableMacro(const std::string& macroName);
    
    void disableMacro(MacroName macroName);
    
    void mergeMacro(const ShaderMacroCollection &macros,
                    ShaderMacroCollection &result) const;
    
private:
    wgpu::Device& _device;
    std::unordered_map<uint32_t, Buffer> _shaderBuffers{};
    std::unordered_map<uint32_t, SampledTexturePtr> _shaderTextures{};
    std::unordered_map<uint32_t, SampledTexturePtr> _shaderSamplers{};
    
    ShaderMacroCollection _macroCollection;
};

}
#endif /* shader_data_hpp */
