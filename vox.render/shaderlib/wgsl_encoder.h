//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_encoder_hpp
#define wgsl_encoder_hpp

#include "vector3.h"
#include "wgsl_common.h"
#include "shader/shader_macro_collection.h"
#include <unordered_map>
#include <unordered_set>

namespace vox {
class WGSL;

class WGSLEncoder {
public:
    using BindGroupLayoutEntryMap = std::unordered_map<uint32_t, std::unordered_map<uint32_t, wgpu::BindGroupLayoutEntry>>;
    using BindGroupInfo = std::unordered_map<uint32_t, std::unordered_set<uint32_t>>;
    
    WGSLEncoder(WGSLEncoder const &other) = delete;
    
    WGSLEncoder(WGSLEncoder&& other) = delete;
    
    WGSLEncoder& operator=(WGSLEncoder const& other) = delete;
    
    WGSLEncoder& operator=(WGSLEncoder&& other) = delete;
    
    ~WGSLEncoder();
    
public:
    static size_t startCounter(uint32_t initVal = (uint32_t)Attributes::TOTAL_COUNT);
    
    static uint32_t getCounterNumber(size_t index);
    
    static void endCounter(size_t index);
    
public:
    void addStruct(const std::string& code);
    
    void addFunction(const std::string& code);

public:
    void addUniformBinding(const std::string& uniformName, UniformType type, uint32_t group = 0);

    void addUniformBinding(const std::string& uniformName, const std::string& type, uint32_t group = 0);
    
    void addUniformBinding(const std::string& uniformName, const std::string& type, uint32_t binding, uint32_t group);

    void addSampledTextureBinding(const std::string& texName, TextureType texType,
                                  const std::string& samplerName, SamplerType samplerType,
                                  uint32_t group = 0);
    
    void addSampledTextureBinding(const std::string& texName, TextureType texType, uint32_t texBinding,
                                  const std::string& samplerName, SamplerType samplerType, uint32_t samplerBinding,
                                  uint32_t group);
    
public:
    void addStorageBufferBinding(const std::string& bufferName, UniformType type,
                                 bool isRead, uint32_t group = 0);

    void addStorageBufferBinding(const std::string& bufferName, const std::string& type,
                                 bool isRead, uint32_t group = 0);
    
    void addStorageBufferBinding(const std::string& bufferName, const std::string& type,
                                 bool isRead, uint32_t binding, uint32_t group);
    
    void addStorageTextureBinding(const std::string& texName, StorageTextureType texType,
                                  wgpu::TextureFormat texelFormat, uint32_t group = 0);

public:
    void addInoutType(const std::string& structName, Attributes attributes, UniformType type);

    void addInoutType(const std::string& structName, uint32_t location, const std::string& attributes, UniformType type);
    
    void addInoutType(const std::string& structName, uint32_t location, const std::string& attributes, const std::string& type);
    
    void addInoutType(const std::string& structName, BuiltInType builtin, const std::string& attributes, UniformType type);
    
    void addInoutType(const std::string& structName, BuiltInType builtin, const std::string& attributes, const std::string& type);
    
public:
    void addEntry(const std::initializer_list<std::pair<std::string, std::string>>& inParam,
                  const std::pair<std::string, std::string>& outType, std::function<void(std::string&)> code,
                  const std::initializer_list<std::pair<std::string, BuiltInType>>& builtIn = {});

    void addEntry(const std::array<uint32_t, 3>& workgroupSize, std::function<void(std::string&)> code,
                  const std::initializer_list<std::pair<std::string, BuiltInType>>& builtIn = {});
    
public:
    void flush();
    
    void clearCache();
    
private:
    void _buildSource();
    
    friend class WGSL;
    WGSLEncoder(WGSL* wgsl, wgpu::ShaderStage currentStage);
    
    WGSL* _wgsl{nullptr};
    wgpu::ShaderStage _currentStage = wgpu::ShaderStage::None;

    std::string _source{};
    BindGroupInfo _bindGroupInfo{};
    BindGroupLayoutEntryMap _bindGroupLayoutEntryMap{};
    bool _needFlush{false};

    std::string _structBlock{};
    std::string _uniformBlock{};
    std::string _functionBlock{};
    std::unordered_map<std::string, std::vector<std::string>> _inoutType{};
    std::string _entryBlock{};
    
    static std::vector<uint32_t> _counters;
};

}

#endif /* wgsl_encoder_hpp */
