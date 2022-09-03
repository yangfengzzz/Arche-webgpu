// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/particle_emitter3.h"

namespace vox {

ParticleEmitter3::ParticleEmitter3() = default;

ParticleEmitter3::~ParticleEmitter3() = default;

const ParticleSystemData3Ptr& ParticleEmitter3::target() const { return _particles; }

void ParticleEmitter3::setTarget(const ParticleSystemData3Ptr& particles) {
    _particles = particles;

    onSetTarget(particles);
}

bool ParticleEmitter3::isEnabled() const { return _isEnabled; }

void ParticleEmitter3::setIsEnabled(bool enabled) { _isEnabled = enabled; }

void ParticleEmitter3::update(double currentTimeInSeconds, double timeIntervalInSeconds) {
    if (_onBeginUpdateCallback) {
        _onBeginUpdateCallback(this, currentTimeInSeconds, timeIntervalInSeconds);
    }

    onUpdate(currentTimeInSeconds, timeIntervalInSeconds);
}

void ParticleEmitter3::onSetTarget(const ParticleSystemData3Ptr& particles) {}

void ParticleEmitter3::setOnBeginUpdateCallback(const OnBeginUpdateCallback& callback) {
    _onBeginUpdateCallback = callback;
}

}  // namespace vox
