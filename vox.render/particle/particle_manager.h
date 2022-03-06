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
    static constexpr uint32_t PARTICLES_KERNEL_GROUP_WIDTH = 512;
    
    static constexpr uint32_t threadsGroupCount(uint32_t nthreads) {
        return (nthreads + PARTICLES_KERNEL_GROUP_WIDTH-1u) / PARTICLES_KERNEL_GROUP_WIDTH;
    }

    static constexpr uint32_t floorParticleCount(uint32_t nparticles) {
      return PARTICLES_KERNEL_GROUP_WIDTH * (nparticles / PARTICLES_KERNEL_GROUP_WIDTH);
    }
    
    static ParticleManager &getSingleton(void);
    
    static ParticleManager *getSingletonPtr(void);
    
    ParticleManager(wgpu::Device& device);
    
    const std::vector<Particle*>& particles() const;
    
    void addParticle(Particle* particle);
    
    void removeParticle(Particle* particle);
    
    void update(wgpu::ComputePassEncoder passEncoder);
    
private:
    void _emission(const uint32_t count,
                   Particle* particle, wgpu::ComputePassEncoder &passEncoder);
    
    void _simulation(const uint32_t count,
                     Particle* particle, wgpu::ComputePassEncoder &passEncoder);
    
    std::vector<Particle*> _particles{};
    
    std::unique_ptr<ComputePass> _emitterPass{nullptr};
    std::unique_ptr<ComputePass> _simulationPass{nullptr};
};

template<> inline ParticleManager *Singleton<ParticleManager>::msSingleton{nullptr};

}
#endif /* particle_manager_hpp */
