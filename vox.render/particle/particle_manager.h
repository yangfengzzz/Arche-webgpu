//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_manager_hpp
#define particle_manager_hpp

#include "particle_renderer.h"
#include "rendering/compute_pass.h"
#include "singleton.h"

namespace vox {
class ParticleManager : public Singleton<ParticleManager> {
public:
    static constexpr uint32_t PARTICLES_KERNEL_GROUP_WIDTH = 256;

    static constexpr uint32_t threadsGroupCount(uint32_t nthreads) {
        return (nthreads + PARTICLES_KERNEL_GROUP_WIDTH - 1u) / PARTICLES_KERNEL_GROUP_WIDTH;
    }

    static constexpr uint32_t floorParticleCount(uint32_t nparticles) {
        return PARTICLES_KERNEL_GROUP_WIDTH * (nparticles / PARTICLES_KERNEL_GROUP_WIDTH);
    }

    static ParticleManager& getSingleton(void);

    static ParticleManager* getSingletonPtr(void);

    ParticleManager(wgpu::Device& device);

    const std::vector<ParticleRenderer*>& particles() const;

    void addParticle(ParticleRenderer* particle);

    void removeParticle(ParticleRenderer* particle);

    void draw(wgpu::CommandEncoder& commandEncoder);

public:
    float timeStepFactor() const;

    void setTimeStepFactor(float factor);

private:
    void _emission(const uint32_t count, ParticleRenderer* particle, wgpu::ComputePassEncoder& passEncoder);

    void _simulation(ParticleRenderer* particle, wgpu::ComputePassEncoder& passEncoder);

private:
    std::vector<ParticleRenderer*> _particles{};
    float _timeStepFactor = 1.0f;

    std::unique_ptr<ComputePass> _emitterPass{nullptr};
    std::unique_ptr<ComputePass> _simulationPass{nullptr};
};

template <>
inline ParticleManager* Singleton<ParticleManager>::ms_singleton{nullptr};

}  // namespace vox
#endif /* particle_manager_hpp */
