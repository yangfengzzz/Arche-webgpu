//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/shader_data.h"

#include <utility>

#include "shader.h"

namespace vox {
ShaderData::ShaderData(wgpu::Device &device) : _device(device) {}

std::optional<Buffer> ShaderData::getData(const std::string &property_name) {
    auto property = Shader::getPropertyByName(property_name);
    if (property.has_value()) {
        return getData(property.value().uniqueId);
    } else {
        return std::nullopt;
    }
}

std::optional<Buffer> ShaderData::getData(const ShaderProperty &property) { return getData(property.uniqueId); }

std::optional<Buffer> ShaderData::getData(uint32_t uniqueID) {
    auto iter = _shaderBuffers.find(uniqueID);
    if (iter != _shaderBuffers.end()) {
        return iter->second;
    }

    auto functorIter = _shaderBufferFunctors.find(uniqueID);
    if (functorIter != _shaderBufferFunctors.end()) {
        return functorIter->second();
    }

    return std::nullopt;
}

const std::unordered_map<uint32_t, Buffer> &ShaderData::shaderBuffers() const { return _shaderBuffers; }

void ShaderData::setBufferFunctor(const std::string &property_name, const std::function<Buffer()> &functor) {
    auto property = Shader::getPropertyByName(property_name);
    if (property.has_value()) {
        setBufferFunctor(property.value(), functor);
    } else {
        assert(false && "can't find property");
    }
}

void ShaderData::setBufferFunctor(const ShaderProperty &property, const std::function<Buffer()> &functor) {
    _shaderBufferFunctors.insert(std::make_pair(property.uniqueId, functor));
}

// MARK: - Sampler&&Texture
void ShaderData::setSampledTexture(const std::string &texture_name,
                                   const std::string &sample_name,
                                   const SampledTexturePtr &value) {
    // Texture
    {
        auto property = Shader::getPropertyByName(texture_name);
        if (property.has_value()) {
            _shaderTextures[property->uniqueId] = value;
        } else {
            assert(false && "can't find property");
        }
    }
    // Sampler
    {
        auto property = Shader::getPropertyByName(sample_name);
        if (property.has_value()) {
            _shaderSamplers[property->uniqueId] = value;
        } else {
            assert(false && "can't find property");
        }
    }
}

void ShaderData::setSampledTexture(const ShaderProperty &texture_prop,
                                   const ShaderProperty &sample_prop,
                                   const SampledTexturePtr &value) {
    _shaderTextures[texture_prop.uniqueId] = value;
    _shaderSamplers[sample_prop.uniqueId] = value;
}

std::optional<wgpu::TextureView> ShaderData::getTextureView(uint32_t uniqueID) {
    auto iter = _shaderTextures.find(uniqueID);
    if (iter != _shaderTextures.end()) {
        return iter->second->textureView();
    } else {
        return std::nullopt;
    }
}

std::optional<wgpu::Sampler> ShaderData::getSampler(uint32_t uniqueID) {
    auto iter = _shaderSamplers.find(uniqueID);
    if (iter != _shaderSamplers.end()) {
        return iter->second->sampler();
    } else {
        return std::nullopt;
    }
}

// MARK: - Macro
void ShaderData::enableMacro(const std::string &macroName) { _macroCollection.enableMacro(macroName); }

void ShaderData::enableMacro(const std::string &macroName, double value) {
    _macroCollection.enableMacro(macroName, value);
}

void ShaderData::disableMacro(const std::string &macroName) { _macroCollection.disableMacro(macroName); }

void ShaderData::enableMacro(MacroName macroName) { _macroCollection.enableMacro(macroName); }

void ShaderData::enableMacro(MacroName macroName, double value) { _macroCollection.enableMacro(macroName, value); }

void ShaderData::disableMacro(MacroName macroName) { _macroCollection.disableMacro(macroName); }

void ShaderData::mergeMacro(const ShaderMacroCollection &macros, ShaderMacroCollection &result) const {
    ShaderMacroCollection::unionCollection(macros, _macroCollection, result);
}

}  // namespace vox
