//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <tuple>
#include <vector>

#include "vox.geometry/particle_emitter3.h"

namespace vox {

//!
//! \brief 3-D particle-based emitter set.
//!
class ParticleEmitterSet3 final : public ParticleEmitter3 {
public:
    class Builder;

    //! Constructs an emitter.
    ParticleEmitterSet3();

    //! Constructs an emitter with sub-emitters.
    explicit ParticleEmitterSet3(std::vector<ParticleEmitter3Ptr> emitters);

    //! Destructor.
    ~ParticleEmitterSet3() override;

    //! Adds sub-emitter.
    void addEmitter(const ParticleEmitter3Ptr& emitter);

    //! Returns builder fox ParticleEmitterSet3.
    static Builder builder();

private:
    std::vector<ParticleEmitter3Ptr> _emitters;

    void onSetTarget(const ParticleSystemData3Ptr& particles) override;

    void onUpdate(double currentTimeInSeconds, double timeIntervalInSecond) override;
};

//! Shared pointer type for the ParticleEmitterSet3.
typedef std::shared_ptr<ParticleEmitterSet3> ParticleEmitterSet3Ptr;

//!
//! \brief Front-end to create ParticleEmitterSet3 objects step by step.
//!
class ParticleEmitterSet3::Builder final {
public:
    //! Returns builder with list of sub-emitters.
    Builder& withEmitters(const std::vector<ParticleEmitter3Ptr>& emitters);

    //! Builds ParticleEmitterSet3.
    [[nodiscard]] ParticleEmitterSet3 build() const;

    //! Builds shared pointer of ParticleEmitterSet3 instance.
    [[nodiscard]] ParticleEmitterSet3Ptr makeShared() const;

private:
    std::vector<ParticleEmitter3Ptr> _emitters;
};

}  // namespace vox
