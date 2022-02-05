//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_h
#define wgsl_h

#include "wgsl_common.h"
#include "shaderlib/wgsl_encoder.h"
#include "shader/shader_macro_collection.h"
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
    WGSLEncoder createSourceEncoder(wgpu::ShaderStage currentStage);
    
protected:
    friend class WGSLEncoder;
    void _setSource(const std::string& source);
    void _setBindGroupInfo(const BindGroupInfo& info);
    void _setBindGroupLayoutEntryMap(const BindGroupLayoutEntryMap& map);
    
    std::string _source{};
    BindGroupInfo _bindGroupInfo{};
    BindGroupLayoutEntryMap _bindGroupLayoutEntryMap{};
};
using WGSLPtr = std::unique_ptr<WGSL>;

}
#endif /* wgsl_h */
