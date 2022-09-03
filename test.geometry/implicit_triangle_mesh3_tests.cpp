// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include "unit_tests_utils.h"
#include "vox.geometry/box3.h"
#include "vox.geometry/implicit_triangle_mesh3.h"
#include "vox.geometry/sphere3.h"
#include "vox.geometry/surface_to_implicit3.h"

using namespace vox;

TEST(ImplicitTriangleMesh3, SignedDistance) {
    auto box = Box3::builder().withLowerCorner({0, 0, 0}).withUpperCorner({1, 1, 1}).makeShared();
    SurfaceToImplicit3 refSurf(box);

    std::ifstream objFile("./assets/Models/cube.obj");
    auto mesh = TriangleMesh3::builder().makeShared();
    mesh->readObj(&objFile);

    auto imesh = ImplicitTriangleMesh3::builder().withTriangleMesh(mesh).withResolutionX(20).makeShared();

    for (size_t i = 0; i < getNumberOfSamplePoints3(); ++i) {
        auto sample = getSamplePoints3()[i];
        auto refAns = refSurf.signedDistance(sample);
        auto actAns = imesh->signedDistance(sample);

        EXPECT_NEAR(refAns, actAns, 1.0 / 20);
    }
}
