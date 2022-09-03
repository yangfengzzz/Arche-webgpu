//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <limits>
#include <memory>
#include <random>

#include "vox.geometry/implicit_surface3.h"
#include "vox.geometry/particle_emitter3.h"
#include "vox.geometry/point_generator3.h"
#include "vox.math/bounding_box3.h"

namespace vox {

//!
//! \brief 3-D volumetric particle emitter.
//!
//! This class emits particles from volumetric geometry.
//!
class VolumeParticleEmitter3 final : public ParticleEmitter3 {
public:
    class Builder;

    //!
    //! Constructs an emitter that spawns particles from given implicit surface
    //! which defines the volumetric geometry. Provided bounding box limits
    //! the particle generation region.
    //!
    //! \param[in]  implicitSurface         The implicit surface.
    //! \param[in]  maxRegion               The max region.
    //! \param[in]  spacing                 The spacing between particles.
    //! \param[in]  initialVel              The initial velocity.
    //! \param[in]  linearVel               The linear velocity of the emitter.
    //! \param[in]  angularVel              The angular velocity of the emitter.
    //! \param[in]  maxNumberOfParticles    The max number of particles to be
    //!                                     emitted.
    //! \param[in]  jitter                  The jitter amount between 0 and 1.
    //! \param[in]  isOneShot               True if emitter gets disabled after one shot.
    //! \param[in]  allowOverlapping        True if particles can be overlapped.
    //! \param[in]  seed                    The random seed.
    //!
    VolumeParticleEmitter3(ImplicitSurface3Ptr implicitSurface,
                           const BoundingBox3D& maxRegion,
                           double spacing,
                           const Vector3D& initialVel = Vector3D(),
                           const Vector3D& linearVel = Vector3D(),
                           const Vector3D& angularVel = Vector3D(),
                           size_t maxNumberOfParticles = kMaxSize,
                           double jitter = 0.0,
                           bool isOneShot = true,
                           bool allowOverlapping = false,
                           uint32_t seed = 0);

    //!
    //! \brief      Sets the point generator.
    //!
    //! This function sets the point generator that defines the pattern of the
    //! point distribution within the volume.
    //!
    //! \param[in]  newPointsGen The new points generator.
    //!
    void setPointGenerator(const PointGenerator3Ptr& newPointsGen);

    //! Returns source surface.
    [[nodiscard]] const ImplicitSurface3Ptr& surface() const;

    //! Sets the source surface.
    void setSurface(const ImplicitSurface3Ptr& newSurface);

    //! Returns max particle gen region.
    [[nodiscard]] const BoundingBox3D& maxRegion() const;

    //! Sets the max particle gen region.
    void setMaxRegion(const BoundingBox3D& newBox);

    //! Returns jitter amount.
    [[nodiscard]] double jitter() const;

    //! Sets jitter amount between 0 and 1.
    void setJitter(double newJitter);

    //! Returns true if particles should be emitted just once.
    [[nodiscard]] bool isOneShot() const;

    //!
    //! \brief      Sets the flag to true if particles are emitted just once.
    //!
    //! If true is set, the emitter will generate particles only once even after
    //! multiple emit calls. If false, it will keep generating particles from
    //! the volumetric geometry. Default value is true.
    //!
    //! \param[in]  newValue True if particles should be emitted just once.
    //!
    void setIsOneShot(bool newValue);

    //! Returns true if particles can be overlapped.
    [[nodiscard]] bool allowOverlapping() const;

    //!
    //! \brief      Sets the flag to true if particles can overlap each other.
    //!
    //! If true is set, the emitter will generate particles even if the new
    //! particles can find existing nearby particles within the particle
    //! spacing.
    //!
    //! \param[in]  newValue True if particles can be overlapped.
    //!
    void setAllowOverlapping(bool newValue);

    //! Returns max number of particles to be emitted.
    [[nodiscard]] size_t maxNumberOfParticles() const;

    //! Sets the max number of particles to be emitted.
    void setMaxNumberOfParticles(size_t newMaxNumberOfParticles);

    //! Returns the spacing between particles.
    [[nodiscard]] double spacing() const;

    //! Sets the spacing between particles.
    void setSpacing(double newSpacing);

    //! Sets the initial velocity of the particles.
    [[nodiscard]] Vector3D initialVelocity() const;

    //! Returns the initial velocity of the particles.
    void setInitialVelocity(const Vector3D& newInitialVel);

    //! Returns the linear velocity of the emitter.
    [[nodiscard]] Vector3D linearVelocity() const;

    //! Sets the linear velocity of the emitter.
    void setLinearVelocity(const Vector3D& newLinearVel);

    //! Returns the angular velocity of the emitter.
    [[nodiscard]] Vector3D angularVelocity() const;

    //! Sets the linear velocity of the emitter.
    void setAngularVelocity(const Vector3D& newAngularVel);

    //! Returns builder fox VolumeParticleEmitter3.
    static Builder builder();

private:
    std::mt19937 _rng;

    ImplicitSurface3Ptr _implicitSurface;
    BoundingBox3D _bounds;
    double _spacing;
    Vector3D _initialVel;
    Vector3D _linearVel;
    Vector3D _angularVel;
    PointGenerator3Ptr _pointsGen;

    size_t _maxNumberOfParticles = kMaxSize;
    size_t _numberOfEmittedParticles = 0;

    double _jitter = 0.0;
    bool _isOneShot = true;
    bool _allowOverlapping = false;

    //!
    //! \brief      Emits particles to the particle system data.
    //!
    //! \param[in]  currentTimeInSeconds    Current simulation time.
    //! \param[in]  timeIntervalInSeconds   The time-step interval.
    //!
    void onUpdate(double currentTimeInSeconds, double timeIntervalInSeconds) override;

    void emit(const ParticleSystemData3Ptr& particles, Array1<Point3D>* newPositions, Array1<Vector3D>* newVelocities);

    double random();

    [[nodiscard]] Vector3D velocityAt(const Point3D& point) const;
};

//! Shared pointer for the VolumeParticleEmitter3 type.
typedef std::shared_ptr<VolumeParticleEmitter3> VolumeParticleEmitter3Ptr;

//!
//! \brief Front-end to create VolumeParticleEmitter3 objects step by step.
//!
class VolumeParticleEmitter3::Builder final {
public:
    //! Returns builder with implicit surface defining volume shape.
    Builder& withImplicitSurface(const ImplicitSurface3Ptr& implicitSurface);

    //! Returns builder with surface defining volume shape.
    Builder& withSurface(const Surface3Ptr& surface);

    //! Returns builder with max region.
    Builder& withMaxRegion(const BoundingBox3D& bounds);

    //! Returns builder with spacing.
    Builder& withSpacing(double spacing);

    //! Returns builder with initial velocity.
    Builder& withInitialVelocity(const Vector3D& initialVel);

    //! Returns builder with linear velocity.
    Builder& withLinearVelocity(const Vector3D& linearVel);

    //! Returns builder with angular velocity.
    Builder& withAngularVelocity(const Vector3D& angularVel);

    //! Returns builder with max number of particles.
    Builder& withMaxNumberOfParticles(size_t maxNumberOfParticles);

    //! Returns builder with jitter amount.
    Builder& withJitter(double jitter);

    //! Returns builder with one-shot flag.
    Builder& withIsOneShot(bool isOneShot);

    //! Returns builder with overlapping flag.
    Builder& withAllowOverlapping(bool allowOverlapping);

    //! Returns builder with random seed.
    Builder& withRandomSeed(uint32_t seed);

    //! Builds VolumeParticleEmitter3.
    [[nodiscard]] VolumeParticleEmitter3 build() const;

    //! Builds shared pointer of VolumeParticleEmitter3 instance.
    [[nodiscard]] VolumeParticleEmitter3Ptr makeShared() const;

private:
    ImplicitSurface3Ptr _implicitSurface;
    bool _isBoundSet = false;
    BoundingBox3D _bounds;
    double _spacing = 0.1;
    Vector3D _initialVel;
    Vector3D _linearVel;
    Vector3D _angularVel;
    size_t _maxNumberOfParticles = kMaxSize;
    double _jitter = 0.0;
    bool _isOneShot = true;
    bool _allowOverlapping = false;
    uint32_t _seed = 0;
};

}  // namespace vox
