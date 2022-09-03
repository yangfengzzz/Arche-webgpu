// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/implicit_triangle_mesh3.h"

#include "vox.geometry/triangle_mesh_to_sdf.h"

using namespace vox;

ImplicitTriangleMesh3::ImplicitTriangleMesh3(const TriangleMesh3Ptr& mesh,
                                             size_t resolutionX,
                                             double margin,
                                             const Transform3D& transform,
                                             bool isNormalFlipped)
    : ImplicitSurface3(transform, isNormalFlipped), _mesh(mesh) {
    if (mesh->numberOfTriangles() > 0 && mesh->numberOfPoints() > 0) {
        BoundingBox3D box = _mesh->boundingBox();
        Vector3D scale(box.width(), box.height(), box.depth());
        box.lower_corner -= margin * scale;
        box.upper_corner += margin * scale;
        auto resolutionY = static_cast<size_t>(std::ceil(resolutionX * box.height() / box.width()));
        auto resolutionZ = static_cast<size_t>(std::ceil(resolutionX * box.depth() / box.width()));

        double dx = box.width() / resolutionX;

        _grid = std::make_shared<VertexCenteredScalarGrid3>();
        _grid->resize(resolutionX, resolutionY, resolutionZ, dx, dx, dx, box.lower_corner.x, box.lower_corner.y,
                      box.lower_corner.z);

        triangleMeshToSdf(*_mesh, _grid.get());

        _customImplicitSurface =
                CustomImplicitSurface3::builder()
                        .withSignedDistanceFunction([&](const Point3D& pt) -> double { return _grid->sample(pt); })
                        .withDomain(_grid->boundingBox())
                        .withResolution(dx)
                        .makeShared();
    } else {
        // Empty mesh -- return big/uniform number
        _customImplicitSurface = CustomImplicitSurface3::builder()
                                         .withSignedDistanceFunction([&](const Point3D&) -> double { return kMaxD; })
                                         .makeShared();
    }
}

ImplicitTriangleMesh3::~ImplicitTriangleMesh3() = default;

Point3D ImplicitTriangleMesh3::closestPointLocal(const Point3D& otherPoint) const {
    return _customImplicitSurface->closestPoint(otherPoint);
}

double ImplicitTriangleMesh3::closestDistanceLocal(const Point3D& otherPoint) const {
    return _customImplicitSurface->closestDistance(otherPoint);
}

bool ImplicitTriangleMesh3::intersectsLocal(const Ray3D& ray) const { return _customImplicitSurface->intersects(ray); }

BoundingBox3D ImplicitTriangleMesh3::boundingBoxLocal() const { return _mesh->boundingBox(); }

Vector3D ImplicitTriangleMesh3::closestNormalLocal(const Point3D& otherPoint) const {
    return _customImplicitSurface->closestNormal(otherPoint);
}

double ImplicitTriangleMesh3::signedDistanceLocal(const Point3D& otherPoint) const {
    return _customImplicitSurface->signedDistance(otherPoint);
}

SurfaceRayIntersection3 ImplicitTriangleMesh3::closestIntersectionLocal(const Ray3D& ray) const {
    return _customImplicitSurface->closestIntersection(ray);
}

ImplicitTriangleMesh3::Builder ImplicitTriangleMesh3::builder() { return {}; }

const VertexCenteredScalarGrid3Ptr& ImplicitTriangleMesh3::grid() const { return _grid; }

ImplicitTriangleMesh3::Builder& ImplicitTriangleMesh3::Builder::withTriangleMesh(const TriangleMesh3Ptr& mesh) {
    _mesh = mesh;
    return *this;
}

ImplicitTriangleMesh3::Builder& ImplicitTriangleMesh3::Builder::withResolutionX(size_t resolutionX) {
    _resolutionX = resolutionX;
    return *this;
}

ImplicitTriangleMesh3::Builder& ImplicitTriangleMesh3::Builder::withMargin(double margin) {
    _margin = margin;
    return *this;
}

ImplicitTriangleMesh3 ImplicitTriangleMesh3::Builder::build() const {
    return ImplicitTriangleMesh3(_mesh, _resolutionX, _margin, _transform, _isNormalFlipped);
}

ImplicitTriangleMesh3Ptr ImplicitTriangleMesh3::Builder::makeShared() const {
    return {new ImplicitTriangleMesh3(_mesh, _resolutionX, _margin, _transform, _isNormalFlipped),
            [](ImplicitTriangleMesh3* obj) { delete obj; }};
}
