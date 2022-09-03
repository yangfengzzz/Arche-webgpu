// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include "vox.geometry/vector_grid3.h"

#include <algorithm>

using namespace vox;

VectorGrid3::VectorGrid3() = default;

VectorGrid3::~VectorGrid3() = default;

void VectorGrid3::clear() { resize(Size3(), gridSpacing(), origin(), Vector3D()); }

void VectorGrid3::resize(size_t resolutionX,
                         size_t resolutionY,
                         size_t resolutionZ,
                         double gridSpacingX,
                         double gridSpacingY,
                         double gridSpacingZ,
                         double originX,
                         double originY,
                         double originZ,
                         double initialValueX,
                         double initialValueY,
                         double initialValueZ) {
    resize(Size3(resolutionX, resolutionY, resolutionZ), Vector3D(gridSpacingX, gridSpacingY, gridSpacingZ),
           Point3D(originX, originY, originZ), Vector3D(initialValueX, initialValueY, initialValueZ));
}

void VectorGrid3::resize(const Size3 &resolution,
                         const Vector3D &gridSpacing,
                         const Point3D &origin,
                         const Vector3D &initialValue) {
    setSizeParameters(resolution, gridSpacing, origin);

    onResize(resolution, gridSpacing, origin, initialValue);
}

void VectorGrid3::resize(
        double gridSpacingX, double gridSpacingY, double gridSpacingZ, double originX, double originY, double originZ) {
    resize(Vector3D(gridSpacingX, gridSpacingY, gridSpacingZ), Point3D(originX, originY, originZ));
}

void VectorGrid3::resize(const Vector3D &gridSpacing, const Point3D &origin) {
    resize(resolution(), gridSpacing, origin);
}

VectorGridBuilder3::VectorGridBuilder3() = default;

VectorGridBuilder3::~VectorGridBuilder3() = default;
