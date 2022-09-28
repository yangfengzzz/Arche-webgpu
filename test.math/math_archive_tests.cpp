//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "test.math/gtest_math_helper.h"
#include "vox.base/io/archive.h"
#include "vox.math/bounding_box3.h"
#include "vox.math/math_archive.h"

TEST(MathArchive, vox_math) {
    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;

        vox::io::MemoryStream stream;
        ASSERT_TRUE(stream.opened());

        // Write math types.
        vox::io::OArchive o(&stream, endianess);
        const vox::Vector2F of2(46.f, 69.f);
        o << of2;
        const vox::Vector3F of3(46.f, 69.f, 58.f);
        o << of3;
        const vox::Vector4F of4(46.f, 69.f, 58.f, 35.f);
        o << of4;
        const vox::Quaternion oquat(46.f, 69.f, 58.f, 35.f);
        o << oquat;
        const vox::ScalableTransform otrans = {of3, oquat, of3};
        o << otrans;
        const vox::BoundingBox3F o_box(vox::Point3F(14.f, 26.f, 46.f), vox::Point3F(58.f, 69.f, 99.f));
        o << o_box;
        const vox::RectFloat o_rect_float(46.f, 69.f, 58.f, 35.f);
        o << o_rect_float;
        const vox::RectInt o_rect_int(46, 69, 58, 35);
        o << o_rect_int;

        // Reads math types.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive i(&stream);
        vox::Vector2F if2;
        i >> if2;
        EXPECT_FLOAT2_EQ(if2, 46.f, 69.f);
        vox::Vector3F if3;
        i >> if3;
        EXPECT_FLOAT3_EQ(if3, 46.f, 69.f, 58.f);
        vox::Vector4F if4;
        i >> if4;
        EXPECT_FLOAT4_EQ(if4, 46.f, 69.f, 58.f, 35.f);
        vox::QuaternionF iquat;
        i >> iquat;
        EXPECT_QUATERNION_EQ(iquat, 46.f, 69.f, 58.f, 35.f);
        vox::ScalableTransform itrans;
        i >> itrans;
        EXPECT_FLOAT3_EQ(itrans.translation, 46.f, 69.f, 58.f);
        EXPECT_QUATERNION_EQ(itrans.rotation, 46.f, 69.f, 58.f, 35.f);
        EXPECT_FLOAT3_EQ(itrans.scale, 46.f, 69.f, 58.f);
        vox::BoundingBox3F i_box;
        i >> i_box;
        EXPECT_POINT3_EQ(i_box.lower_corner, 14.f, 26.f, 46.f);
        EXPECT_POINT3_EQ(i_box.upper_corner, 58.f, 69.f, 99.f);
        vox::RectFloat i_rect_float{};
        i >> i_rect_float;
        EXPECT_FLOAT_EQ(i_rect_float.left, 46.f);
        EXPECT_FLOAT_EQ(i_rect_float.bottom, 69.f);
        EXPECT_FLOAT_EQ(i_rect_float.width, 58.f);
        EXPECT_FLOAT_EQ(i_rect_float.height, 35.f);
        vox::RectInt i_rect_int{};
        i >> i_rect_int;
        EXPECT_EQ(i_rect_int.left, 46);
        EXPECT_EQ(i_rect_int.bottom, 69);
        EXPECT_EQ(i_rect_int.width, 58);
        EXPECT_EQ(i_rect_int.height, 35);
    }
}
