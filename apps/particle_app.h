//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_app_hpp
#define particle_app_hpp

#include "forward_application.h"
#include "particle/particle_manager.h"

namespace vox {
class ParticleApp : public ForwardApplication {
public:
    bool prepare(Engine &engine) override;

    void loadScene(uint32_t width, uint32_t height) override;
    
    void updateGPUTask(wgpu::CommandEncoder& commandEncoder) override;
    
private:
    std::unique_ptr<ParticleManager> _manager{nullptr};
};
}
#endif /* particle_app_hpp */
