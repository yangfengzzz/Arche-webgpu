//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_h
#define wgsl_h

#include "wgsl_library.h"
#include <unordered_map>
#include <unordered_set>
#include <dawn/webgpu_cpp.h>

namespace vox {
class WGSL {
public:
    using BindGroupLayoutEntryMap = std::unordered_map<uint32_t, std::unordered_map<uint32_t, wgpu::BindGroupLayoutEntry>>;
    using BindGroupInfo = std::unordered_map<uint32_t, std::unordered_set<uint32_t>>;
    
    class Builder;
    
    WGSL() = default;
    
    WGSL(const std::string& source,
         const BindGroupInfo& info,
         const BindGroupLayoutEntryMap& entryMap);
    
    virtual ~WGSL() = default;
    
    virtual std::pair<const std::string&, const BindGroupInfo&> compile(const ShaderMacroCollection& macros);
    
    const BindGroupLayoutEntryMap& bindGroupLayoutEntryMap();
    
public:
    void begin(wgpu::ShaderStage stage);
    
    void addStruct(const std::string& code);

    void addUniformBinding(const std::string& name, const std::string& type, uint32_t group = 0);
    
    void addUniformBinding(const std::string& name, UniformType type, uint32_t group = 0);
        
    void beginInputStruct(const std::string& name);
    
    void addInputType(const std::string& code);
    
    void endInputStruct();
    
    void beginOutputStruct(const std::string& name);
    
    void addOutputType(const std::string& code);
    
    void endOutputStruct();
    
    void beginEntry(const std::initializer_list<std::string>& parameter);
    
    void addEntry(const std::string& code);
    
    void endEntry();
    
    void end();
    
protected:
    std::string _source{};
    BindGroupInfo _bindGroupInfo{};
    BindGroupLayoutEntryMap _bindGroupLayoutEntryMap{};
    
    wgpu::ShaderStage _currentStage = wgpu::ShaderStage::None;
    std::string _outputTypeName{};
    std::vector<std::string> _inputTypeName{};
};
using WGSLPtr = std::unique_ptr<WGSL>;

//MARK: - Builder
class WGSL::Builder {
public:
    Builder& begin(wgpu::ShaderStage stage);
    
    Builder& addStruct(const std::string& code);

    Builder& addUniformBinding(const std::string& name, UniformType type, uint32_t group = 0);

    Builder& addUniformBinding(const std::string& name, const std::string& type, uint32_t group = 0);
            
    Builder& beginInputStruct(const std::string& name);
    
    Builder& addInputType(const std::string& code);
    
    Builder& endInputStruct();
    
    Builder& beginOutputStruct(const std::string& name);
    
    Builder& addOutputType(const std::string& code);
    
    Builder& endOutputStruct();
    
    Builder& beginEntry(const std::initializer_list<std::string>& parameter);
    
    Builder& addEntry(const std::string& code);
    
    Builder& endEntry();
    
    Builder& end();
    
    WGSLPtr build();
    
private:
    std::string _source{};
    WGSL::BindGroupInfo _bindGroupInfo{};
    WGSL::BindGroupLayoutEntryMap _bindGroupLayoutEntryMap{};
    
    wgpu::ShaderStage _currentStage = wgpu::ShaderStage::None;
    std::string _outputTypeName{};
    std::vector<std::string> _inputTypeName{};
};

}
#endif /* wgsl_h */
