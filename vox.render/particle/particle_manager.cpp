//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_manager.h"
#include <glog/logging.h>

namespace vox {
template<> ParticleManager *Singleton<ParticleManager>::msSingleton = 0;

//-----------------------------------------------------------------------
ParticleManager *ParticleManager::getSingletonPtr(void) {
    return msSingleton;
}

ParticleManager &ParticleManager::getSingleton(void) {
    assert(msSingleton);
    return (*msSingleton);
}

ParticleManager::ParticleManager(wgpu::Device& device) {
    _emitterPass = std::make_unique<ComputePass>(device, nullptr);
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
    _emitterPass->compute(passEncoder);
    _simulationPass->compute(passEncoder);
}

}
