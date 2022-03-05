//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef subpass_hpp
#define subpass_hpp

#include "render_context.h"
#include "scene.h"

namespace vox {
class RenderPass;

class Subpass {
public:
    enum Type {
        Render,
        Compute,
    };
    
    virtual Type type() = 0;
    
    Subpass() = default;
    
    Subpass(const Subpass &) = delete;
    
    Subpass(Subpass &&) = default;
    
    virtual ~Subpass() = default;
    
    Subpass &operator=(const Subpass &) = delete;
    
    Subpass &operator=(Subpass &&) = delete;
    
    /**
     * @brief Prepares the shaders and shader variants for a subpass
     */
    virtual void prepare() = 0;
    
    virtual void setRenderPass(RenderPass* pass);
    
protected:
    RenderPass* _pass{nullptr};
};

}

#endif /* subpass_hpp */
