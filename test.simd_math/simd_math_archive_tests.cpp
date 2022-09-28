//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "test.simd_math/gtest_math_helper.h"
#include "vox.base/io/archive.h"
#include "vox.simd_math/simd_math.h"
#include "vox.simd_math/simd_math_archive.h"

TEST(SimdMathArchive, vox_simd_math) {
    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;

        vox::io::MemoryStream stream;
        ASSERT_TRUE(stream.opened());

        // Write soa math types.
        vox::io::OArchive o(&stream, endianess);
        const vox::simd_math::SimdFloat4 of4 = vox::simd_math::simd_float4::Load(46.f, 58.f, 14.f, 5.f);
        o << of4;
        const vox::simd_math::SimdInt4 oi4 = vox::simd_math::simd_int4::Load(46, 58, 14, 5);
        o << oi4;
        const vox::simd_math::Float4x4 of44 = {{vox::simd_math::simd_float4::Load(46.f, 58.f, 14.f, 5.f),
                                                vox::simd_math::simd_float4::Load(26.f, 35.f, 1.f, 27.f),
                                                vox::simd_math::simd_float4::Load(99.f, 11.f, 4.f, 46.f),
                                                vox::simd_math::simd_float4::Load(58.f, 26.f, 14.f, 99.f)}};
        o << of44;

        // Reads soa math types.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive i(&stream);
        vox::simd_math::SimdFloat4 if4;
        i >> if4;
        EXPECT_SIMDFLOAT_EQ(if4, 46.f, 58.f, 14.f, 5.f);
        vox::simd_math::SimdInt4 ii4;
        i >> ii4;
        EXPECT_SIMDINT_EQ(ii4, 46, 58, 14, 5);
        vox::simd_math::Float4x4 if44;
        i >> if44;
        EXPECT_FLOAT4x4_EQ(if44, 46.f, 58.f, 14.f, 5.f, 26.f, 35.f, 1.f, 27.f, 99.f, 11.f, 4.f, 46.f, 58.f, 26.f, 14.f,
                           99.f);
    }
}
