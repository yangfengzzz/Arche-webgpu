//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_subpass_hpp
#define render_subpass_hpp

#include "subpass.h"

namespace vox {
class RenderSubpass : public Subpass {
public:
    Subpass::Type type() override final;

    RenderSubpass(RenderContext* renderContext,
                  Scene* scene,
                  Camera* camera);
    
    RenderSubpass(const RenderSubpass &) = delete;
    
    RenderSubpass(RenderSubpass &&) = default;
    
    virtual ~RenderSubpass() = default;
    
    RenderSubpass &operator=(const RenderSubpass &) = delete;
    
    RenderSubpass &operator=(RenderSubpass &&) = delete;
    
    /**
     * @brief Draw virtual function
     * @param commandEncoder CommandEncoder to use to record draw commands
     */
    virtual void draw(wgpu::RenderPassEncoder& commandEncoder) = 0;
    
protected:
    static bool _compareFromNearToFar(const RenderElement &a, const RenderElement &b);
    static bool _compareFromFarToNear(const RenderElement &a, const RenderElement &b);
};

}
#endif /* render_subpass_hpp */
