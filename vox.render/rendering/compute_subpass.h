//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef compute_subpass_hpp
#define compute_subpass_hpp

#include "subpass.h"

namespace vox {
class ComputeSubpass : public Subpass {
public:
    using BindGroupLayoutEntryVecMap = std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>>;
    using BindGroupLayoutDescriptorMap = std::unordered_map<uint32_t, wgpu::BindGroupLayoutDescriptor>;
    
    Subpass::Type type() override final;
    
    ComputeSubpass(WGSLPtr&& source);
    
    ComputeSubpass(const ComputeSubpass &) = delete;
    
    ComputeSubpass(ComputeSubpass &&) = default;
    
    virtual ~ComputeSubpass() = default;
    
    ComputeSubpass &operator=(const ComputeSubpass &) = delete;
    
    ComputeSubpass &operator=(ComputeSubpass &&) = delete;
    
public:
    /**
     * @brief Prepares the shaders and shader variants for a subpass
     */
    virtual void prepare() override;
    
    /**
     * @brief Compute virtual function
     * @param commandEncoder CommandEncoder to use to record compute commands
     */
    virtual void compute(wgpu::ComputePassEncoder &commandEncoder);
    
protected:
    wgpu::ShaderModule &_compileShader(const ShaderMacroCollection& macros);
    
    void _flush();
    
    WGSLPtr _source;
    BindGroupLayoutEntryVecMap _bindGroupLayoutEntryVecMap{};
    BindGroupLayoutDescriptorMap _bindGroupLayoutDescriptorMap{};
    
private:
    wgpu::BindGroupLayoutEntry _findEntry(uint32_t group, uint32_t binding);
};

}
#endif /* compute_subpass_hpp */
