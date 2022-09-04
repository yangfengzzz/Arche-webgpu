//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/particle/particle_manager.h"

#include "vox.base/logging.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
ParticleManager* ParticleManager::getSingletonPtr() { return ms_singleton; }

ParticleManager& ParticleManager::getSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}
//-----------------------------------------------------------------------
ParticleManager::ParticleManager(wgpu::Device& device) {
    _emitterPass = std::make_unique<ComputePass>(
            device, ShaderManager::GetSingleton().LoadShader("base/particle/particle_emission.comp"));
    _simulationPass = std::make_unique<ComputePass>(
            device, ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));
}

const std::vector<ParticleRenderer*>& ParticleManager::particles() const { return _particles; }

void ParticleManager::addParticle(ParticleRenderer* particle) {
    auto iter = std::find(_particles.begin(), _particles.end(), particle);
    if (iter == _particles.end()) {
        _particles.push_back(particle);
    } else {
        LOGE("Particle already attached.")
    }
}

void ParticleManager::removeParticle(ParticleRenderer* particle) {
    auto iter = std::find(_particles.begin(), _particles.end(), particle);
    if (iter != _particles.end()) {
        _particles.erase(iter);
    }
}

float ParticleManager::timeStepFactor() const { return _timeStepFactor; }

void ParticleManager::setTimeStepFactor(float factor) { _timeStepFactor = factor; }

void ParticleManager::draw(wgpu::CommandEncoder& commandEncoder) {
    auto passEncoder = commandEncoder.BeginComputePass();
    for (auto& particle : _particles) {
        /* Max number of particles able to be spawned. */
        uint32_t const num_dead_particles = ParticleRenderer::kMaxParticleCount - particle->numAliveParticles();
        /* Number of particles to be emitted. */
        uint32_t const emit_count = std::min(ParticleRenderer::kBatchEmitCount, num_dead_particles);  //
        _emission(emit_count, particle, passEncoder);
        _simulation(particle, passEncoder);
    }
    passEncoder.End();
}

void ParticleManager::_emission(const uint32_t count,
                                ParticleRenderer* particle,
                                wgpu::ComputePassEncoder& passEncoder) {
    /* Emit only if a minimum count is reached. */
    if (!count) {
        return;
    }
    if (count < ParticleRenderer::kBatchEmitCount) {
        // return;
    }
    particle->setEmitCount(count);

    _emitterPass->attachShaderData(&particle->shaderData);
    auto nGroups = threadsGroupCount(count);
    _emitterPass->setDispatchCount(nGroups);
    _emitterPass->compute(passEncoder);
    _emitterPass->detachShaderData(&particle->shaderData);
}

void ParticleManager::_simulation(ParticleRenderer* particle, wgpu::ComputePassEncoder& passEncoder) {
    if (particle->numAliveParticles() == 0u) {
        return;
    }

    _simulationPass->attachShaderData(&particle->shaderData);
    auto nGroups = threadsGroupCount(particle->numAliveParticles());
    _simulationPass->setDispatchCount(nGroups);
    _simulationPass->compute(passEncoder);
    _simulationPass->detachShaderData(&particle->shaderData);
}

}  // namespace vox
