// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include "vox.geometry/svd.h"

using namespace vox;

TEST(Svd, Float) {
    MatrixMxNF a{{0, 1}, {1, 1}, {1, 0}};

    MatrixMxNF u, v;
    VectorNF w;

    svd(a, u, w, v);

    MatrixMxNF w2(2, 2);
    w2(0, 0) = w[0];
    w2(1, 1) = w[1];

    MatrixMxNF aApprox = u * w2 * v.transposed();
    EXPECT_TRUE(a.isSimilar(aApprox, 1e-6));
}

TEST(Svd, Double) {
    MatrixMxND a{{0, 1}, {1, 1}, {1, 0}};

    MatrixMxND u, v;
    VectorND w;

    svd(a, u, w, v);

    MatrixMxND w2(2, 2);
    w2(0, 0) = w[0];
    w2(1, 1) = w[1];

    MatrixMxND aApprox = u * w2 * v.transposed();
    EXPECT_TRUE(a.isSimilar(aApprox, 1e-12));
}
