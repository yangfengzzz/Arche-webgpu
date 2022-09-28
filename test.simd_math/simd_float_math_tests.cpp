//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cmath>
#include <cstdint>
#include <limits>

#include "gtest/gtest.h"
#include "test.simd_math/gtest_helper.h"
#include "test.simd_math/gtest_math_helper.h"
#include "vox.base/constants.h"
#include "vox.simd_math/simd_math.h"

using vox::math::SimdFloat4;
using vox::math::SimdInt4;

static_assert(sizeof(SimdFloat4) == 4 * sizeof(float), "Expects SimdFloat4 to be the size of 4 floats.");
static_assert(alignof(SimdFloat4) == 16, "Expects SimdFloat4 to be the size of 16 bytes.");

TEST(Name, vox_simd_math) { EXPECT_TRUE(vox::math::SimdImplementationName() != nullptr); }

TEST(LoadFloat, vox_simd_math) {
    const SimdFloat4 fX = vox::math::simd_float4::LoadX(15.f);
    EXPECT_SIMDFLOAT_EQ(fX, 15.f, 0.f, 0.f, 0.f);

    const SimdFloat4 f1 = vox::math::simd_float4::Load1(15.f);
    EXPECT_SIMDFLOAT_EQ(f1, 15.f, 15.f, 15.f, 15.f);

    const SimdFloat4 f4 = vox::math::simd_float4::Load(1.f, -1.f, 2.f, -3.f);
    EXPECT_SIMDFLOAT_EQ(f4, 1.f, -1.f, 2.f, -3.f);
}

TEST(LoadFloatPtr, vox_simd_math) {
    union Data {
        float f[4 + 5];
        char c[(4 + 5) * sizeof(float)];  // The 2nd char isn't aligned to a float.
    };
    const Data d_in = {{-1.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f}};
    const int aligned_offset = (16 - (reinterpret_cast<intptr_t>(d_in.f) & 0xf)) / sizeof(float);
    assert(aligned_offset > 0 && aligned_offset <= 4);

    EXPECT_SIMDFLOAT_EQ(vox::math::simd_float4::LoadPtr(d_in.f + aligned_offset), d_in.f[aligned_offset + 0],
                        d_in.f[aligned_offset + 1], d_in.f[aligned_offset + 2], d_in.f[aligned_offset + 3]);
    EXPECT_ASSERTION(vox::math::simd_float4::LoadPtr(d_in.f + aligned_offset - 1), "alignment");

    EXPECT_SIMDFLOAT_EQ(vox::math::simd_float4::LoadPtrU(d_in.f + aligned_offset + 1), d_in.f[aligned_offset + 1],
                        d_in.f[aligned_offset + 2], d_in.f[aligned_offset + 3], d_in.f[aligned_offset + 4]);
    EXPECT_ASSERTION(vox::math::simd_float4::LoadPtrU(reinterpret_cast<const float*>(d_in.c + 1)), "alignment");
    EXPECT_SIMDFLOAT_EQ(vox::math::simd_float4::LoadXPtrU(d_in.f + aligned_offset + 1), d_in.f[aligned_offset + 1], 0.f,
                        0.f, 0.f);
    EXPECT_ASSERTION(vox::math::simd_float4::LoadXPtrU(reinterpret_cast<const float*>(d_in.c + 1)), "alignment");
    EXPECT_SIMDFLOAT_EQ(vox::math::simd_float4::Load1PtrU(d_in.f + aligned_offset + 1), d_in.f[aligned_offset + 1],
                        d_in.f[aligned_offset + 1], d_in.f[aligned_offset + 1], d_in.f[aligned_offset + 1]);
    EXPECT_ASSERTION(vox::math::simd_float4::Load1PtrU(reinterpret_cast<const float*>(d_in.c + 1)), "alignment");
    EXPECT_SIMDFLOAT_EQ(vox::math::simd_float4::Load2PtrU(d_in.f + aligned_offset + 1), d_in.f[aligned_offset + 1],
                        d_in.f[aligned_offset + 2], 0.f, 0.f);
    EXPECT_ASSERTION(vox::math::simd_float4::Load2PtrU(reinterpret_cast<const float*>(d_in.c + 1)), "alignment");
    EXPECT_SIMDFLOAT_EQ(vox::math::simd_float4::Load3PtrU(d_in.f + aligned_offset + 1), d_in.f[aligned_offset + 1],
                        d_in.f[aligned_offset + 2], d_in.f[aligned_offset + 3], 0.f);
    EXPECT_ASSERTION(vox::math::simd_float4::Load3PtrU(reinterpret_cast<const float*>(d_in.c + 1)), "alignment");
}

TEST(GetFloat, vox_simd_math) {
    const SimdFloat4 f = vox::math::simd_float4::Load(1.f, 2.f, 3.f, 4.f);

    EXPECT_FLOAT_EQ(vox::math::GetX(f), 1.f);
    EXPECT_FLOAT_EQ(vox::math::GetY(f), 2.f);
    EXPECT_FLOAT_EQ(vox::math::GetZ(f), 3.f);
    EXPECT_FLOAT_EQ(vox::math::GetW(f), 4.f);
}

TEST(SetFloat, vox_simd_math) {
    const SimdFloat4 a = vox::math::simd_float4::Load(1.f, 2.f, 3.f, 4.f);
    const SimdFloat4 b = vox::math::simd_float4::Load(5.f, 6.f, 7.f, 8.f);

    EXPECT_SIMDFLOAT_EQ(vox::math::SetX(a, b), 5.f, 2.f, 3.f, 4.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::SetY(a, b), 1.f, 5.f, 3.f, 4.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::SetZ(a, b), 1.f, 2.f, 5.f, 4.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::SetW(a, b), 1.f, 2.f, 3.f, 5.f);

    EXPECT_ASSERTION(vox::math::SetI(a, b, 4), "Invalid index, out of range.");
    EXPECT_SIMDFLOAT_EQ(vox::math::SetI(a, b, 0), 5.f, 2.f, 3.f, 4.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::SetI(a, b, 1), 1.f, 5.f, 3.f, 4.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::SetI(a, b, 2), 1.f, 2.f, 5.f, 4.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::SetI(a, b, 3), 1.f, 2.f, 3.f, 5.f);
}

TEST(StoreFloatPtr, vox_simd_math) {
    const SimdFloat4 f4 = vox::math::simd_float4::Load(-1.f, 1.f, 2.f, 3.f);

    union Data {
        float f[4 + 4];                   // The 2nd float isn't aligned to a SimdFloat4.
        SimdFloat4 f4[2];                 // Forces alignment.
        char c[(4 + 4) * sizeof(float)];  // The 2nd char isn't aligned to a float.
    };

    {
        Data d_out = {};
        vox::math::StorePtrU(f4, d_out.f + 1);
        EXPECT_FLOAT_EQ(d_out.f[0], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[1], -1.f);
        EXPECT_FLOAT_EQ(d_out.f[2], 1.f);
        EXPECT_FLOAT_EQ(d_out.f[3], 2.f);
        EXPECT_FLOAT_EQ(d_out.f[4], 3.f);
        EXPECT_ASSERTION(vox::math::StorePtrU(f4, reinterpret_cast<float*>(d_out.c + 1)), "alignment");
    }
    {
        Data d_out = {};
        vox::math::Store1PtrU(f4, d_out.f + 1);
        EXPECT_FLOAT_EQ(d_out.f[0], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[1], -1.f);
        EXPECT_FLOAT_EQ(d_out.f[2], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[3], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[4], 0.f);
        EXPECT_ASSERTION(vox::math::Store1PtrU(f4, reinterpret_cast<float*>(d_out.c + 1)), "alignment");
    }
    {
        Data d_out = {};
        vox::math::Store2PtrU(f4, d_out.f + 1);
        EXPECT_FLOAT_EQ(d_out.f[0], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[1], -1.f);
        EXPECT_FLOAT_EQ(d_out.f[2], 1.f);
        EXPECT_FLOAT_EQ(d_out.f[3], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[4], 0.f);
        EXPECT_ASSERTION(vox::math::Store2Ptr(f4, reinterpret_cast<float*>(d_out.c + 1)), "alignment");
    }
    {
        Data d_out = {};
        vox::math::Store3PtrU(f4, d_out.f + 1);
        EXPECT_FLOAT_EQ(d_out.f[0], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[1], -1.f);
        EXPECT_FLOAT_EQ(d_out.f[2], 1.f);
        EXPECT_FLOAT_EQ(d_out.f[3], 2.f);
        EXPECT_FLOAT_EQ(d_out.f[4], 0.f);
        EXPECT_ASSERTION(vox::math::Store3Ptr(f4, reinterpret_cast<float*>(d_out.c + 1)), "alignment");
    }
    {
        Data d_out = {};
        vox::math::StorePtr(f4, d_out.f);
        EXPECT_FLOAT_EQ(d_out.f[0], -1.f);
        EXPECT_FLOAT_EQ(d_out.f[1], 1.f);
        EXPECT_FLOAT_EQ(d_out.f[2], 2.f);
        EXPECT_FLOAT_EQ(d_out.f[3], 3.f);
        EXPECT_FLOAT_EQ(d_out.f[4], 0.f);
        EXPECT_ASSERTION(vox::math::StorePtr(f4, d_out.f + 1), "alignment");
    }
    {
        Data d_out = {};
        vox::math::Store1Ptr(f4, d_out.f);
        EXPECT_FLOAT_EQ(d_out.f[0], -1.f);
        EXPECT_FLOAT_EQ(d_out.f[1], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[2], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[3], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[4], 0.f);
        EXPECT_ASSERTION(vox::math::Store1Ptr(f4, d_out.f + 1), "alignment");
    }
    {
        Data d_out = {};
        vox::math::Store2Ptr(f4, d_out.f);
        EXPECT_FLOAT_EQ(d_out.f[0], -1.f);
        EXPECT_FLOAT_EQ(d_out.f[1], 1.f);
        EXPECT_FLOAT_EQ(d_out.f[2], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[3], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[4], 0.f);
        EXPECT_ASSERTION(vox::math::Store2Ptr(f4, d_out.f + 1), "alignment");
    }
    {
        Data d_out = {};
        vox::math::Store3Ptr(f4, d_out.f);
        EXPECT_FLOAT_EQ(d_out.f[0], -1.f);
        EXPECT_FLOAT_EQ(d_out.f[1], 1.f);
        EXPECT_FLOAT_EQ(d_out.f[2], 2.f);
        EXPECT_FLOAT_EQ(d_out.f[3], 0.f);
        EXPECT_FLOAT_EQ(d_out.f[4], 0.f);
        EXPECT_ASSERTION(vox::math::Store3Ptr(f4, d_out.f + 1), "alignment");
    }
}

TEST(ConstantFloat, vox_simd_math) {
    const SimdFloat4 zero = vox::math::simd_float4::zero();
    EXPECT_SIMDFLOAT_EQ(zero, 0.f, 0.f, 0.f, 0.f);

    const SimdFloat4 one = vox::math::simd_float4::one();
    EXPECT_SIMDFLOAT_EQ(one, 1.f, 1.f, 1.f, 1.f);

    const SimdFloat4 x_axis = vox::math::simd_float4::x_axis();
    EXPECT_SIMDFLOAT_EQ(x_axis, 1.f, 0.f, 0.f, 0.f);

    const SimdFloat4 y_axis = vox::math::simd_float4::y_axis();
    EXPECT_SIMDFLOAT_EQ(y_axis, 0.f, 1.f, 0.f, 0.f);

    const SimdFloat4 z_axis = vox::math::simd_float4::z_axis();
    EXPECT_SIMDFLOAT_EQ(z_axis, 0.f, 0.f, 1.f, 0.f);

    const SimdFloat4 w_axis = vox::math::simd_float4::w_axis();
    EXPECT_SIMDFLOAT_EQ(w_axis, 0.f, 0.f, 0.f, 1.f);
}

TEST(SplatFloat, vox_simd_math) {
    const SimdFloat4 f = vox::math::simd_float4::Load(1.f, -1.f, 2.f, -3.f);

    const SimdFloat4 x = vox::math::SplatX(f);
    EXPECT_SIMDFLOAT_EQ(x, 1.f, 1.f, 1.f, 1.f);

    const SimdFloat4 y = vox::math::SplatY(f);
    EXPECT_SIMDFLOAT_EQ(y, -1.f, -1.f, -1.f, -1.f);

    const SimdFloat4 z = vox::math::SplatZ(f);
    EXPECT_SIMDFLOAT_EQ(z, 2.f, 2.f, 2.f, 2.f);

    const SimdFloat4 w = vox::math::SplatW(f);
    EXPECT_SIMDFLOAT_EQ(w, -3.f, -3.f, -3.f, -3.f);

    const SimdFloat4 s3210 = vox::math::Swizzle<3, 2, 1, 0>(f);
    EXPECT_SIMDFLOAT_EQ(s3210, -3.f, 2.f, -1.f, 1.f);

    const SimdFloat4 s0123 = vox::math::Swizzle<0, 1, 2, 3>(f);
    EXPECT_SIMDFLOAT_EQ(s0123, 1.f, -1.f, 2.f, -3.f);

    const SimdFloat4 s0011 = vox::math::Swizzle<0, 0, 1, 1>(f);
    EXPECT_SIMDFLOAT_EQ(s0011, 1.f, 1.f, -1.f, -1.f);

    const SimdFloat4 s2233 = vox::math::Swizzle<2, 2, 3, 3>(f);
    EXPECT_SIMDFLOAT_EQ(s2233, 2.f, 2.f, -3.f, -3.f);

    const SimdFloat4 s0101 = vox::math::Swizzle<0, 1, 0, 1>(f);
    EXPECT_SIMDFLOAT_EQ(s0101, 1.f, -1.f, 1.f, -1.f);

    const SimdFloat4 s2323 = vox::math::Swizzle<2, 3, 2, 3>(f);
    EXPECT_SIMDFLOAT_EQ(s2323, 2.f, -3.f, 2.f, -3.f);
}

TEST(FromInt, vox_simd_math) {
    const vox::math::SimdInt4 i = vox::math::simd_int4::Load(0, 46, -93, 9926429);
    EXPECT_SIMDFLOAT_EQ(vox::math::simd_float4::FromInt(i), 0.f, 46.f, -93.f, 9926429.f);
}

TEST(ArithmeticFloat, vox_simd_math) {
    const vox::math::SimdFloat4 a = vox::math::simd_float4::Load(0.5f, 1.f, 2.f, 3.f);
    const vox::math::SimdFloat4 b = vox::math::simd_float4::Load(4.f, 5.f, -6.f, 0.f);
    const vox::math::SimdFloat4 c = vox::math::simd_float4::Load(-8.f, 9.f, 10.f, 11.f);

    const vox::math::SimdFloat4 add = a + b;
    EXPECT_SIMDFLOAT_EQ(add, 4.5f, 6.f, -4.f, 3.f);

    const vox::math::SimdFloat4 sub = a - b;
    EXPECT_SIMDFLOAT_EQ(sub, -3.5f, -4.f, 8.f, 3.f);

    const vox::math::SimdFloat4 neg = -b;
    EXPECT_SIMDFLOAT_EQ(neg, -4.f, -5.f, 6.f, -0.f);

    const vox::math::SimdFloat4 mul = a * b;
    EXPECT_SIMDFLOAT_EQ(mul, 2.f, 5.f, -12.f, 0.f);

    const vox::math::SimdFloat4 div = a / b;
    EXPECT_SIMDFLOAT3_EQ(div, 0.5f / 4.f, 1.f / 5.f, -2.f / 6.f);

    const vox::math::SimdFloat4 madd = vox::math::MAdd(a, b, c);
    EXPECT_SIMDFLOAT_EQ(madd, -6.f, 14.f, -2.f, 11.f);

    const vox::math::SimdFloat4 msub = vox::math::MSub(a, b, c);
    EXPECT_SIMDFLOAT_EQ(msub, 10.f, -4.f, -22.f, -11.f);

    const vox::math::SimdFloat4 nmadd = vox::math::NMAdd(a, b, c);
    EXPECT_SIMDFLOAT_EQ(nmadd, -10.f, 4.f, 22.f, 11.f);

    const vox::math::SimdFloat4 nmsub = vox::math::NMSub(a, b, c);
    EXPECT_SIMDFLOAT_EQ(nmsub, 6.f, -14.f, 2.f, -11.f);

    const vox::math::SimdFloat4 divx = vox::math::DivX(a, b);
    EXPECT_SIMDFLOAT_EQ(divx, 0.5f / 4.f, 1.f, 2.f, 3.f);

    const vox::math::SimdFloat4 hadd2 = vox::math::HAdd2(a);
    EXPECT_SIMDFLOAT_EQ(hadd2, 1.5f, 1.f, 2.f, 3.f);

    const vox::math::SimdFloat4 hadd3 = vox::math::HAdd3(a);
    EXPECT_SIMDFLOAT_EQ(hadd3, 3.5f, 1.f, 2.f, 3.f);

    const vox::math::SimdFloat4 hadd4 = vox::math::HAdd4(a);
    EXPECT_FLOAT_EQ(vox::math::GetX(hadd4), 6.5f);

    const vox::math::SimdFloat4 dot2 = vox::math::Dot2(a, b);
    EXPECT_FLOAT_EQ(vox::math::GetX(dot2), 7.f);

    const vox::math::SimdFloat4 dot3 = vox::math::Dot3(a, b);
    EXPECT_FLOAT_EQ(vox::math::GetX(dot3), -5.f);

    const vox::math::SimdFloat4 dot4 = vox::math::Dot4(a, b);
    EXPECT_FLOAT_EQ(vox::math::GetX(dot4), -5.f);

    const vox::math::SimdFloat4 cross = vox::math::Cross3(vox::math::simd_float4::Load(1.f, -2.f, 3.f, 46.f),
                                                          vox::math::simd_float4::Load(4.f, 5.f, 6.f, 27.f));
    EXPECT_FLOAT_EQ(vox::math::GetX(cross), -27.f);
    EXPECT_FLOAT_EQ(vox::math::GetY(cross), 6.f);
    EXPECT_FLOAT_EQ(vox::math::GetZ(cross), 13.f);

    const vox::math::SimdFloat4 rcp = vox::math::RcpEst(b);
    EXPECT_SIMDFLOAT3_EQ_EST(rcp, 1.f / 4.f, 1.f / 5.f, -1.f / 6.f);

    const vox::math::SimdFloat4 rcpnr = vox::math::RcpEstNR(b);
    EXPECT_SIMDFLOAT3_EQ(rcpnr, 1.f / 4.f, 1.f / 5.f, -1.f / 6.f);

    const vox::math::SimdFloat4 rcpxnr = vox::math::RcpEstXNR(b);
    EXPECT_FLOAT_EQ(vox::math::GetX(rcpxnr), 1.f / 4.f);

    const vox::math::SimdFloat4 rcpx = vox::math::RcpEstX(b);
    EXPECT_SIMDFLOAT_EQ_EST(rcpx, 1.f / 4.f, 5.f, -6.f, 0.f);

    const vox::math::SimdFloat4 sqrt = vox::math::Sqrt(a);
    EXPECT_SIMDFLOAT_EQ(sqrt, .7071068f, 1.f, 1.4142135f, 1.7320508f);

    const vox::math::SimdFloat4 sqrtx = vox::math::SqrtX(b);
    EXPECT_SIMDFLOAT_EQ(sqrtx, 2.f, 5.f, -6.f, 0.f);

    const vox::math::SimdFloat4 rsqrt = vox::math::RSqrtEst(b);
    EXPECT_SIMDFLOAT2_EQ_EST(rsqrt, 1.f / 2.f, 1.f / 2.23606798f);

    const vox::math::SimdFloat4 rsqrtnr = vox::math::RSqrtEstNR(b);
    EXPECT_SIMDFLOAT2_EQ_EST(rsqrtnr, 1.f / 2.f, 1.f / 2.23606798f);

    const vox::math::SimdFloat4 rsqrtx = vox::math::RSqrtEstX(a);
    EXPECT_SIMDFLOAT_EQ_EST(rsqrtx, 1.f / .7071068f, 1.f, 2.f, 3.f);

    const vox::math::SimdFloat4 rsqrtxnr = vox::math::RSqrtEstXNR(a);
    EXPECT_FLOAT_EQ(vox::math::GetX(rsqrtxnr), 1.f / .7071068f);

    const vox::math::SimdFloat4 abs = vox::math::Abs(b);
    EXPECT_SIMDFLOAT_EQ(abs, 4.f, 5.f, 6.f, 0.f);

    const SimdInt4 sign = vox::math::Sign(b);
    EXPECT_SIMDINT_EQ(sign, 0, 0, 0x80000000, 0);
}

TEST(LengthFloat, vox_simd_math) {
    const SimdFloat4 f = vox::math::simd_float4::Load(1.f, 2.f, 4.f, 8.f);

    const SimdFloat4 len2 = vox::math::Length2(f);
    EXPECT_FLOAT_EQ(vox::math::GetX(len2), 2.236068f);

    const SimdFloat4 len3 = vox::math::Length3(f);
    EXPECT_FLOAT_EQ(vox::math::GetX(len3), 4.5825758f);

    const SimdFloat4 len4 = vox::math::Length4(f);
    EXPECT_FLOAT_EQ(vox::math::GetX(len4), 9.2195444f);

    const SimdFloat4 len2sqr = vox::math::Length2Sqr(f);
    EXPECT_FLOAT_EQ(vox::math::GetX(len2sqr), 5.f);

    const SimdFloat4 len3sqr = vox::math::Length3Sqr(f);
    EXPECT_FLOAT_EQ(vox::math::GetX(len3sqr), 21.f);

    const SimdFloat4 len4sqr = vox::math::Length4Sqr(f);
    EXPECT_FLOAT_EQ(vox::math::GetX(len4sqr), 85.f);
}

TEST(NormalizeFloat, vox_simd_math) {
    const SimdFloat4 f = vox::math::simd_float4::Load(1.f, 2.f, 4.f, 8.f);
    const SimdFloat4 unit = vox::math::simd_float4::x_axis();
    const SimdFloat4 zero = vox::math::simd_float4::zero();

    EXPECT_SIMDINT_EQ(vox::math::IsNormalized2(f), 0, 0, 0, 0);
    const SimdFloat4 norm2 = vox::math::Normalize2(f);
    EXPECT_SIMDFLOAT_EQ(norm2, .44721359f, .89442718f, 4.f, 8.f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalized2(norm2), 0xffffffff, 0, 0, 0);

    const SimdFloat4 norm_est2 = vox::math::NormalizeEst2(f);
    EXPECT_SIMDFLOAT_EQ_EST(norm_est2, .44721359f, .89442718f, 4.f, 8.f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalizedEst2(norm_est2), 0xffffffff, 0, 0, 0);

    EXPECT_ASSERTION(vox::math::Normalize2(zero), "_v is not normalizable");
    EXPECT_ASSERTION(vox::math::NormalizeEst2(zero), "_v is not normalizable");

    EXPECT_SIMDINT_EQ(vox::math::IsNormalized3(f), 0, 0, 0, 0);
    const SimdFloat4 norm3 = vox::math::Normalize3(f);
    EXPECT_SIMDFLOAT_EQ(norm3, .21821788f, .43643576f, .87287152f, 8.f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalized3(norm3), 0xffffffff, 0, 0, 0);

    const SimdFloat4 norm_est3 = vox::math::NormalizeEst3(f);
    EXPECT_SIMDFLOAT_EQ_EST(norm_est3, .21821788f, .43643576f, .87287152f, 8.f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalizedEst3(norm_est3), 0xffffffff, 0, 0, 0);

    EXPECT_ASSERTION(vox::math::Normalize3(zero), "_v is not normalizable");
    EXPECT_ASSERTION(vox::math::NormalizeEst3(zero), "_v is not normalizable");

    EXPECT_SIMDINT_EQ(vox::math::IsNormalized4(f), 0, 0, 0, 0);
    const SimdFloat4 norm4 = vox::math::Normalize4(f);
    EXPECT_SIMDFLOAT_EQ(norm4, .1084652f, .2169304f, .4338609f, .86772186f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalized4(norm4), 0xffffffff, 0, 0, 0);

    const SimdFloat4 norm_est4 = vox::math::NormalizeEst4(f);
    EXPECT_SIMDFLOAT_EQ_EST(norm_est4, .1084652f, .2169304f, .4338609f, .86772186f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalizedEst4(norm_est4), 0xffffffff, 0, 0, 0);

    EXPECT_ASSERTION(vox::math::Normalize4(zero), "_v is not normalizable");
    EXPECT_ASSERTION(vox::math::NormalizeEst4(zero), "_v is not normalizable");

    const SimdFloat4 safe2 = vox::math::NormalizeSafe2(f, unit);
    EXPECT_SIMDFLOAT_EQ(safe2, .4472136f, .8944272f, 4.f, 8.f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalized2(safe2), 0xffffffff, 0, 0, 0);
    const SimdFloat4 safer2 = vox::math::NormalizeSafe2(zero, unit);
    EXPECT_SIMDFLOAT_EQ(safer2, 1.f, 0.f, 0.f, 0.f);
    const SimdFloat4 safe_est2 = vox::math::NormalizeSafeEst2(f, unit);
    EXPECT_SIMDFLOAT_EQ_EST(safe_est2, .4472136f, .8944272f, 4.f, 8.f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalizedEst2(safe_est2), 0xffffffff, 0, 0, 0);
    const SimdFloat4 safer_est2 = vox::math::NormalizeSafeEst2(zero, unit);
    EXPECT_SIMDFLOAT_EQ_EST(safer_est2, 1.f, 0.f, 0.f, 0.f);

    const SimdFloat4 safe3 = vox::math::NormalizeSafe3(f, unit);
    EXPECT_SIMDFLOAT_EQ(safe3, .21821788f, .43643576f, .87287152f, 8.f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalized3(safe3), 0xffffffff, 0, 0, 0);
    const SimdFloat4 safer3 = vox::math::NormalizeSafe3(zero, unit);
    EXPECT_SIMDFLOAT_EQ(safer3, 1.f, 0.f, 0.f, 0.f);
    const SimdFloat4 safe_est3 = vox::math::NormalizeSafeEst3(f, unit);
    EXPECT_SIMDFLOAT_EQ_EST(safe_est3, .21821788f, .43643576f, .87287152f, 8.f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalizedEst3(safe_est3), 0xffffffff, 0, 0, 0);
    const SimdFloat4 safer_est3 = vox::math::NormalizeSafeEst3(zero, unit);
    EXPECT_SIMDFLOAT_EQ_EST(safer_est3, 1.f, 0.f, 0.f, 0.f);

    const SimdFloat4 safe4 = vox::math::NormalizeSafe4(f, unit);
    EXPECT_SIMDFLOAT_EQ(safe4, .1084652f, .2169305f, .433861f, .8677219f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalized4(safe4), 0xffffffff, 0, 0, 0);
    const SimdFloat4 safer4 = vox::math::NormalizeSafe4(zero, unit);
    EXPECT_SIMDFLOAT_EQ(safer4, 1.f, 0.f, 0.f, 0.f);
    const SimdFloat4 safe_est4 = vox::math::NormalizeSafeEst4(f, unit);
    EXPECT_SIMDFLOAT_EQ_EST(safe_est4, .1084652f, .2169305f, .433861f, .8677219f);
    EXPECT_SIMDINT_EQ(vox::math::IsNormalizedEst4(safe_est4), 0xffffffff, 0, 0, 0);
    const SimdFloat4 safer_est4 = vox::math::NormalizeSafeEst4(zero, unit);
    EXPECT_SIMDFLOAT_EQ_EST(safer_est4, 1.f, 0.f, 0.f, 0.f);
}

TEST(CompareFloat, vox_simd_math) {
    const SimdFloat4 a = vox::math::simd_float4::Load(0.5f, 1.f, 2.f, 3.f);
    const SimdFloat4 b = vox::math::simd_float4::Load(4.f, 1.f, -6.f, 7.f);
    const SimdFloat4 c = vox::math::simd_float4::Load(4.f, 5.f, 6.f, 7.f);

    const SimdFloat4 min = vox::math::Min(a, b);
    EXPECT_SIMDFLOAT_EQ(min, 0.5f, 1.f, -6.f, 3.f);

    const SimdFloat4 max = vox::math::Max(a, b);
    EXPECT_SIMDFLOAT_EQ(max, 4.f, 1.f, 2.f, 7.f);

    const SimdFloat4 min0 = vox::math::Min0(b);
    EXPECT_SIMDFLOAT_EQ(min0, 0.f, 0.f, -6.f, 0.f);

    const SimdFloat4 max0 = vox::math::Max0(b);
    EXPECT_SIMDFLOAT_EQ(max0, 4.f, 1.f, 0.f, 7.f);

    EXPECT_SIMDFLOAT_EQ(vox::math::Clamp(a, vox::math::simd_float4::Load(-12.f, 2.f, 9.f, 3.f), c), .5f, 2.f, 6.f, 3.f);

    const SimdInt4 eq1 = vox::math::CmpEq(a, b);
    EXPECT_SIMDINT_EQ(eq1, 0, 0xffffffff, 0, 0);

    const SimdInt4 eq2 = vox::math::CmpEq(a, a);
    EXPECT_SIMDINT_EQ(eq2, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

    const SimdInt4 neq1 = vox::math::CmpNe(a, b);
    EXPECT_SIMDINT_EQ(neq1, 0xffffffff, 0, 0xffffffff, 0xffffffff);

    const SimdInt4 neq2 = vox::math::CmpNe(a, a);
    EXPECT_SIMDINT_EQ(neq2, 0, 0, 0, 0);

    const SimdInt4 lt = vox::math::CmpLt(a, b);
    EXPECT_SIMDINT_EQ(lt, 0xffffffff, 0, 0, 0xffffffff);

    const SimdInt4 le = vox::math::CmpLe(a, b);
    EXPECT_SIMDINT_EQ(le, 0xffffffff, 0xffffffff, 0u, 0xffffffff);

    const SimdInt4 gt = vox::math::CmpGt(a, b);
    EXPECT_SIMDINT_EQ(gt, 0u, 0u, 0xffffffff, 0u);

    const SimdInt4 ge = vox::math::CmpGe(a, b);
    EXPECT_SIMDINT_EQ(ge, 0u, 0xffffffff, 0xffffffff, 0u);
}

TEST(LerpFloat, vox_simd_math) {
    const SimdFloat4 a = vox::math::simd_float4::Load(0.f, 1.f, 2.f, 4.f);
    const SimdFloat4 b = vox::math::simd_float4::Load(0.f, -1.f, -2.f, -4.f);
    const SimdFloat4 zero = vox::math::simd_float4::Load1(0.f);
    const SimdFloat4 one = vox::math::simd_float4::Load1(1.f);

    const SimdFloat4 lerp0 = vox::math::Lerp(a, b, zero);
    EXPECT_SIMDFLOAT_EQ(lerp0, 0.f, 1.f, 2.f, 4.f);

    const SimdFloat4 lerp1 = vox::math::Lerp(a, b, one);
    EXPECT_SIMDFLOAT_EQ(lerp1, 0.f, -1.f, -2.f, -4.f);

    const SimdFloat4 lhalf = vox::math::Lerp(a, b, vox::math::simd_float4::Load1(0.5f));
    EXPECT_SIMDFLOAT_EQ(lhalf, 0.f, 0.f, 0.f, 0.f);

    const SimdFloat4 lmixed = vox::math::Lerp(a, b, vox::math::simd_float4::Load(0.f, -1.f, 0.5f, 2.f));
    EXPECT_SIMDFLOAT_EQ(lmixed, 0.f, 3.f, 0.f, -12.f);
}

TEST(TrigonometryFloat, vox_simd_math) {
    using vox::kHalfPiF;
    using vox::kPiF;
    using vox::kTwoPiF;

    const SimdFloat4 angle = vox::math::simd_float4::Load(kPiF, kPiF / 6.f, -kHalfPiF, 5.f * kHalfPiF);
    const SimdFloat4 cos = vox::math::simd_float4::Load(-1.f, .86602539f, 0.f, 0.f);
    const SimdFloat4 sin = vox::math::simd_float4::Load(0.f, .5f, -1.f, 1.f);

    const SimdFloat4 angle_tan = vox::math::simd_float4::Load(0.f, kPiF / 6.f, -kPiF / 3.f, 9.f * kPiF / 4.f);
    const SimdFloat4 tan = vox::math::simd_float4::Load(0.f, .57735f, -1.73205f, 1.f);

    EXPECT_SIMDFLOAT_EQ(vox::math::Cos(angle), -1.f, .86602539f, 0.f, 0.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::Cos(angle + vox::math::simd_float4::Load1(kTwoPiF)), -1.f, .86602539f, 0.f, 0.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::Cos(angle + vox::math::simd_float4::Load1(kTwoPiF * 12.f)), -1.f, .86602539f, 0.f,
                        0.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::Cos(angle - vox::math::simd_float4::Load1(kTwoPiF * 24.f)), -1.f, .86602539f, 0.f,
                        0.f);

    EXPECT_SIMDFLOAT_EQ(vox::math::CosX(angle), -1.f, kPiF / 6.f, -kHalfPiF, 5.f * kHalfPiF);
    EXPECT_SIMDFLOAT_EQ(vox::math::CosX(angle + vox::math::simd_float4::LoadX(kTwoPiF)), -1.f, kPiF / 6.f, -kHalfPiF,
                        5.f * kHalfPiF);
    EXPECT_SIMDFLOAT_EQ(vox::math::CosX(angle + vox::math::simd_float4::LoadX(kTwoPiF * 12.f)), -1.f, kPiF / 6.f,
                        -kHalfPiF, 5.f * kHalfPiF);
    EXPECT_SIMDFLOAT_EQ(vox::math::CosX(angle - vox::math::simd_float4::LoadX(kTwoPiF * 24.f)), -1.f, kPiF / 6.f,
                        -kHalfPiF, 5.f * kHalfPiF);

    EXPECT_SIMDFLOAT_EQ(vox::math::Sin(angle), 0.f, .5f, -1.f, 1.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::Sin(angle + vox::math::simd_float4::Load1(kTwoPiF)), 0.f, 0.5f, -1.f, 1.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::Sin(angle + vox::math::simd_float4::Load1(kTwoPiF * 12.f)), 0.f, .5f, -1.f, 1.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::Sin(angle - vox::math::simd_float4::Load1(kTwoPiF * 24.f)), 0.f, .5f, -1.f, 1.f);

    EXPECT_SIMDFLOAT_EQ(vox::math::SinX(angle), 0.f, kPiF / 6.f, -kHalfPiF, 5.f * kHalfPiF);
    EXPECT_SIMDFLOAT_EQ(vox::math::SinX(angle + vox::math::simd_float4::LoadX(kTwoPiF)), 0.f, kPiF / 6.f, -kHalfPiF,
                        5.f * kHalfPiF);
    EXPECT_SIMDFLOAT_EQ(vox::math::SinX(angle + vox::math::simd_float4::LoadX(kTwoPiF * 12.f)), 0.f, kPiF / 6.f,
                        -kHalfPiF, 5.f * kHalfPiF);
    EXPECT_SIMDFLOAT_EQ(vox::math::SinX(angle - vox::math::simd_float4::LoadX(kTwoPiF * 24.f)), 0.f, kPiF / 6.f,
                        -kHalfPiF, 5.f * kHalfPiF);

    EXPECT_SIMDFLOAT_EQ(vox::math::ACos(cos), kPiF, kPiF / 6.f, kHalfPiF, kHalfPiF);
    EXPECT_SIMDFLOAT_EQ(vox::math::ACosX(cos), kPiF, .86602539f, 0.f, 0.f);

    EXPECT_SIMDFLOAT_EQ(vox::math::ASin(sin), 0.f, kPiF / 6.f, -kHalfPiF, kHalfPiF);
    EXPECT_SIMDFLOAT_EQ(vox::math::ASinX(sin), 0.f, .5f, -1.f, 1.f);

    EXPECT_SIMDFLOAT_EQ(vox::math::Tan(angle_tan), 0.f, 0.57735f, -1.73205f, 1.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::TanX(angle_tan), 0.f, kPiF / 6.f, -kPiF / 3.f, 9.f * kPiF / 4.f);

    EXPECT_SIMDFLOAT_EQ(vox::math::ATan(tan), 0.f, kPiF / 6.f, -kPiF / 3.f, kPiF / 4.f);
    EXPECT_SIMDFLOAT_EQ(vox::math::ATanX(tan), 0.f, .57735f, -1.73205f, 1.f);
}

TEST(LogicalFloat, vox_simd_math) {
    const SimdFloat4 a = vox::math::simd_float4::Load(0.f, 1.f, 2.f, 3.f);
    const SimdFloat4 b = vox::math::simd_float4::Load(1.f, -1.f, -3.f, -4.f);
    const SimdInt4 mbool = vox::math::simd_int4::Load(0xffffffff, 0, 0, 0xffffffff);
    const SimdInt4 mbit = vox::math::simd_int4::Load(0xffffffff, 0, 0x80000000, 0x7fffffff);
    const SimdFloat4 mfloat = vox::math::simd_float4::Load(1.f, 0.f, -0.f, 3.f);

    const SimdFloat4 select = vox::math::Select(mbool, a, b);
    EXPECT_SIMDFLOAT_EQ(select, 0.f, -1.f, -3.f, 3.f);

    const SimdFloat4 andm = vox::math::And(b, mbit);
    EXPECT_SIMDFLOAT_EQ(andm, 1.f, 0.f, 0.f, 4.f);

    const SimdFloat4 andnm = vox::math::AndNot(b, mbit);
    EXPECT_SIMDFLOAT_EQ(andnm, 0.f, -1.f, 3.f, -0.f);

    const SimdFloat4 andf = vox::math::And(b, mfloat);
    EXPECT_SIMDFLOAT_EQ(andf, 1.f, 0.f, -0.f, 2.f);

    const SimdFloat4 orm = vox::math::Or(a, mbit);
    union {
        float f;
        unsigned int i;
    } orx = {vox::math::GetX(orm)};
    EXPECT_EQ(orx.i, 0xffffffff);
    EXPECT_FLOAT_EQ(vox::math::GetY(orm), 1.f);
    EXPECT_FLOAT_EQ(vox::math::GetZ(orm), -2.f);
    union {
        float f;
        int i;
    } orw = {vox::math::GetW(orm)};
    EXPECT_TRUE(orw.i == 0x7fffffff);

    const SimdFloat4 ormf = vox::math::Or(a, mfloat);
    EXPECT_SIMDFLOAT_EQ(ormf, 1.f, 1.f, -2.f, 3.f);

    const SimdFloat4 xorm = vox::math::Xor(a, mbit);
    union {
        float f;
        unsigned int i;
    } xorx = {vox::math::GetX(xorm)};
    EXPECT_TRUE(xorx.i == 0xffffffff);
    EXPECT_FLOAT_EQ(vox::math::GetY(xorm), 1.f);
    EXPECT_FLOAT_EQ(vox::math::GetZ(xorm), -2.f);
    union {
        float f;
        unsigned int i;
    } xorw = {vox::math::GetW(xorm)};
    EXPECT_TRUE(xorw.i == 0x3fbfffff);

    const SimdFloat4 xormf = vox::math::Xor(a, mfloat);
    EXPECT_SIMDFLOAT_EQ(xormf, 1.f, 1.f, -2.f, 0.f);
}

TEST(Half, vox_simd_math) {
    // 0
    EXPECT_EQ(vox::math::FloatToHalf(0.f), 0);
    EXPECT_FLOAT_EQ(vox::math::HalfToFloat(0), 0.f);
    EXPECT_EQ(vox::math::FloatToHalf(-0.f), 0x8000);
    EXPECT_FLOAT_EQ(vox::math::HalfToFloat(0x8000), -0.f);
    EXPECT_EQ(vox::math::FloatToHalf(std::numeric_limits<float>::min()), 0);
    EXPECT_EQ(vox::math::FloatToHalf(std::numeric_limits<float>::denorm_min()), 0);
    EXPECT_EQ(vox::math::FloatToHalf(std::numeric_limits<float>::denorm_min() / 10.f), 0);

    // 1
    EXPECT_EQ(vox::math::FloatToHalf(1.f), 0x3c00);
    EXPECT_FLOAT_EQ(vox::math::HalfToFloat(0x3c00), 1.f);
    EXPECT_EQ(vox::math::FloatToHalf(-1.f), 0xbc00);
    EXPECT_FLOAT_EQ(vox::math::HalfToFloat(0xbc00), -1.f);

    // Bounds
    EXPECT_EQ(vox::math::FloatToHalf(65504.f), 0x7bff);
    EXPECT_EQ(vox::math::FloatToHalf(-65504.f), 0xfbff);

    // Min, Max, Infinity
    EXPECT_EQ(vox::math::FloatToHalf(10e-16f), 0);
    EXPECT_EQ(vox::math::FloatToHalf(10e+16f), 0x7c00);
    EXPECT_FLOAT_EQ(vox::math::HalfToFloat(0x7c00), std::numeric_limits<float>::infinity());
    EXPECT_EQ(vox::math::FloatToHalf(std::numeric_limits<float>::max()), 0x7c00);
    EXPECT_EQ(vox::math::FloatToHalf(std::numeric_limits<float>::infinity()), 0x7c00);
    EXPECT_EQ(vox::math::FloatToHalf(-10e+16f), 0xfc00);
    EXPECT_EQ(vox::math::FloatToHalf(-std::numeric_limits<float>::infinity()), 0xfc00);
    EXPECT_EQ(vox::math::FloatToHalf(-std::numeric_limits<float>::max()), 0xfc00);
    EXPECT_FLOAT_EQ(vox::math::HalfToFloat(0xfc00), -std::numeric_limits<float>::infinity());

    // Nan
    EXPECT_EQ(vox::math::FloatToHalf(std::numeric_limits<float>::quiet_NaN()), 0x7e00);
    EXPECT_EQ(vox::math::FloatToHalf(std::numeric_limits<float>::signaling_NaN()), 0x7e00);
    // According to the IEEE standard, NaN values have the odd property that
    // comparisons involving them are always false
    EXPECT_FALSE(vox::math::HalfToFloat(0x7e00) == vox::math::HalfToFloat(0x7e00));

    // Random tries in range [10e-4,10e4].
    for (float pow = -4.f; pow <= 4.f; pow += 1.f) {
        const float max = powf(10.f, pow);
        // Expect a 1/1000 precision over floats.
        const float precision = max / 1000.f;

        const int n = 1000;
        for (int i = 0; i < n; ++i) {
            const float frand = max * (2.f * i / n - 1.f);

            const uint16_t h = vox::math::FloatToHalf(frand);
            const float f = vox::math::HalfToFloat(h);
            EXPECT_NEAR(frand, f, precision);
        }
    }
}

TEST(SimdHalf, vox_simd_math) {
    // 0
    EXPECT_SIMDINT_EQ(vox::math::FloatToHalf(vox::math::simd_float4::Load(0.f, -0.f, std::numeric_limits<float>::min(),
                                                                          std::numeric_limits<float>::denorm_min())),
                      0, 0x00008000, 0, 0);
    EXPECT_SIMDFLOAT_EQ(vox::math::HalfToFloat(vox::math::simd_int4::Load(0, 0x00008000, 0, 0)), 0.f, -0.f, 0.f, 0.f);

    // 1
    EXPECT_SIMDINT_EQ(vox::math::FloatToHalf(vox::math::simd_float4::Load(1.f, -1.f, 0.f, -0.f)), 0x00003c00,
                      0x0000bc00, 0, 0x00008000);
    EXPECT_SIMDFLOAT_EQ(vox::math::HalfToFloat(vox::math::simd_int4::Load(0x3c00, 0xbc00, 0, 0x00008000)), 1.f, -1.f,
                        0.f, -0.f);

    // Bounds
    EXPECT_SIMDINT_EQ(vox::math::FloatToHalf(vox::math::simd_float4::Load(65504.f, -65504.f, 65604.f, -65604.f)),
                      0x00007bff, 0x0000fbff, 0x00007c00, 0x0000fc00);

    // Min, Max, Infinity
    EXPECT_SIMDINT_EQ(
            vox::math::FloatToHalf(vox::math::simd_float4::Load(10e-16f, 10e+16f, std::numeric_limits<float>::max(),
                                                                std::numeric_limits<float>::infinity())),
            0, 0x00007c00, 0x00007c00, 0x00007c00);
    EXPECT_SIMDINT_EQ(
            vox::math::FloatToHalf(vox::math::simd_float4::Load(-10e-16f, -10e+16f, -std::numeric_limits<float>::max(),
                                                                -std::numeric_limits<float>::max())),
            0x00008000, 0x0000fc00, 0x0000fc00, 0x0000fc00);

    // Nan
    EXPECT_SIMDINT_EQ(
            vox::math::FloatToHalf(vox::math::simd_float4::Load(std::numeric_limits<float>::quiet_NaN(),
                                                                std::numeric_limits<float>::signaling_NaN(), 0, 0)),
            0x00007e00, 0x00007e00, 0, 0);

    // Inf and NAN
    const SimdFloat4 infnan = vox::math::HalfToFloat(vox::math::simd_int4::Load(0x00007c00, 0x0000fc00, 0x00007e00, 0));
    EXPECT_FLOAT_EQ(vox::math::GetX(infnan), std::numeric_limits<float>::infinity());
    EXPECT_FLOAT_EQ(vox::math::GetY(infnan), -std::numeric_limits<float>::infinity());
    EXPECT_FALSE(vox::math::GetZ(infnan) == vox::math::GetZ(infnan));

    // Random tries in range [10e-4,10e4].
    srand(0);
    for (float pow = -4.f; pow <= 4.f; pow += 1.f) {
        const float max = powf(10.f, pow);
        // Expect a 1/1000 precision over floats.
        const float precision = max / 1000.f;

        const int n = 1000;
        for (int i = 0; i < n; ++i) {
            const SimdFloat4 frand =
                    vox::math::simd_float4::Load(max * (.5f * i / n - .25f), max * (1.f * i / n - .5f),
                                                 max * (1.5f * i / n - .75f), max * (2.f * i / n - 1.f));

            const SimdInt4 h = vox::math::FloatToHalf(frand);
            const SimdFloat4 f = vox::math::HalfToFloat(h);

            EXPECT_NEAR(vox::math::GetX(frand), vox::math::GetX(f), precision);
            EXPECT_NEAR(vox::math::GetY(frand), vox::math::GetY(f), precision);
            EXPECT_NEAR(vox::math::GetZ(frand), vox::math::GetZ(f), precision);
            EXPECT_NEAR(vox::math::GetW(frand), vox::math::GetW(f), precision);
        }
    }
}
