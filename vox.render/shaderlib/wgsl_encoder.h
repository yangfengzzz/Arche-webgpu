//
//  wgsl_encoder.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef wgsl_encoder_hpp
#define wgsl_encoder_hpp

#include "wgsl_common.h"
#include "shader/shader_macro_collection.h"
#include <dawn/webgpu_cpp.h>
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
    void addStruct(const std::string& code);
    
    void addFunction(const std::string& code);

public:
    void addUniformBinding(const std::string& uniformName, const std::string& type, uint32_t group = 0);
    
    void addUniformBinding(const std::string& uniformName, UniformType type, uint32_t group = 0);

public:
    void addInoutType(const std::string& structName, Attributes attributes, UniformType type);

    void addInoutType(const std::string& structName, uint32_t location, const std::string& attributes, UniformType type);
    
    void addInoutType(const std::string& structName, uint32_t location, const std::string& attributes, const std::string& type);
    
    void addInoutType(const std::string& structName, BuiltInType builtin, const std::string& attributes, UniformType type);
    
    void addInoutType(const std::string& structName, BuiltInType builtin, const std::string& attributes, const std::string& type);
    
public:
    void addEntry(const std::initializer_list<std::pair<std::string, std::string>>& inParam,
                  const std::string& outType, std::function<std::string()> code);
    
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
};

}

#endif /* wgsl_encoder_hpp */