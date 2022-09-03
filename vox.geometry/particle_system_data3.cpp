// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include "vox.geometry/particle_system_data3.h"

#include <algorithm>
#include <vector>

#include "vox.base/logging.h"
#include "vox.base/parallel.h"
#include "vox.base/timer.h"
#include "vox.geometry/point_parallel_hash_grid_searcher3.h"

using namespace vox;

static const size_t kDefaultHashGridResolution = 64;

ParticleSystemData3::ParticleSystemData3() : ParticleSystemData3(0) {}

ParticleSystemData3::ParticleSystemData3(size_t numberOfParticles) {
    _velocityIdx = addVectorData();
    _forceIdx = addVectorData();

    // Use PointParallelHashGridSearcher3 by default
    _neighborSearcher = std::make_shared<PointParallelHashGridSearcher3>(
            kDefaultHashGridResolution, kDefaultHashGridResolution, kDefaultHashGridResolution, 2.0 * _radius);

    resize(numberOfParticles);
}

ParticleSystemData3::ParticleSystemData3(const ParticleSystemData3& other) { set(other); }

ParticleSystemData3::~ParticleSystemData3() = default;

void ParticleSystemData3::resize(size_t newNumberOfParticles) {
    _numberOfParticles = newNumberOfParticles;

    for (auto& attr : _scalarDataList) {
        attr.resize(newNumberOfParticles, 0.0);
    }

    for (auto& attr : _vectorDataList) {
        attr.resize(newNumberOfParticles, Vector3D());
    }
    _positionData.resize(newNumberOfParticles, Point3D());
}

size_t ParticleSystemData3::numberOfParticles() const { return _numberOfParticles; }

size_t ParticleSystemData3::addScalarData(double initialVal) {
    size_t attrIdx = _scalarDataList.size();
    _scalarDataList.emplace_back(numberOfParticles(), initialVal);
    return attrIdx;
}

size_t ParticleSystemData3::addVectorData(const Vector3D& initialVal) {
    size_t attrIdx = _vectorDataList.size();
    _vectorDataList.emplace_back(numberOfParticles(), initialVal);
    return attrIdx;
}

double ParticleSystemData3::radius() const { return _radius; }

void ParticleSystemData3::setRadius(double newRadius) { _radius = std::max(newRadius, 0.0); }

double ParticleSystemData3::mass() const { return _mass; }

void ParticleSystemData3::setMass(double newMass) { _mass = std::max(newMass, 0.0); }

ConstArrayAccessor1<Point3D> ParticleSystemData3::positions() const { return _positionData.constAccessor(); }

ArrayAccessor1<Point3D> ParticleSystemData3::positions() { return _positionData.accessor(); }

ConstArrayAccessor1<Vector3D> ParticleSystemData3::velocities() const { return vectorDataAt(_velocityIdx); }

ArrayAccessor1<Vector3D> ParticleSystemData3::velocities() { return vectorDataAt(_velocityIdx); }

ConstArrayAccessor1<Vector3D> ParticleSystemData3::forces() const { return vectorDataAt(_forceIdx); }

ArrayAccessor1<Vector3D> ParticleSystemData3::forces() { return vectorDataAt(_forceIdx); }

ConstArrayAccessor1<double> ParticleSystemData3::scalarDataAt(size_t idx) const {
    return _scalarDataList[idx].constAccessor();
}

ArrayAccessor1<double> ParticleSystemData3::scalarDataAt(size_t idx) { return _scalarDataList[idx].accessor(); }

ConstArrayAccessor1<Vector3D> ParticleSystemData3::vectorDataAt(size_t idx) const {
    return _vectorDataList[idx].constAccessor();
}

ArrayAccessor1<Vector3D> ParticleSystemData3::vectorDataAt(size_t idx) { return _vectorDataList[idx].accessor(); }

void ParticleSystemData3::addParticle(const Point3D& newPosition,
                                      const Vector3D& newVelocity,
                                      const Vector3D& newForce) {
    Array1<Point3D> newPositions = {newPosition};
    Array1<Vector3D> newVelocities = {newVelocity};
    Array1<Vector3D> newForces = {newForce};

    addParticles(newPositions.constAccessor(), newVelocities.constAccessor(), newForces.constAccessor());
}

void ParticleSystemData3::addParticles(const ConstArrayAccessor1<Point3D>& newPositions,
                                       const ConstArrayAccessor1<Vector3D>& newVelocities,
                                       const ConstArrayAccessor1<Vector3D>& newForces) {
    VOX_THROW_INVALID_ARG_IF(newVelocities.size() > 0 && newVelocities.size() != newPositions.size())
    VOX_THROW_INVALID_ARG_IF(newForces.size() > 0 && newForces.size() != newPositions.size())

    size_t oldNumberOfParticles = numberOfParticles();
    size_t newNumberOfParticles = oldNumberOfParticles + newPositions.size();

    resize(newNumberOfParticles);

    auto pos = positions();
    auto vel = velocities();
    auto frc = forces();

    parallelFor(kZeroSize, newPositions.size(), [&](size_t i) { pos[i + oldNumberOfParticles] = newPositions[i]; });

    if (newVelocities.size() > 0) {
        parallelFor(kZeroSize, newPositions.size(),
                    [&](size_t i) { vel[i + oldNumberOfParticles] = newVelocities[i]; });
    }

    if (newForces.size() > 0) {
        parallelFor(kZeroSize, newPositions.size(), [&](size_t i) { frc[i + oldNumberOfParticles] = newForces[i]; });
    }
}

const PointNeighborSearcher3Ptr& ParticleSystemData3::neighborSearcher() const { return _neighborSearcher; }

void ParticleSystemData3::setNeighborSearcher(const PointNeighborSearcher3Ptr& newNeighborSearcher) {
    _neighborSearcher = newNeighborSearcher;
}

const std::vector<std::vector<size_t>>& ParticleSystemData3::neighborLists() const { return _neighborLists; }

void ParticleSystemData3::buildNeighborSearcher(double maxSearchRadius) {
    utility::Timer timer;

    // Use PointParallelHashGridSearcher3 by default
    _neighborSearcher = std::make_shared<PointParallelHashGridSearcher3>(
            kDefaultHashGridResolution, kDefaultHashGridResolution, kDefaultHashGridResolution, 2.0 * maxSearchRadius);

    _neighborSearcher->build(positions());

    LOGI("Building neighbor searcher took: {} seconds", timer.Elapsed())
}

void ParticleSystemData3::buildNeighborLists(double maxSearchRadius) {
    utility::Timer timer;

    _neighborLists.resize(numberOfParticles());

    auto points = positions();
    for (size_t i = 0; i < numberOfParticles(); ++i) {
        Point3D origin = points[i];
        _neighborLists[i].clear();

        _neighborSearcher->forEachNearbyPoint(origin, maxSearchRadius, [&](size_t j, const Point3D&) {
            if (i != j) {
                _neighborLists[i].push_back(j);
            }
        });
    }

    LOGI("Building neighbor list took: {} seconds", timer.Elapsed())
}

void ParticleSystemData3::set(const ParticleSystemData3& other) {
    _radius = other._radius;
    _mass = other._mass;
    _velocityIdx = other._velocityIdx;
    _forceIdx = other._forceIdx;
    _numberOfParticles = other._numberOfParticles;

    for (auto& attr : other._scalarDataList) {
        _scalarDataList.emplace_back(attr);
    }

    for (auto& attr : other._vectorDataList) {
        _vectorDataList.emplace_back(attr);
    }

    _positionData = other._positionData;

    _neighborSearcher = other._neighborSearcher->clone();
    _neighborLists = other._neighborLists;
}

ParticleSystemData3& ParticleSystemData3::operator=(const ParticleSystemData3& other) {
    set(other);
    return *this;
}
