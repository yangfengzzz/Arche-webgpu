//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cstdint>

#include "gtest/gtest.h"
#include "test.simd_math/gtest_helper.h"
#include "test.simd_math/gtest_math_helper.h"
#include "vox.base/constants.h"
#include "vox.simd_math/simd_math.h"

using vox::simd_math::SimdInt4;

static_assert(sizeof(SimdInt4) == 4 * sizeof(int32_t), "Expects SimdInt4 to be the size of 4 int32_t.");
static_assert(alignof(SimdInt4) == 16, "Expects SimdInt4 to be the size of 16 bytes.");

TEST(LoadInt, vox_simd_math) {
    const SimdInt4 iX = vox::simd_math::simd_int4::LoadX(15);
    EXPECT_SIMDINT_EQ(iX, 15, 0, 0, 0);

    const SimdInt4 i1 = vox::simd_math::simd_int4::Load1(15);
    EXPECT_SIMDINT_EQ(i1, 15, 15, 15, 15);

    const SimdInt4 i4 = vox::simd_math::simd_int4::Load(1, -1, 2, -3);
    EXPECT_SIMDINT_EQ(i4, 1, -1, 2, -3);

    const SimdInt4 itX = vox::simd_math::simd_int4::LoadX(true);
    EXPECT_SIMDINT_EQ(itX, 0xffffffff, 0, 0, 0);

    const SimdInt4 ifX = vox::simd_math::simd_int4::LoadX(false);
    EXPECT_SIMDINT_EQ(ifX, 0, 0, 0, 0);

    const SimdInt4 it1 = vox::simd_math::simd_int4::Load1(true);
    EXPECT_SIMDINT_EQ(it1, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

    const SimdInt4 if1 = vox::simd_math::simd_int4::Load1(false);
    EXPECT_SIMDINT_EQ(if1, 0, 0, 0, 0);

    const SimdInt4 ibttff = vox::simd_math::simd_int4::Load(true, true, false, false);
    EXPECT_SIMDINT_EQ(ibttff, 0xffffffff, 0xffffffff, 0, 0);

    const SimdInt4 ibftft = vox::simd_math::simd_int4::Load(false, true, false, true);
    EXPECT_SIMDINT_EQ(ibftft, 0, 0xffffffff, 0, 0xffffffff);
}

TEST(LoadIntPtr, vox_simd_math) {
    union Data {
        int i[4 + 5];
        char c[(4 + 5) * sizeof(int)];  // The 2nd char isn't aligned to an integer.
    };
    const Data d_in = {{-1, 1, 2, 3, 4, 5, 6, 7, 8}};
    const int aligned_offset = (16 - (reinterpret_cast<intptr_t>(d_in.i) & 0xf)) / sizeof(float);
    assert(aligned_offset > 0 && aligned_offset <= 4);

    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::LoadPtrU(d_in.i + aligned_offset + 1), d_in.i[aligned_offset + 1],
                      d_in.i[aligned_offset + 2], d_in.i[aligned_offset + 3], d_in.i[aligned_offset + 4]);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::LoadPtrU(reinterpret_cast<const int*>(d_in.c + 1)), "alignment");
    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::LoadPtr(d_in.i + aligned_offset), d_in.i[aligned_offset],
                      d_in.i[aligned_offset + 1], d_in.i[aligned_offset + 2], d_in.i[aligned_offset + 3]);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::LoadPtr(d_in.i + aligned_offset + 1), "alignment");

    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::LoadXPtrU(d_in.i + aligned_offset), d_in.i[aligned_offset], 0, 0, 0);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::LoadXPtrU(reinterpret_cast<const int*>(d_in.c + 1)), "alignment");
    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::Load1PtrU(d_in.i + aligned_offset), d_in.i[aligned_offset],
                      d_in.i[aligned_offset], d_in.i[aligned_offset], d_in.i[aligned_offset]);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::Load1PtrU(reinterpret_cast<const int*>(d_in.c + 1)), "alignment");

    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::LoadXPtr(d_in.i + aligned_offset), d_in.i[aligned_offset], 0, 0, 0);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::LoadXPtr(d_in.i + aligned_offset + 1), "alignment");
    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::Load1Ptr(d_in.i + aligned_offset), d_in.i[aligned_offset],
                      d_in.i[aligned_offset], d_in.i[aligned_offset], d_in.i[aligned_offset]);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::Load1Ptr(d_in.i + aligned_offset + 1), "alignment");

    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::Load2Ptr(d_in.i + aligned_offset), d_in.i[aligned_offset],
                      d_in.i[aligned_offset + 1], 0, 0);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::Load2Ptr(d_in.i + aligned_offset + 1), "alignment");
    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::Load2PtrU(d_in.i + aligned_offset + 1), d_in.i[aligned_offset + 1],
                      d_in.i[aligned_offset + 2], 0, 0);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::Load2PtrU(reinterpret_cast<const int*>(d_in.c + 1)), "alignment");

    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::Load3Ptr(d_in.i + aligned_offset), d_in.i[aligned_offset],
                      d_in.i[aligned_offset + 1], d_in.i[aligned_offset + 2], 0);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::Load3Ptr(d_in.i + aligned_offset + 1), "alignment");
    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::Load3PtrU(d_in.i + aligned_offset + 1), d_in.i[aligned_offset + 1],
                      d_in.i[aligned_offset + 2], d_in.i[aligned_offset + 3], 0);
    EXPECT_ASSERTION(vox::simd_math::simd_int4::Load3PtrU(reinterpret_cast<const int*>(d_in.c + 1)), "alignment");
}

TEST(GetInt, vox_simd_math) {
    const SimdInt4 i = vox::simd_math::simd_int4::Load(1, 2, 3, 4);

    EXPECT_EQ(vox::simd_math::GetX(i), 1);
    EXPECT_EQ(vox::simd_math::GetY(i), 2);
    EXPECT_EQ(vox::simd_math::GetZ(i), 3);
    EXPECT_EQ(vox::simd_math::GetW(i), 4);
}

TEST(SetInt, vox_simd_math) {
    const SimdInt4 a = vox::simd_math::simd_int4::Load(1, 2, 3, 4);
    const SimdInt4 b = vox::simd_math::simd_int4::Load(5, 6, 7, 8);

    EXPECT_SIMDINT_EQ(vox::simd_math::SetX(a, b), 5, 2, 3, 4);
    EXPECT_SIMDINT_EQ(vox::simd_math::SetY(a, b), 1, 5, 3, 4);
    EXPECT_SIMDINT_EQ(vox::simd_math::SetZ(a, b), 1, 2, 5, 4);
    EXPECT_SIMDINT_EQ(vox::simd_math::SetW(a, b), 1, 2, 3, 5);

    EXPECT_ASSERTION(vox::simd_math::SetI(a, b, 4), "Invalid index, out of range.");
    EXPECT_SIMDINT_EQ(vox::simd_math::SetI(a, b, 0), 5, 2, 3, 4);
    EXPECT_SIMDINT_EQ(vox::simd_math::SetI(a, b, 1), 1, 5, 3, 4);
    EXPECT_SIMDINT_EQ(vox::simd_math::SetI(a, b, 2), 1, 2, 5, 4);
    EXPECT_SIMDINT_EQ(vox::simd_math::SetI(a, b, 3), 1, 2, 3, 5);
}

TEST(StoreIntPtr, vox_simd_math) {
    const SimdInt4 i4 = vox::simd_math::simd_int4::Load(-1, 1, 2, 3);

    union Data {
        int i[4 + 4];                   // The 2nd float isn't aligned to a SimdInt4.
        SimdInt4 i4[2];                 // Forces alignment.
        char c[(4 + 4) * sizeof(int)];  // The 2nd char isn't aligned to an integer.
    };

    {
        Data d_out = {};
        vox::simd_math::StorePtrU(i4, d_out.i + 1);
        EXPECT_EQ(d_out.i[0], 0);
        EXPECT_EQ(d_out.i[1], -1);
        EXPECT_EQ(d_out.i[2], 1);
        EXPECT_EQ(d_out.i[3], 2);
        EXPECT_EQ(d_out.i[4], 3);
        EXPECT_ASSERTION(vox::simd_math::StorePtrU(i4, reinterpret_cast<int*>(d_out.c + 1)), "alignment");
    }
    {
        Data d_out = {};
        vox::simd_math::Store1PtrU(i4, d_out.i + 1);
        EXPECT_EQ(d_out.i[0], 0);
        EXPECT_EQ(d_out.i[1], -1);
        EXPECT_EQ(d_out.i[2], 0);
        EXPECT_EQ(d_out.i[3], 0);
        EXPECT_EQ(d_out.i[4], 0);
        EXPECT_ASSERTION(vox::simd_math::Store1PtrU(i4, reinterpret_cast<int*>(d_out.c + 1)), "alignment");
    }
    {
        Data d_out = {};
        vox::simd_math::Store2PtrU(i4, d_out.i + 1);
        EXPECT_EQ(d_out.i[0], 0);
        EXPECT_EQ(d_out.i[1], -1);
        EXPECT_EQ(d_out.i[2], 1);
        EXPECT_EQ(d_out.i[3], 0);
        EXPECT_EQ(d_out.i[4], 0);
        EXPECT_ASSERTION(vox::simd_math::Store2Ptr(i4, reinterpret_cast<int*>(d_out.c + 1)), "alignment");
    }
    {
        Data d_out = {};
        vox::simd_math::Store3PtrU(i4, d_out.i + 1);
        EXPECT_EQ(d_out.i[0], 0);
        EXPECT_EQ(d_out.i[1], -1);
        EXPECT_EQ(d_out.i[2], 1);
        EXPECT_EQ(d_out.i[3], 2);
        EXPECT_EQ(d_out.i[4], 0);
        EXPECT_ASSERTION(vox::simd_math::Store3Ptr(i4, reinterpret_cast<int*>(d_out.c + 1)), "alignment");
    }
    {
        Data d_out = {};
        vox::simd_math::StorePtr(i4, d_out.i);
        EXPECT_EQ(d_out.i[0], -1);
        EXPECT_EQ(d_out.i[1], 1);
        EXPECT_EQ(d_out.i[2], 2);
        EXPECT_EQ(d_out.i[3], 3);
        EXPECT_EQ(d_out.i[4], 0);
        EXPECT_ASSERTION(vox::simd_math::StorePtr(i4, d_out.i + 1), "alignment");
    }
    {
        Data d_out = {};
        vox::simd_math::Store1Ptr(i4, d_out.i);
        EXPECT_EQ(d_out.i[0], -1);
        EXPECT_EQ(d_out.i[1], 0);
        EXPECT_EQ(d_out.i[2], 0);
        EXPECT_EQ(d_out.i[3], 0);
        EXPECT_EQ(d_out.i[4], 0);
        EXPECT_ASSERTION(vox::simd_math::Store1Ptr(i4, d_out.i + 1), "alignment");
    }
    {
        Data d_out = {};
        vox::simd_math::Store2Ptr(i4, d_out.i);
        EXPECT_EQ(d_out.i[0], -1);
        EXPECT_EQ(d_out.i[1], 1);
        EXPECT_EQ(d_out.i[2], 0);
        EXPECT_EQ(d_out.i[3], 0);
        EXPECT_EQ(d_out.i[4], 0);
        EXPECT_ASSERTION(vox::simd_math::Store2Ptr(i4, d_out.i + 1), "alignment");
    }
    {
        Data d_out = {};
        vox::simd_math::Store3Ptr(i4, d_out.i);
        EXPECT_EQ(d_out.i[0], -1);
        EXPECT_EQ(d_out.i[1], 1);
        EXPECT_EQ(d_out.i[2], 2);
        EXPECT_EQ(d_out.i[3], 0);
        EXPECT_EQ(d_out.i[4], 0);
        EXPECT_ASSERTION(vox::simd_math::Store3Ptr(i4, d_out.i + 1), "alignment");
    }
}

TEST(ConstantInt, vox_simd_math) {
    const SimdInt4 zero = vox::simd_math::simd_int4::zero();
    EXPECT_SIMDINT_EQ(zero, 0, 0, 0, 0);

    const SimdInt4 one = vox::simd_math::simd_int4::one();
    EXPECT_SIMDINT_EQ(one, 1, 1, 1, 1);

    const SimdInt4 x_axis = vox::simd_math::simd_int4::x_axis();
    EXPECT_SIMDINT_EQ(x_axis, 1, 0, 0, 0);

    const SimdInt4 y_axis = vox::simd_math::simd_int4::y_axis();
    EXPECT_SIMDINT_EQ(y_axis, 0, 1, 0, 0);

    const SimdInt4 z_axis = vox::simd_math::simd_int4::z_axis();
    EXPECT_SIMDINT_EQ(z_axis, 0, 0, 1, 0);

    const SimdInt4 w_axis = vox::simd_math::simd_int4::w_axis();
    EXPECT_SIMDINT_EQ(w_axis, 0, 0, 0, 1);

    const SimdInt4 all_true = vox::simd_math::simd_int4::all_true();
    EXPECT_SIMDINT_EQ(all_true, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

    const SimdInt4 all_false = vox::simd_math::simd_int4::all_false();
    EXPECT_SIMDINT_EQ(all_false, 0, 0, 0, 0);

    const SimdInt4 mask_sign = vox::simd_math::simd_int4::mask_sign();
    EXPECT_SIMDINT_EQ(mask_sign, 0x80000000, 0x80000000, 0x80000000, 0x80000000);

    const SimdInt4 mask_sign_xyz = vox::simd_math::simd_int4::mask_sign_xyz();
    EXPECT_SIMDINT_EQ(mask_sign_xyz, 0x80000000, 0x80000000, 0x80000000, 0x00000000);

    const SimdInt4 mask_sign_w = vox::simd_math::simd_int4::mask_sign_w();
    EXPECT_SIMDINT_EQ(mask_sign_w, 0x00000000, 0x00000000, 0x00000000, 0x80000000);

    const SimdInt4 mask_not_sign = vox::simd_math::simd_int4::mask_not_sign();
    EXPECT_SIMDINT_EQ(mask_not_sign, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff);

    const SimdInt4 mask_ffff = vox::simd_math::simd_int4::mask_ffff();
    EXPECT_SIMDINT_EQ(mask_ffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

    const SimdInt4 mask_0000 = vox::simd_math::simd_int4::mask_0000();
    EXPECT_SIMDINT_EQ(mask_0000, 0, 0, 0, 0);

    const SimdInt4 mask_fff0 = vox::simd_math::simd_int4::mask_fff0();
    EXPECT_SIMDINT_EQ(mask_fff0, 0xffffffff, 0xffffffff, 0xffffffff, 0);

    const SimdInt4 mask_f000 = vox::simd_math::simd_int4::mask_f000();
    EXPECT_SIMDINT_EQ(mask_f000, 0xffffffff, 0, 0, 0);

    const SimdInt4 mask_0f00 = vox::simd_math::simd_int4::mask_0f00();
    EXPECT_SIMDINT_EQ(mask_0f00, 0, 0xffffffff, 0, 0);

    const SimdInt4 mask_00f0 = vox::simd_math::simd_int4::mask_00f0();
    EXPECT_SIMDINT_EQ(mask_00f0, 0, 0, 0xffffffff, 0);

    const SimdInt4 mask_000f = vox::simd_math::simd_int4::mask_000f();
    EXPECT_SIMDINT_EQ(mask_000f, 0, 0, 0, 0xffffffff);
}

TEST(SplatInt, vox_simd_math) {
    const SimdInt4 i = vox::simd_math::simd_int4::Load(1, -1, 2, -3);

    const SimdInt4 x = vox::simd_math::SplatX(i);
    EXPECT_SIMDINT_EQ(x, 1, 1, 1, 1);

    const SimdInt4 y = vox::simd_math::SplatY(i);
    EXPECT_SIMDINT_EQ(y, -1, -1, -1, -1);

    const SimdInt4 z = vox::simd_math::SplatZ(i);
    EXPECT_SIMDINT_EQ(z, 2, 2, 2, 2);

    const SimdInt4 w = vox::simd_math::SplatW(i);
    EXPECT_SIMDINT_EQ(w, -3, -3, -3, -3);

    const SimdInt4 s3210 = vox::simd_math::Swizzle<3, 2, 1, 0>(i);
    EXPECT_SIMDINT_EQ(s3210, -3, 2, -1, 1);

    const SimdInt4 s0123 = vox::simd_math::Swizzle<0, 1, 2, 3>(i);
    EXPECT_SIMDINT_EQ(s0123, 1, -1, 2, -3);

    const SimdInt4 s0011 = vox::simd_math::Swizzle<0, 0, 1, 1>(i);
    EXPECT_SIMDINT_EQ(s0011, 1, 1, -1, -1);

    const SimdInt4 s2233 = vox::simd_math::Swizzle<2, 2, 3, 3>(i);
    EXPECT_SIMDINT_EQ(s2233, 2, 2, -3, -3);

    const SimdInt4 s0101 = vox::simd_math::Swizzle<0, 1, 0, 1>(i);
    EXPECT_SIMDINT_EQ(s0101, 1, -1, 1, -1);

    const SimdInt4 s2323 = vox::simd_math::Swizzle<2, 3, 2, 3>(i);
    EXPECT_SIMDINT_EQ(s2323, 2, -3, 2, -3);
}

TEST(FromFloat, vox_simd_math) {
    const vox::simd_math::SimdFloat4 f = vox::simd_math::simd_float4::Load(0.f, 46.93f, 46.26f, -93.99f);
    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::FromFloatRound(f), 0, 47, 46, -94);
    EXPECT_SIMDINT_EQ(vox::simd_math::simd_int4::FromFloatTrunc(f), 0, 46, 46, -93);
}

TEST(ArithmeticInt, vox_simd_math) {
    const SimdInt4 a = vox::simd_math::simd_int4::Load(0, 1, 2, 3);
    const SimdInt4 b = vox::simd_math::simd_int4::Load(4, 5, -6, 7);

    const SimdInt4 hadd2 = vox::simd_math::HAdd2(a);
    EXPECT_SIMDINT_EQ(hadd2, 1, 1, 2, 3);

    const SimdInt4 hadd3 = vox::simd_math::HAdd3(a);
    EXPECT_SIMDINT_EQ(hadd3, 3, 1, 2, 3);

    const SimdInt4 hadd4 = vox::simd_math::HAdd4(a);
    EXPECT_SIMDINT_EQ(hadd4, 6, 1, 2, 3);

    const SimdInt4 abs = vox::simd_math::Abs(b);
    EXPECT_SIMDINT_EQ(abs, 4, 5, 6, 7);

    const SimdInt4 sign = vox::simd_math::Sign(b);
    EXPECT_SIMDINT_EQ(sign, 0, 0, 0x80000000, 0);
}

TEST(CompareInt, vox_simd_math) {
    const SimdInt4 a = vox::simd_math::simd_int4::Load(0, 1, 2, 3);
    const SimdInt4 b = vox::simd_math::simd_int4::Load(4, 1, -6, 7);
    const SimdInt4 c = vox::simd_math::simd_int4::Load(4, 5, 6, 7);

    const SimdInt4 min = vox::simd_math::Min(a, b);
    EXPECT_SIMDINT_EQ(min, 0, 1, -6, 3);

    const SimdInt4 max = vox::simd_math::Max(a, b);
    EXPECT_SIMDINT_EQ(max, 4, 1, 2, 7);

    const SimdInt4 min0 = vox::simd_math::Min0(b);
    EXPECT_SIMDINT_EQ(min0, 0, 0, -6, 0);

    const SimdInt4 max0 = vox::simd_math::Max0(b);
    EXPECT_SIMDINT_EQ(max0, 4, 1, 0, 7);

    EXPECT_SIMDINT_EQ(vox::simd_math::Clamp(a, vox::simd_math::simd_int4::Load(-12, 2, 9, 3), c), 0, 2, 6, 3);

    const SimdInt4 eq1 = vox::simd_math::CmpEq(a, b);
    EXPECT_SIMDINT_EQ(eq1, 0, 0xffffffff, 0, 0);

    const SimdInt4 eq2 = vox::simd_math::CmpEq(a, a);
    EXPECT_SIMDINT_EQ(eq2, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

    const SimdInt4 neq1 = vox::simd_math::CmpNe(a, b);
    EXPECT_SIMDINT_EQ(neq1, 0xffffffff, 0, 0xffffffff, 0xffffffff);

    const SimdInt4 neq2 = vox::simd_math::CmpNe(a, a);
    EXPECT_SIMDINT_EQ(neq2, 0, 0, 0, 0);

    const SimdInt4 lt = vox::simd_math::CmpLt(a, b);
    EXPECT_SIMDINT_EQ(lt, 0xffffffff, 0, 0, 0xffffffff);

    const SimdInt4 le = vox::simd_math::CmpLe(a, b);
    EXPECT_SIMDINT_EQ(le, 0xffffffff, 0xffffffff, 0, 0xffffffff);

    const SimdInt4 gt = vox::simd_math::CmpGt(a, b);
    EXPECT_SIMDINT_EQ(gt, 0, 0, 0xffffffff, 0);

    const SimdInt4 ge = vox::simd_math::CmpGe(a, b);
    EXPECT_SIMDINT_EQ(ge, 0, 0xffffffff, 0xffffffff, 0);
}

TEST(MaskInt, vox_simd_math) {
    EXPECT_EQ(vox::simd_math::MoveMask(vox::simd_math::simd_int4::all_false()), 0x00000000);
    EXPECT_EQ(vox::simd_math::MoveMask(vox::simd_math::simd_int4::all_true()), 0x0000000f);
    EXPECT_EQ(vox::simd_math::MoveMask(vox::simd_math::simd_int4::mask_f000()), 0x00000001);
    EXPECT_EQ(vox::simd_math::MoveMask(vox::simd_math::simd_int4::mask_0f00()), 0x00000002);
    EXPECT_EQ(vox::simd_math::MoveMask(vox::simd_math::simd_int4::mask_00f0()), 0x00000004);
    EXPECT_EQ(vox::simd_math::MoveMask(vox::simd_math::simd_int4::mask_000f()), 0x00000008);
    EXPECT_EQ(vox::simd_math::MoveMask(vox::simd_math::simd_int4::Load(0xffffffff, 0x00000000, 0x80000001, 0x7fffffff)),
              0x00000005);
    EXPECT_EQ(vox::simd_math::MoveMask(vox::simd_math::simd_int4::Load(0xffffffff, 0x1000000f, 0x80000001, 0x8ffffffe)),
              0x0000000d);
    EXPECT_TRUE(vox::simd_math::AreAllFalse(vox::simd_math::simd_int4::all_false()));
    EXPECT_FALSE(vox::simd_math::AreAllFalse(vox::simd_math::simd_int4::all_true()));
    EXPECT_FALSE(vox::simd_math::AreAllFalse(vox::simd_math::simd_int4::mask_000f()));

    EXPECT_TRUE(vox::simd_math::AreAllTrue(vox::simd_math::simd_int4::all_true()));
    EXPECT_FALSE(vox::simd_math::AreAllTrue(vox::simd_math::simd_int4::all_false()));
    EXPECT_FALSE(vox::simd_math::AreAllTrue(vox::simd_math::simd_int4::mask_000f()));

    EXPECT_TRUE(vox::simd_math::AreAllFalse3(vox::simd_math::simd_int4::all_false()));
    EXPECT_TRUE(vox::simd_math::AreAllFalse3(vox::simd_math::simd_int4::mask_000f()));
    EXPECT_FALSE(vox::simd_math::AreAllFalse3(vox::simd_math::simd_int4::all_true()));
    EXPECT_FALSE(vox::simd_math::AreAllFalse3(vox::simd_math::simd_int4::mask_f000()));

    EXPECT_TRUE(vox::simd_math::AreAllTrue3(vox::simd_math::simd_int4::all_true()));
    EXPECT_FALSE(vox::simd_math::AreAllTrue3(vox::simd_math::simd_int4::all_false()));
    EXPECT_FALSE(vox::simd_math::AreAllTrue3(vox::simd_math::simd_int4::mask_f000()));

    EXPECT_TRUE(vox::simd_math::AreAllFalse2(vox::simd_math::simd_int4::all_false()));
    EXPECT_TRUE(vox::simd_math::AreAllFalse2(vox::simd_math::simd_int4::mask_000f()));
    EXPECT_FALSE(vox::simd_math::AreAllFalse2(vox::simd_math::simd_int4::all_true()));
    EXPECT_FALSE(vox::simd_math::AreAllFalse2(vox::simd_math::simd_int4::mask_f000()));

    EXPECT_TRUE(vox::simd_math::AreAllTrue2(vox::simd_math::simd_int4::all_true()));
    EXPECT_FALSE(vox::simd_math::AreAllTrue2(vox::simd_math::simd_int4::all_false()));
    EXPECT_FALSE(vox::simd_math::AreAllTrue2(vox::simd_math::simd_int4::mask_f000()));

    EXPECT_TRUE(vox::simd_math::AreAllFalse1(vox::simd_math::simd_int4::all_false()));
    EXPECT_TRUE(vox::simd_math::AreAllFalse1(vox::simd_math::simd_int4::mask_000f()));
    EXPECT_FALSE(vox::simd_math::AreAllFalse1(vox::simd_math::simd_int4::all_true()));
    EXPECT_FALSE(vox::simd_math::AreAllFalse1(vox::simd_math::simd_int4::mask_f000()));

    EXPECT_TRUE(vox::simd_math::AreAllTrue1(vox::simd_math::simd_int4::all_true()));
    EXPECT_FALSE(vox::simd_math::AreAllTrue1(vox::simd_math::simd_int4::all_false()));
    EXPECT_TRUE(vox::simd_math::AreAllTrue1(vox::simd_math::simd_int4::mask_f000()));
}

TEST(LogicalInt, vox_simd_math) {
    const SimdInt4 a = vox::simd_math::simd_int4::Load(0xffffffff, 0x00000000, 0x80000001, 0x7fffffff);
    const SimdInt4 b = vox::simd_math::simd_int4::Load(0x80000001, 0xffffffff, 0x7fffffff, 0x00000000);
    const SimdInt4 c = vox::simd_math::simd_int4::Load(0x01234567, 0x89abcdef, 0x01234567, 0x89abcdef);
    const SimdInt4 cond = vox::simd_math::simd_int4::Load(0xffffffff, 0x00000000, 0xffffffff, 0x00000000);

    const SimdInt4 andm = vox::simd_math::And(a, b);
    EXPECT_SIMDINT_EQ(andm, 0x80000001, 0x00000000, 0x00000001, 0x00000000);

    const SimdInt4 andnm = vox::simd_math::AndNot(a, b);
    EXPECT_SIMDINT_EQ(andnm, 0x7ffffffe, 0x00000000, 0x80000000, 0x7fffffff);

    const SimdInt4 orm = vox::simd_math::Or(a, b);
    EXPECT_SIMDINT_EQ(orm, 0xffffffff, 0xffffffff, 0xffffffff, 0x7fffffff);

    const SimdInt4 xorm = vox::simd_math::Xor(a, b);
    EXPECT_SIMDINT_EQ(xorm, 0x7ffffffe, 0xffffffff, 0xfffffffe, 0x7fffffff);

    const SimdInt4 select = vox::simd_math::Select(cond, b, c);
    EXPECT_SIMDINT_EQ(select, 0x80000001, 0x89abcdef, 0x7fffffff, 0x89abcdef);
}

TEST(ShiftInt, vox_simd_math) {
    const SimdInt4 a = vox::simd_math::simd_int4::Load(0xffffffff, 0x00000000, 0x80000001, 0x7fffffff);

    const SimdInt4 shift_l = vox::simd_math::ShiftL(a, 3);
    EXPECT_SIMDINT_EQ(shift_l, 0xfffffff8, 0x00000000, 0x00000008, 0xfffffff8);

    const SimdInt4 shift_r = vox::simd_math::ShiftR(a, 3);
    EXPECT_SIMDINT_EQ(shift_r, 0xffffffff, 0x00000000, 0xf0000000, 0x0fffffff);

    const SimdInt4 shift_ru = vox::simd_math::ShiftRu(a, 3);
    EXPECT_SIMDINT_EQ(shift_ru, 0x1fffffff, 0x00000000, 0x10000000, 0x0fffffff);
}
