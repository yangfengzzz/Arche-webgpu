// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include "vox.base/constants.h"
#include "vox.geometry/blas.h"
#include "vox.geometry/cg.h"
#include "vox.math/matrix2x2.h"
#include "vox.math/vector2.h"

using namespace vox;

TEST(Cg, Solve) {
    // Solve:
    // | 4 1 | |x|   |1|
    // | 1 3 | |y| = |2|

    const Matrix2x2D matrix(4.0, 1.0, 1.0, 3.0);
    const Vector2D rhs(1.0, 2.0);

    typedef Blas<double, Vector2D, Matrix2x2D> BlasType;

    {
        // Zero iteration should give proper residual from iteration data.
        Vector2D x, r, d, q, s;
        unsigned int lastNumIter;
        double lastResidualNorm;

        cg<BlasType>(matrix, rhs, 0, 0.0, &x, &r, &d, &q, &s, &lastNumIter, &lastResidualNorm);

        EXPECT_DOUBLE_EQ(0.0, x.x);
        EXPECT_DOUBLE_EQ(0.0, x.y);

        EXPECT_DOUBLE_EQ(std::sqrt(5.0), lastResidualNorm);
        EXPECT_EQ(0u, lastNumIter);
    }
    {
        Vector2D x, r, d, q, s;
        unsigned int lastNumIter;
        double lastResidualNorm;

        cg<BlasType>(matrix, rhs, 10, 0.0, &x, &r, &d, &q, &s, &lastNumIter, &lastResidualNorm);

        EXPECT_DOUBLE_EQ(1.0 / 11.0, x.x);
        EXPECT_DOUBLE_EQ(7.0 / 11.0, x.y);

        EXPECT_LE(lastResidualNorm, kEpsilonD);
        EXPECT_LE(lastNumIter, 2u);
    }
}

TEST(Pcg, Solve) {
    // Solve:
    // | 4 1 | |x|   |1|
    // | 1 3 | |y| = |2|

    const Matrix2x2D matrix(4.0, 1.0, 1.0, 3.0);
    const Vector2D rhs(1.0, 2.0);

    typedef Blas<double, Vector2D, Matrix2x2D> BlasType;

    struct DiagonalPreconditioner {
        Vector2D precond;

        void build(const Matrix2x2D& matrix) {
            precond.x = matrix(0, 0);
            precond.y = matrix(1, 1);
        }

        void solve(const Vector2D& b, Vector2D* x) { *x = b / precond; }
    };

    {
        Vector2D x, r, d, q, s;
        unsigned int lastNumIter;
        double lastResidualNorm;
        DiagonalPreconditioner precond;

        precond.build(matrix);

        pcg<BlasType>(matrix, rhs, 10, 0.0, &precond, &x, &r, &d, &q, &s, &lastNumIter, &lastResidualNorm);

        EXPECT_DOUBLE_EQ(1.0 / 11.0, x.x);
        EXPECT_DOUBLE_EQ(7.0 / 11.0, x.y);

        EXPECT_LE(lastResidualNorm, kEpsilonD);
        EXPECT_LE(lastNumIter, 2u);
    }
}
