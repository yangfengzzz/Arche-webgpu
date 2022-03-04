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
    Subpass::Type type() override final;
    
    ComputeSubpass(RenderContext *context,
                   Scene *scene,
                   Camera *camera);
    
    ComputeSubpass(const ComputeSubpass &) = delete;
    
    ComputeSubpass(ComputeSubpass &&) = default;
    
    virtual ~ComputeSubpass() = default;
    
    ComputeSubpass &operator=(const ComputeSubpass &) = delete;
    
    ComputeSubpass &operator=(ComputeSubpass &&) = delete;
    
    /**
     * @brief Compute virtual function
     * @param commandEncoder CommandEncoder to use to record compute commands
     */
    virtual void compute(wgpu::ComputePassEncoder &commandEncoder) = 0;
};

}
#endif /* compute_subpass_hpp */
