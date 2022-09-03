//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <tuple>
#include <vector>

#include "vox.geometry/particle_emitter2.h"

namespace vox {

//!
//! \brief 2-D particle-based emitter set.
//!
class ParticleEmitterSet2 final : public ParticleEmitter2 {
public:
    class Builder;

    //! Constructs an emitter.
    ParticleEmitterSet2();

    //! Constructs an emitter with sub-emitters.
    explicit ParticleEmitterSet2(std::vector<ParticleEmitter2Ptr> emitters);

    //! Destructor.
    ~ParticleEmitterSet2() override;

    //! Adds sub-emitter.
    void addEmitter(const ParticleEmitter2Ptr& emitter);

    //! Returns builder fox ParticleEmitterSet2.
    static Builder builder();

private:
    std::vector<ParticleEmitter2Ptr> _emitters;

    void onSetTarget(const ParticleSystemData2Ptr& particles) override;

    void onUpdate(double currentTimeInSeconds, double timeIntervalInSecond) override;
};

//! Shared pointer type for the ParticleEmitterSet2.
typedef std::shared_ptr<ParticleEmitterSet2> ParticleEmitterSet2Ptr;

//!
//! \brief Front-end to create ParticleEmitterSet2 objects step by step.
//!
class ParticleEmitterSet2::Builder final {
public:
    //! Returns builder with list of sub-emitters.
    Builder& withEmitters(const std::vector<ParticleEmitter2Ptr>& emitters);

    //! Builds ParticleEmitterSet2.
    [[nodiscard]] ParticleEmitterSet2 build() const;

    //! Builds shared pointer of ParticleEmitterSet2 instance.
    [[nodiscard]] ParticleEmitterSet2Ptr makeShared() const;

private:
    std::vector<ParticleEmitter2Ptr> _emitters;
};

}  // namespace vox
