//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <limits>
#include <random>

#include "vox.geometry/particle_emitter2.h"

namespace vox {

//!
//! \brief 2-D point particle emitter.
//!
//! This class emits particles from a single point in given direction, speed,
//! and spreading angle.
//!
class PointParticleEmitter2 final : public ParticleEmitter2 {
public:
    class Builder;

    //!
    //! Constructs an emitter that spawns particles from given origin,
    //! direction, speed, spread angle, max number of new particles per second,
    //! max total number of particles to be emitted, and random seed.
    //!
    //! \param[in]  origin                      The origin.
    //! \param[in]  direction                   The direction.
    //! \param[in]  speed                       The speed.
    //! \param[in]  spreadAngleInDegrees        The spread angle in degrees.
    //! \param[in]  maxNumOfNewParticlesPerSec  The max number of new particles
    //!                                         per second.
    //! \param[in]  maxNumOfParticles           The max number of particles to
    //!                                         be emitted.
    //! \param[in]  seed                        The random seed.
    //!
    PointParticleEmitter2(const Point2D& origin,
                          const Vector2D& direction,
                          double speed,
                          double spreadAngleInDegrees,
                          size_t maxNumOfNewParticlesPerSec = 1,
                          size_t maxNumOfParticles = std::numeric_limits<size_t>::max(),
                          uint32_t seed = 0);

    //! Returns max number of new particles per second.
    [[nodiscard]] size_t maxNumberOfNewParticlesPerSecond() const;

    //! Sets max number of new particles per second.
    void setMaxNumberOfNewParticlesPerSecond(size_t rate);

    //! Returns max number of particles to be emitted.
    [[nodiscard]] size_t maxNumberOfParticles() const;

    //! Sets max number of particles to be emitted.
    void setMaxNumberOfParticles(size_t maxNumberOfParticles);

    //! Returns builder fox PointParticleEmitter2.
    static Builder builder();

private:
    std::mt19937 _rng;

    double _firstFrameTimeInSeconds = 0.0;
    size_t _numberOfEmittedParticles = 0;

    size_t _maxNumberOfNewParticlesPerSecond;
    size_t _maxNumberOfParticles;

    Point2D _origin;
    Vector2D _direction;
    double _speed;
    double _spreadAngleInRadians;

    //!
    //! \brief      Emits particles to the particle system data.
    //!
    //! \param[in]  currentTimeInSeconds    Current simulation time.
    //! \param[in]  timeIntervalInSeconds   The time-step interval.
    //!
    void onUpdate(double currentTimeInSeconds, double timeIntervalInSeconds) override;

    void emit(Array1<Point2D>* newPositions, Array1<Vector2D>* newVelocities, size_t maxNewNumberOfParticles);

    double random();
};

//! Shared pointer for the PointParticleEmitter2 type.
typedef std::shared_ptr<PointParticleEmitter2> PointParticleEmitter2Ptr;

//!
//! \brief Front-end to create PointParticleEmitter2 objects step by step.
//!
class PointParticleEmitter2::Builder final {
public:
    //! Returns builder with origin.
    Builder& withOrigin(const Point2D& origin);

    //! Returns builder with direction.
    Builder& withDirection(const Vector2D& direction);

    //! Returns builder with speed.
    Builder& withSpeed(double speed);

    //! Returns builder with spread angle in degrees.
    Builder& withSpreadAngleInDegrees(double spreadAngleInDegrees);

    Builder& withMaxNumberOfNewParticlesPerSecond(size_t maxNumOfNewParticlesPerSec);

    //! Returns builder with max number of particles.
    Builder& withMaxNumberOfParticles(size_t maxNumberOfParticles);

    //! Returns builder with random seed.
    Builder& withRandomSeed(uint32_t seed);

    //! Builds PointParticleEmitter2.
    [[nodiscard]] PointParticleEmitter2 build() const;

    //! Builds shared pointer of PointParticleEmitter2 instance.
    [[nodiscard]] PointParticleEmitter2Ptr makeShared() const;

private:
    size_t _maxNumberOfNewParticlesPerSecond = 1;
    size_t _maxNumberOfParticles = kMaxSize;
    Point2D _origin{0, 0};
    Vector2D _direction{0, 1};
    double _speed = 1.0;
    double _spreadAngleInDegrees = 90.0;
    uint32_t _seed = 0;
};

}  // namespace vox
