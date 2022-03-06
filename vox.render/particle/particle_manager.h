//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_manager_hpp
#define particle_manager_hpp

#include "singleton.h"
#include "particle.h"
#include "rendering/compute_pass.h"

namespace vox {
class ParticleManager : public Singleton<ParticleManager> {
public:
    static ParticleManager &getSingleton(void);
    
    static ParticleManager *getSingletonPtr(void);
    
    ParticleManager(wgpu::Device& device);
    
    const std::vector<Particle*>& particles() const;
    
    void addParticle(Particle* particle);
    
    void removeParticle(Particle* particle);
    
    void update(wgpu::ComputePassEncoder passEncoder);
    
private:
    std::vector<Particle*> _particles{};
    
    std::unique_ptr<ComputePass> _emitterPass{nullptr};
    std::unique_ptr<ComputePass> _simulationPass{nullptr};
};

}
#endif /* particle_manager_hpp */
