// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include "fdm_linear_system_solver_test_helper3.h"
#include "vox.geometry/fdm_jacobi_solver3.h"

using namespace vox;

TEST(FdmJacobiSolver3, Solve) {
    FdmLinearSystem3 system;
    FdmLinearSystemSolverTestHelper3::buildTestLinearSystem(&system, {3, 3, 3});

    FdmJacobiSolver3 solver(100, 10, 1e-9);
    solver.solve(&system);

    EXPECT_GT(solver.tolerance(), solver.lastResidual());
}

TEST(FdmJacobiSolver3, SolveCompressed) {
    FdmCompressedLinearSystem3 system;
    FdmLinearSystemSolverTestHelper3::buildTestCompressedLinearSystem(&system, {3, 3, 3});

    FdmJacobiSolver3 solver(100, 10, 1e-9);
    solver.solveCompressed(&system);

    EXPECT_GT(solver.tolerance(), solver.lastResidual());
}
