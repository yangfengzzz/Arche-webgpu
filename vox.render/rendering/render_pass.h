//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_pass_hpp
#define render_pass_hpp

#include "subpass.h"
#include "resource_cache.h"
#include <vector>

namespace vox {
class RenderPass {
public:
    RenderPass(wgpu::Device& device, wgpu::RenderPassDescriptor& desc);
    
    RenderPass(const RenderPass &) = delete;
    
    RenderPass(RenderPass &&) = delete;
    
    virtual ~RenderPass() = default;
    
    RenderPass &operator=(const RenderPass &) = delete;
    
    RenderPass &operator=(RenderPass &&) = delete;
    
    void draw(wgpu::CommandEncoder& commandEncoder,
              std::optional<std::string> label = std::nullopt);
        
    const wgpu::RenderPassDescriptor& renderPassDescriptor();
            
public:
    /**
     * @brief Appends a subpass to the pipeline
     * @param subpass Subpass to append
     */
    void addSubpass(std::unique_ptr<Subpass> &&subpass);
    
    std::vector<std::unique_ptr<Subpass>> &subpasses();
    
    /**
     * @return Subpass currently being recorded, or the first one
     *         if drawing has not started
     */
    std::unique_ptr<Subpass> &activeSubpass();
    
public:
    void addParentPass(const std::string& name, RenderPass* pass);
    
    RenderPass* removeParentPass(const std::string& name);
    
    RenderPass* findPass(const std::string& name);
    
    void clearParentPass();
    
public:
    ResourceCache& resourceCache();
    
private:
    wgpu::RenderPassDescriptor& _desc;
    
    std::vector<std::unique_ptr<Subpass>> _subpasses;
    size_t _activeSubpassIndex{0};
    
    std::unordered_map<std::string, RenderPass*> _parentPass;
    
    ResourceCache _resourceCache;    
};

}

#endif /* render_pass_hpp */
