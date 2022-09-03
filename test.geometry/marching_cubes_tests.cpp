// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "unit_tests_utils.h"
#include "vox.geometry/array3.h"
#include "vox.geometry/marching_cubes.h"
#include "vox.geometry/vertex_centered_scalar_grid3.h"

using namespace vox;

TEST(MarchingCubes, Connectivity) {
    TriangleMesh3 triMesh;

    Array3<double> grid(2, 2, 2);
    grid(0, 0, 0) = -0.5;
    grid(0, 0, 1) = -0.5;
    grid(0, 1, 0) = 0.5;
    grid(0, 1, 1) = 0.5;
    grid(1, 0, 0) = -0.5;
    grid(1, 0, 1) = -0.5;
    grid(1, 1, 0) = 0.5;
    grid(1, 1, 1) = 0.5;

    marchingCubes(grid.constAccessor(), Vector3D(1, 1, 1), Point3D(), &triMesh, 0, kDirectionAll, kDirectionNone);
    EXPECT_EQ(24u, triMesh.numberOfPoints());

    triMesh.clear();
    marchingCubes(grid.constAccessor(), Vector3D(1, 1, 1), Point3D(), &triMesh, 0, kDirectionAll, kDirectionBack);
    EXPECT_EQ(22u, triMesh.numberOfPoints());

    triMesh.clear();
    marchingCubes(grid.constAccessor(), Vector3D(1, 1, 1), Point3D(), &triMesh, 0, kDirectionAll, kDirectionFront);
    EXPECT_EQ(22u, triMesh.numberOfPoints());

    triMesh.clear();
    marchingCubes(grid.constAccessor(), Vector3D(1, 1, 1), Point3D(), &triMesh, 0, kDirectionAll, kDirectionLeft);
    EXPECT_EQ(22u, triMesh.numberOfPoints());

    triMesh.clear();
    marchingCubes(grid.constAccessor(), Vector3D(1, 1, 1), Point3D(), &triMesh, 0, kDirectionAll, kDirectionRight);
    EXPECT_EQ(22u, triMesh.numberOfPoints());

    triMesh.clear();
    marchingCubes(grid.constAccessor(), Vector3D(1, 1, 1), Point3D(), &triMesh, 0, kDirectionAll, kDirectionDown);
    EXPECT_EQ(24u, triMesh.numberOfPoints());

    triMesh.clear();
    marchingCubes(grid.constAccessor(), Vector3D(1, 1, 1), Point3D(), &triMesh, 0, kDirectionAll, kDirectionUp);
    EXPECT_EQ(24u, triMesh.numberOfPoints());

    triMesh.clear();
    marchingCubes(grid.constAccessor(), Vector3D(1, 1, 1), Point3D(), &triMesh, 0, kDirectionAll, kDirectionAll);
    EXPECT_EQ(8u, triMesh.numberOfPoints());
}
