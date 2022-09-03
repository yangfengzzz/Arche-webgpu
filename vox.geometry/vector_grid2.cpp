// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include "vox.geometry/vector_grid2.h"

#include <algorithm>

#include "vox.geometry/array_samplers2.h"

using namespace vox;

VectorGrid2::VectorGrid2() = default;

VectorGrid2::~VectorGrid2() = default;

void VectorGrid2::clear() { resize(Size2(), gridSpacing(), origin(), Vector2D()); }

void VectorGrid2::resize(size_t resolutionX,
                         size_t resolutionY,
                         double gridSpacingX,
                         double gridSpacingY,
                         double originX,
                         double originY,
                         double initialValueX,
                         double initialValueY) {
    resize(Size2(resolutionX, resolutionY), Vector2D(gridSpacingX, gridSpacingY), Point2D(originX, originY),
           Vector2D(initialValueX, initialValueY));
}

void VectorGrid2::resize(const Size2 &resolution,
                         const Vector2D &gridSpacing,
                         const Point2D &origin,
                         const Vector2D &initialValue) {
    setSizeParameters(resolution, gridSpacing, origin);

    onResize(resolution, gridSpacing, origin, initialValue);
}

void VectorGrid2::resize(double gridSpacingX, double gridSpacingY, double originX, double originY) {
    resize(Vector2D(gridSpacingX, gridSpacingY), Point2D(originX, originY));
}

void VectorGrid2::resize(const Vector2D &gridSpacing, const Point2D &origin) {
    resize(resolution(), gridSpacing, origin);
}

VectorGridBuilder2::VectorGridBuilder2() = default;

VectorGridBuilder2::~VectorGridBuilder2() = default;
