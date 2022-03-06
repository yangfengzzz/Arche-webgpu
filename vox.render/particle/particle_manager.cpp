//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_manager.h"
#include "particle/wgsl/wgsl_particle_emission.h"
#include <glog/logging.h>

namespace vox {
ParticleManager *ParticleManager::getSingletonPtr(void) {
    return msSingleton;
}

ParticleManager &ParticleManager::getSingleton(void) {
    assert(msSingleton);
    return (*msSingleton);
}
//-----------------------------------------------------------------------
ParticleManager::ParticleManager(wgpu::Device& device) {
    _emitterPass = std::make_unique<ComputePass>(device, std::make_unique<WGSLParticleEmission>());
    _simulationPass = std::make_unique<ComputePass>(device, nullptr);
}

const std::vector<Particle*>& ParticleManager::particles() const {
    return _particles;
}

void ParticleManager::addParticle(Particle* particle) {
    auto iter = std::find(_particles.begin(), _particles.end(), particle);
    if (iter == _particles.end()) {
        _particles.push_back(particle);
    } else {
        LOG(ERROR) << "Particle already attached." << std::endl;;
    }
}

void ParticleManager::removeParticle(Particle* particle) {
    auto iter = std::find(_particles.begin(), _particles.end(), particle);
    if (iter != _particles.end()) {
        _particles.erase(iter);
    }
}

void ParticleManager::update(wgpu::ComputePassEncoder passEncoder) {
    for (auto& particle : _particles) {        
        /* Max number of particles able to be spawned. */
        uint32_t const num_dead_particles = Particle::kMaxParticleCount - particle->numAliveParticles();
        /* Number of particles to be emitted. */
        uint32_t const emit_count = std::min(Particle::kBatchEmitCount, num_dead_particles); //
        _emission(emit_count, particle, passEncoder);
        _simulation(emit_count, particle, passEncoder);
    }
}

void ParticleManager::_emission(const uint32_t count, Particle* particle,
                                wgpu::ComputePassEncoder &passEncoder) {
    /* Emit only if a minimum count is reached. */
    if (!count) {
        return;
    }
    if (count < Particle::kBatchEmitCount) {
        //return;
    }
    particle->setEmitCount(count);
    
    _emitterPass->attachShaderData(&particle->shaderData);
    auto nGroups = threadsGroupCount(count);
    _emitterPass->setDispatchCount(nGroups);
    _emitterPass->compute(passEncoder);
    _emitterPass->detachShaderData(&particle->shaderData);
}

void ParticleManager::_simulation(const uint32_t count, Particle* particle,
                                  wgpu::ComputePassEncoder &passEncoder) {
    if (particle->numAliveParticles() + count == 0u) {
        return;
    }
    
    _simulationPass->attachShaderData(&particle->shaderData);
    auto nGroups = threadsGroupCount(particle->numAliveParticles() + count);
    _simulationPass->setDispatchCount(nGroups);
    _simulationPass->compute(passEncoder);
    _simulationPass->detachShaderData(&particle->shaderData);
}

}
