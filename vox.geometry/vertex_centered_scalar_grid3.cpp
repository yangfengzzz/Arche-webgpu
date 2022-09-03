// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/vertex_centered_scalar_grid3.h"

#include <utility>

#include "vox.base/private_helpers.h"

using namespace vox;

VertexCenteredScalarGrid3::VertexCenteredScalarGrid3() = default;

VertexCenteredScalarGrid3::VertexCenteredScalarGrid3(size_t resolutionX,
                                                     size_t resolutionY,
                                                     size_t resolutionZ,
                                                     double gridSpacingX,
                                                     double gridSpacingY,
                                                     double gridSpacingZ,
                                                     double originX,
                                                     double originY,
                                                     double originZ,
                                                     double initialValue) {
    resize(resolutionX, resolutionY, resolutionZ, gridSpacingX, gridSpacingY, gridSpacingZ, originX, originY, originZ,
           initialValue);
}

VertexCenteredScalarGrid3::VertexCenteredScalarGrid3(const Size3 &resolution,
                                                     const Vector3D &gridSpacing,
                                                     const Point3D &origin,
                                                     double initialValue) {
    resize(resolution, gridSpacing, origin, initialValue);
}

VertexCenteredScalarGrid3::VertexCenteredScalarGrid3(const VertexCenteredScalarGrid3 &other) { set(other); }

Size3 VertexCenteredScalarGrid3::dataSize() const {
    if (resolution() != Size3(0, 0, 0)) {
        return resolution() + Size3(1, 1, 1);
    } else {
        return {0, 0, 0};
    }
}

Point3D VertexCenteredScalarGrid3::dataOrigin() const { return origin(); }

std::shared_ptr<ScalarGrid3> VertexCenteredScalarGrid3::clone() const {
    return CLONE_W_CUSTOM_DELETER(VertexCenteredScalarGrid3)
}

void VertexCenteredScalarGrid3::swap(Grid3 *other) {
    auto *sameType = dynamic_cast<VertexCenteredScalarGrid3 *>(other);
    if (sameType != nullptr) {
        swapScalarGrid(sameType);
    }
}

void VertexCenteredScalarGrid3::set(const VertexCenteredScalarGrid3 &other) { setScalarGrid(other); }

VertexCenteredScalarGrid3 &VertexCenteredScalarGrid3::operator=(const VertexCenteredScalarGrid3 &other) {
    set(other);
    return *this;
}

VertexCenteredScalarGrid3::Builder VertexCenteredScalarGrid3::builder() { return {}; }

VertexCenteredScalarGrid3::Builder &VertexCenteredScalarGrid3::Builder::withResolution(const Size3 &resolution) {
    _resolution = resolution;
    return *this;
}

VertexCenteredScalarGrid3::Builder &VertexCenteredScalarGrid3::Builder::withResolution(size_t resolutionX,
                                                                                       size_t resolutionY,
                                                                                       size_t resolutionZ) {
    _resolution.x = resolutionX;
    _resolution.y = resolutionY;
    _resolution.z = resolutionZ;
    return *this;
}

VertexCenteredScalarGrid3::Builder &VertexCenteredScalarGrid3::Builder::withGridSpacing(const Vector3D &gridSpacing) {
    _gridSpacing = gridSpacing;
    return *this;
}

VertexCenteredScalarGrid3::Builder &VertexCenteredScalarGrid3::Builder::withGridSpacing(double gridSpacingX,
                                                                                        double gridSpacingY,
                                                                                        double gridSpacingZ) {
    _gridSpacing.x = gridSpacingX;
    _gridSpacing.y = gridSpacingY;
    _gridSpacing.z = gridSpacingZ;
    return *this;
}

VertexCenteredScalarGrid3::Builder &VertexCenteredScalarGrid3::Builder::withOrigin(const Point3D &gridOrigin) {
    _gridOrigin = gridOrigin;
    return *this;
}

VertexCenteredScalarGrid3::Builder &VertexCenteredScalarGrid3::Builder::withOrigin(double gridOriginX,
                                                                                   double gridOriginY,
                                                                                   double gridOriginZ) {
    _gridOrigin.x = gridOriginX;
    _gridOrigin.y = gridOriginY;
    _gridOrigin.z = gridOriginZ;
    return *this;
}

VertexCenteredScalarGrid3::Builder &VertexCenteredScalarGrid3::Builder::withInitialValue(double initialVal) {
    _initialVal = initialVal;
    return *this;
}

VertexCenteredScalarGrid3 VertexCenteredScalarGrid3::Builder::build() const {
    return VertexCenteredScalarGrid3(_resolution, _gridSpacing, _gridOrigin, _initialVal);
}

VertexCenteredScalarGrid3Ptr VertexCenteredScalarGrid3::Builder::makeShared() const {
    return {new VertexCenteredScalarGrid3(_resolution, _gridSpacing, _gridOrigin, _initialVal),
            [](VertexCenteredScalarGrid3 *obj) { delete obj; }};
}

ScalarGrid3Ptr VertexCenteredScalarGrid3::Builder::build(const Size3 &resolution,
                                                         const Vector3D &gridSpacing,
                                                         const Point3D &gridOrigin,
                                                         double initialVal) const {
    return std::shared_ptr<VertexCenteredScalarGrid3>(
            new VertexCenteredScalarGrid3(resolution, gridSpacing, gridOrigin, initialVal),
            [](VertexCenteredScalarGrid3 *obj) { delete obj; });
}
