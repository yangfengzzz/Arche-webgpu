//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "test.simd_math/gtest_helper.h"
#include "test.simd_math/gtest_math_helper.h"
#include "vox.base/constants.h"
#include "vox.simd_math/simd_quaternion.h"

using vox::math::SimdQuaternion;

TEST(QuaternionConstant, vox_simd_math) { EXPECT_SIMDQUATERNION_EQ(SimdQuaternion::identity(), 0.f, 0.f, 0.f, 1.f); }

TEST(QuaternionArithmetic, vox_simd_math) {
    const SimdQuaternion a = {vox::math::simd_float4::Load(.70710677f, 0.f, 0.f, .70710677f)};
    const SimdQuaternion b = {vox::math::simd_float4::Load(0.f, .70710677f, 0.f, .70710677f)};
    const SimdQuaternion c = {vox::math::simd_float4::Load(0.f, .70710677f, 0.f, -.70710677f)};
    const SimdQuaternion denorm = {vox::math::simd_float4::Load(1.414212f, 0.f, 0.f, 1.414212f)};
    const SimdQuaternion zero = {vox::math::simd_float4::zero()};

    EXPECT_SIMDINT_EQ(IsNormalized(a), 0xffffffff, 0, 0, 0);
    EXPECT_SIMDINT_EQ(IsNormalized(b), 0xffffffff, 0, 0, 0);
    EXPECT_SIMDINT_EQ(IsNormalized(c), 0xffffffff, 0, 0, 0);
    EXPECT_SIMDINT_EQ(IsNormalized(denorm), 0, 0, 0, 0);

    const SimdQuaternion conjugate = Conjugate(a);
    EXPECT_SIMDQUATERNION_EQ(conjugate, -.70710677f, 0.f, 0.f, .70710677f);

    const SimdQuaternion negate = -a;
    EXPECT_SIMDQUATERNION_EQ(negate, -.70710677f, 0.f, 0.f, -.70710677f);

    const SimdQuaternion mul0 = a * conjugate;
    EXPECT_SIMDQUATERNION_EQ(mul0, 0.f, 0.f, 0.f, 1.f);

    const SimdQuaternion mul1 = conjugate * a;
    EXPECT_SIMDQUATERNION_EQ(mul1, 0.f, 0.f, 0.f, 1.f);

    const SimdQuaternion q1234 = {vox::math::simd_float4::Load(1.f, 2.f, 3.f, 4.f)};
    const SimdQuaternion q5678 = {vox::math::simd_float4::Load(5.f, 6.f, 7.f, 8.f)};
    const SimdQuaternion mul12345678 = q1234 * q5678;
    EXPECT_SIMDQUATERNION_EQ(mul12345678, 24.f, 48.f, 48.f, -6.f);

    EXPECT_ASSERTION(Normalize(zero), "is not normalizable");
    const SimdQuaternion norm = Normalize(denorm);
    EXPECT_SIMDINT_EQ(IsNormalized(norm), 0xffffffff, 0, 0, 0);
    EXPECT_SIMDQUATERNION_EQ(norm, .7071068f, 0.f, 0.f, .7071068f);

    // EXPECT_ASSERTION(NormalizeSafe(denorm, zero), "_safer is not normalized");
    const SimdQuaternion norm_safe = NormalizeSafe(denorm, b);
    EXPECT_SIMDINT_EQ(IsNormalized(norm_safe), 0xffffffff, 0, 0, 0);
    EXPECT_SIMDQUATERNION_EQ(norm_safe, .7071068f, 0.f, 0.f, .7071068f);
    const SimdQuaternion norm_safer = NormalizeSafe(zero, b);
    EXPECT_SIMDINT_EQ(IsNormalized(norm_safer), 0xffffffff, 0, 0, 0);
    EXPECT_SIMDQUATERNION_EQ(norm_safer, 0.f, .70710677f, 0.f, .70710677f);

    EXPECT_ASSERTION(NormalizeEst(zero), "is not normalizable");
    const SimdQuaternion norm_est = NormalizeEst(denorm);
    EXPECT_SIMDINT_EQ(IsNormalizedEst(norm_est), 0xffffffff, 0, 0, 0);
    EXPECT_SIMDQUATERNION_EQ_EST(norm_est, .7071068f, 0.f, 0.f, .7071068f);

    // EXPECT_ASSERTION(NormalizeSafe(denorm, zero), "_safer is not normalized");
    const SimdQuaternion norm_safe_est = NormalizeSafeEst(denorm, b);
    EXPECT_SIMDINT_EQ(IsNormalizedEst(norm_safe_est), 0xffffffff, 0, 0, 0);
    EXPECT_SIMDQUATERNION_EQ_EST(norm_safe_est, .7071068f, 0.f, 0.f, .7071068f);
    const SimdQuaternion norm_safer_est = NormalizeSafeEst(zero, b);
    EXPECT_SIMDINT_EQ(IsNormalizedEst(norm_safer_est), 0xffffffff, 0, 0, 0);
    EXPECT_SIMDQUATERNION_EQ_EST(norm_safer_est, 0.f, .70710677f, 0.f, .70710677f);
}

TEST(QuaternionFromVectors, vox_simd_math) {
    // Returns identity for a 0 length vector
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::zero(), vox::math::simd_float4::x_axis()), 0.f, 0.f,
            0.f, 1.f);

    // pi/2 around y
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::z_axis(), vox::math::simd_float4::x_axis()), 0.f,
            0.707106769f, 0.f, 0.707106769f);

    // Non unit pi/2 around y
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::z_axis(),
                                        vox::math::simd_float4::x_axis() * vox::math::simd_float4::Load1(27.f)),
            0.f, 0.707106769f, 0.f, 0.707106769f);

    // Minus pi/2 around y
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::x_axis(), vox::math::simd_float4::z_axis()), 0.f,
            -0.707106769f, 0.f, 0.707106769f);

    // pi/2 around x
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::y_axis(), vox::math::simd_float4::z_axis()),
            0.707106769f, 0.f, 0.f, 0.707106769f);

    // pi/2 around z
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::x_axis(), vox::math::simd_float4::y_axis()), 0.f, 0.f,
            0.707106769f, 0.707106769f);

    // pi/2 around z also
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::Load(0.707106769f, 0.707106769f, 0.f, 99.f),
                                        vox::math::simd_float4::Load(-0.707106769f, 0.707106769f, 0.f, 93.f)),
            0.f, 0.f, 0.707106769f, 0.707106769f);

    // Non unit pi/2 around z also
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::Load(0.707106769f, 0.707106769f, 0.f, 99.f) *
                                                vox::math::simd_float4::Load1(9.f),
                                        vox::math::simd_float4::Load(-0.707106769f, 0.707106769f, 0.f, 93.f) *
                                                vox::math::simd_float4::Load1(46.f)),
            0.f, 0.f, 0.707106769f, 0.707106769f);

    // Non-unit pi/2 around z
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::x_axis(),
                                        vox::math::simd_float4::y_axis() * vox::math::simd_float4::Load1(2.f)),
            0.f, 0.f, 0.707106769f, 0.707106769f);

    // Aligned vectors
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::x_axis(), vox::math::simd_float4::x_axis()), 0.f, 0.f,
            0.f, 1.f);

    // Non-unit aligned vectors
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::x_axis(),
                                        vox::math::simd_float4::x_axis() * vox::math::simd_float4::Load1(2.f)),
            0.f, 0.f, 0.f, 1.f);

    // Opposed vectors
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::x_axis(), -vox::math::simd_float4::x_axis()), 0.f, 1.f,
            0.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(-vox::math::simd_float4::x_axis(), vox::math::simd_float4::x_axis()), 0.f, -1.f,
            0.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::y_axis(), -vox::math::simd_float4::y_axis()), 0.f, 0.f,
            1.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(-vox::math::simd_float4::y_axis(), vox::math::simd_float4::y_axis()), 0.f, 0.f,
            -1.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::z_axis(), -vox::math::simd_float4::z_axis()), 0.f, -1.f,
            0.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(-vox::math::simd_float4::z_axis(), vox::math::simd_float4::z_axis()), 0.f, 1.f,
            0.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::Load(0.707106769f, 0.707106769f, 0.f, 93.f),
                                        -vox::math::simd_float4::Load(0.707106769f, 0.707106769f, 0.f, 99.f)),
            -0.707106769f, 0.707106769f, 0.f, 0.f);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromVectors(vox::math::simd_float4::Load(0.f, 0.707106769f, 0.707106769f, 93.f),
                                        -vox::math::simd_float4::Load(0.f, 0.707106769f, 0.707106769f, 99.f)),
            0.f, -0.707106769f, 0.707106769f, 0.f);

    // Non-unit opposed vectors
    EXPECT_SIMDQUATERNION_EQ(SimdQuaternion::FromVectors(vox::math::simd_float4::Load(2.f, 2.f, 2.f, 0.f),
                                                         vox::math::simd_float4::Load(-2.f, -2.f, -2.f, 0.f)),
                             0.f, -0.707106769f, 0.707106769f, 0);
}

TEST(QuaternionFromUnitVectors, vox_simd_math) {
    // assert 0 length vectors
    EXPECT_ASSERTION(SimdQuaternion::FromUnitVectors(vox::math::simd_float4::zero(), vox::math::simd_float4::x_axis()),
                     "Input vectors must be normalized.");
    EXPECT_ASSERTION(SimdQuaternion::FromUnitVectors(vox::math::simd_float4::x_axis(), vox::math::simd_float4::zero()),
                     "Input vectors must be normalized.");
    // assert non unit vectors
    EXPECT_ASSERTION(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::x_axis() * vox::math::simd_float4::Load1(2.f),
                                            vox::math::simd_float4::x_axis()),
            "Input vectors must be normalized.");
    EXPECT_ASSERTION(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::x_axis(),
                                            vox::math::simd_float4::x_axis() * vox::math::simd_float4::Load1(.5f)),
            "Input vectors must be normalized.");

    // pi/2 around y
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::z_axis(), vox::math::simd_float4::x_axis()), 0.f,
            0.707106769f, 0.f, 0.707106769f);

    // Minus pi/2 around y
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::x_axis(), vox::math::simd_float4::z_axis()), 0.f,
            -0.707106769f, 0.f, 0.707106769f);

    // pi/2 around x
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::y_axis(), vox::math::simd_float4::z_axis()),
            0.707106769f, 0.f, 0.f, 0.707106769f);

    // pi/2 around z
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::x_axis(), vox::math::simd_float4::y_axis()), 0.f,
            0.f, 0.707106769f, 0.707106769f);

    // pi/2 around z also
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::Load(0.707106769f, 0.707106769f, 0.f, 99.f),
                                            vox::math::simd_float4::Load(-0.707106769f, 0.707106769f, 0.f, 93.f)),
            0.f, 0.f, 0.707106769f, 0.707106769f);

    // Aligned vectors
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::x_axis(), vox::math::simd_float4::x_axis()), 0.f,
            0.f, 0.f, 1.f);

    // Opposed vectors
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::x_axis(), -vox::math::simd_float4::x_axis()), 0.f,
            1.f, 0.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(-vox::math::simd_float4::x_axis(), vox::math::simd_float4::x_axis()), 0.f,
            -1.f, 0.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::y_axis(), -vox::math::simd_float4::y_axis()), 0.f,
            0.f, 1.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(-vox::math::simd_float4::y_axis(), vox::math::simd_float4::y_axis()), 0.f,
            0.f, -1.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::z_axis(), -vox::math::simd_float4::z_axis()), 0.f,
            -1.f, 0.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(-vox::math::simd_float4::z_axis(), vox::math::simd_float4::z_axis()), 0.f,
            1.f, 0.f, 0);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::Load(0.707106769f, 0.707106769f, 0.f, 93.f),
                                            -vox::math::simd_float4::Load(0.707106769f, 0.707106769f, 0.f, 99.f)),
            -0.707106769f, 0.707106769f, 0.f, 0.f);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromUnitVectors(vox::math::simd_float4::Load(0.f, 0.707106769f, 0.707106769f, 93.f),
                                            -vox::math::simd_float4::Load(0.f, 0.707106769f, 0.707106769f, 99.f)),
            0.f, -0.707106769f, 0.707106769f, 0.f);
}

TEST(QuaternionAxisAngle, vox_simd_math) {
    // Expect assertions from invalid inputs
    EXPECT_ASSERTION(SimdQuaternion::FromAxisAngle(vox::math::simd_float4::zero(), vox::math::simd_float4::zero()),
                     "axis is not normalized.");
#ifndef NDEBUG
    const SimdQuaternion unorm = {vox::math::simd_float4::Load(0.f, 0.f, 0.f, 2.f)};
#endif  //  NDEBUG
    EXPECT_ASSERTION(ToAxisAngle(unorm), "_q is not normalized.");

    // Identity
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromAxisAngle(vox::math::simd_float4::x_axis(), vox::math::simd_float4::zero()), 0.f, 0.f,
            0.f, 1.f);
    EXPECT_SIMDFLOAT_EQ(ToAxisAngle(SimdQuaternion::identity()), 1.f, 0.f, 0.f, 0.f);

    // Other axis angles
    const vox::math::SimdFloat4 pi_2 = vox::math::simd_float4::LoadX(vox::kHalfPiF);
    const SimdQuaternion qy_pi_2 = SimdQuaternion::FromAxisAngle(vox::math::simd_float4::y_axis(), pi_2);
    EXPECT_SIMDQUATERNION_EQ(qy_pi_2, 0.f, .70710677f, 0.f, .70710677f);
    EXPECT_SIMDFLOAT_EQ(ToAxisAngle(qy_pi_2), 0.f, 1.f, 0.f, vox::kHalfPiF);

    const SimdQuaternion qy_mpi_2 = SimdQuaternion::FromAxisAngle(vox::math::simd_float4::y_axis(), -pi_2);
    EXPECT_SIMDQUATERNION_EQ(qy_mpi_2, 0.f, -.70710677f, 0.f, .70710677f);
    EXPECT_SIMDFLOAT_EQ(ToAxisAngle(qy_mpi_2), 0.f, -1.f, 0.f,
                        vox::kHalfPiF);  // q = -q
    const SimdQuaternion qmy_pi_2 = SimdQuaternion::FromAxisAngle(-vox::math::simd_float4::y_axis(), pi_2);
    EXPECT_SIMDQUATERNION_EQ(qmy_pi_2, 0.f, -.70710677f, 0.f, .70710677f);

    const vox::math::SimdFloat4 any_axis = vox::math::simd_float4::Load(.819865f, .033034f, -.571604f, 99.f);
    const vox::math::SimdFloat4 any_angle = vox::math::simd_float4::Load(1.123f, 99.f, 26.f, 93.f);
    const SimdQuaternion qany = SimdQuaternion::FromAxisAngle(any_axis, any_angle);
    EXPECT_SIMDQUATERNION_EQ(qany, .4365425f, .017589169f, -.30435428f, .84645736f);
    EXPECT_SIMDFLOAT_EQ(ToAxisAngle(qany), .819865f, .033034f, -.571604f, 1.123f);
}

TEST(QuaternionAxisCosAngle, vox_simd_math) {
    // Expect assertions from invalid inputs
    EXPECT_ASSERTION(
            SimdQuaternion::FromAxisCosAngle(vox::math::simd_float4::zero(), vox::math::simd_float4::Load1(0.f)),
            "axis is not normalized");
    EXPECT_ASSERTION(SimdQuaternion::FromAxisCosAngle(vox::math::simd_float4::y_axis(),
                                                      vox::math::simd_float4::Load1(-1.0000001f)),
                     "cos is not in \\[-1,1\\] range.");
    EXPECT_ASSERTION(SimdQuaternion::FromAxisCosAngle(vox::math::simd_float4::y_axis(),
                                                      vox::math::simd_float4::Load1(1.0000001f)),
                     "cos is not in \\[-1,1\\] range.");

    // Identity
    EXPECT_SIMDQUATERNION_EQ(SimdQuaternion::FromAxisCosAngle(vox::math::simd_float4::y_axis(),
                                                              vox::math::simd_float4::Load(1.f, 99.f, 93.f, 5.f)),
                             0.f, 0.f, 0.f, 1.f);

    // Other axis angles
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromAxisCosAngle(vox::math::simd_float4::y_axis(),
                                             vox::math::simd_float4::Load(std::cos(vox::kHalfPiF), 99.f, 93.f, 5.f)),
            0.f, .70710677f, 0.f, .70710677f);
    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromAxisCosAngle(-vox::math::simd_float4::y_axis(),
                                             vox::math::simd_float4::Load(std::cos(vox::kHalfPiF), 99.f, 93.f, 5.f)),
            0.f, -.70710677f, 0.f, .70710677f);

    EXPECT_SIMDQUATERNION_EQ(SimdQuaternion::FromAxisCosAngle(
                                     vox::math::simd_float4::y_axis(),
                                     vox::math::simd_float4::Load(std::cos(3.f * vox::kQuarterPiF), 99.f, 93.f, 5.f)),
                             0.f, 0.923879504f, 0.f, 0.382683426f);

    EXPECT_SIMDQUATERNION_EQ(
            SimdQuaternion::FromAxisCosAngle(vox::math::simd_float4::Load(.819865f, .033034f, -.571604f, 99.f),
                                             vox::math::simd_float4::Load(std::cos(1.123f), 99.f, 93.f, 5.f)),
            .4365425f, .017589169f, -.30435428f, .84645736f);
}

TEST(QuaternionTransformVector, vox_simd_math) {
    // 0 length
    EXPECT_SIMDFLOAT3_EQ(TransformVector(SimdQuaternion::FromAxisAngle(vox::math::simd_float4::y_axis(),
                                                                       vox::math::simd_float4::zero()),
                                         vox::math::simd_float4::zero()),
                         0, 0, 0);

    // Unit length
    EXPECT_SIMDFLOAT3_EQ(TransformVector(SimdQuaternion::FromAxisAngle(vox::math::simd_float4::y_axis(),
                                                                       vox::math::simd_float4::zero()),
                                         vox::math::simd_float4::z_axis()),
                         0, 0, 1);

    const vox::math::SimdFloat4 pi_2 = vox::math::simd_float4::LoadX(vox::kHalfPiF);
    EXPECT_SIMDFLOAT3_EQ(TransformVector(SimdQuaternion::FromAxisAngle(vox::math::simd_float4::y_axis(), pi_2),
                                         vox::math::simd_float4::y_axis()),
                         0, 1, 0);
    EXPECT_SIMDFLOAT3_EQ(TransformVector(SimdQuaternion::FromAxisAngle(vox::math::simd_float4::y_axis(), pi_2),
                                         vox::math::simd_float4::x_axis()),
                         0, 0, -1);
    EXPECT_SIMDFLOAT3_EQ(TransformVector(SimdQuaternion::FromAxisAngle(vox::math::simd_float4::y_axis(), pi_2),
                                         vox::math::simd_float4::z_axis()),
                         1, 0, 0);

    // Non unit
    EXPECT_SIMDFLOAT3_EQ(TransformVector(SimdQuaternion::FromAxisAngle(vox::math::simd_float4::z_axis(), pi_2),
                                         vox::math::simd_float4::x_axis() * vox::math::simd_float4::Load1(2.f)),
                         0, 2, 0);
}
