//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "vox.animation/offline/raw_skeleton.h"
#include "vox.base/io/archive.h"

using vox::animation::offline::RawSkeleton;

TEST(Empty, RawSkeletonSerialize) {
    vox::io::MemoryStream stream;

    // Streams out.
    vox::io::OArchive o(&stream, vox::getNativeEndianness());

    RawSkeleton o_skeleton;
    o << o_skeleton;

    // Streams in.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    RawSkeleton i_skeleton;
    i >> i_skeleton;

    EXPECT_EQ(o_skeleton.num_joints(), i_skeleton.num_joints());
}

TEST(Filled, RawSkeletonSerialize) {
    /* Builds output skeleton.
     4 joints

       *
       |
      root
      / \
     j0 j1
     |
     j2
    */

    RawSkeleton o_skeleton;
    o_skeleton.roots.resize(1);
    RawSkeleton::Joint& root = o_skeleton.roots[0];
    root.name = "root";
    root.transform = vox::ScalableTransform::identity();
    root.children.resize(2);
    root.children[0].name = "j0";
    root.children[0].transform = vox::ScalableTransform::identity();
    root.children[0].transform.translation.x = 46.f;
    root.children[1].name = "j1";
    root.children[1].transform = vox::ScalableTransform::identity();
    root.children[1].transform.scale.y = 99.f;
    root.children[0].children.resize(1);
    root.children[0].children[0].name = "j2";
    root.children[0].children[0].transform = vox::ScalableTransform::identity();
    root.children[0].children[0].transform.rotation = vox::QuaternionF(0.f, 0.f, 1.f, 0.f);

    EXPECT_TRUE(o_skeleton.Validate());
    EXPECT_EQ(o_skeleton.num_joints(), 4);

    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;
        vox::io::MemoryStream stream;

        // Streams out.
        vox::io::OArchive o(&stream, endianess);
        o << o_skeleton;

        // Streams in.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive i(&stream);

        RawSkeleton i_skeleton;
        i >> i_skeleton;

        EXPECT_TRUE(i_skeleton.Validate());
        ASSERT_EQ(o_skeleton.num_joints(), i_skeleton.num_joints());

        // Compares skeletons joint's name.
        EXPECT_STREQ(o_skeleton.roots[0].name.c_str(), i_skeleton.roots[0].name.c_str());
        EXPECT_STREQ(o_skeleton.roots[0].children[0].name.c_str(), i_skeleton.roots[0].children[0].name.c_str());
        EXPECT_STREQ(o_skeleton.roots[0].children[0].children[0].name.c_str(),
                     i_skeleton.roots[0].children[0].children[0].name.c_str());
        EXPECT_STREQ(o_skeleton.roots[0].children[1].name.c_str(), i_skeleton.roots[0].children[1].name.c_str());

        // Compares skeletons joint's transform.
        EXPECT_TRUE(
                o_skeleton.roots[0].transform.translation.isSimilar(i_skeleton.roots[0].transform.translation));
        EXPECT_TRUE(o_skeleton.roots[0].transform.rotation.isSimilar(i_skeleton.roots[0].transform.rotation));
        EXPECT_TRUE(o_skeleton.roots[0].transform.scale.isSimilar(i_skeleton.roots[0].transform.scale));

        EXPECT_TRUE(o_skeleton.roots[0].children[0].transform.translation.isSimilar(
                i_skeleton.roots[0].children[0].transform.translation));
        EXPECT_TRUE(o_skeleton.roots[0].children[0].transform.rotation.isSimilar(
                i_skeleton.roots[0].children[0].transform.rotation));
        EXPECT_TRUE(o_skeleton.roots[0].children[0].transform.scale.isSimilar(
                i_skeleton.roots[0].children[0].transform.scale));

        EXPECT_TRUE(o_skeleton.roots[0].children[0].children[0].transform.translation.isSimilar(
                i_skeleton.roots[0].children[0].children[0].transform.translation));
        EXPECT_TRUE(o_skeleton.roots[0].children[0].children[0].transform.rotation.isSimilar(
                i_skeleton.roots[0].children[0].children[0].transform.rotation));
        EXPECT_TRUE(o_skeleton.roots[0].children[0].children[0].transform.scale.isSimilar(
                i_skeleton.roots[0].children[0].children[0].transform.scale));

        EXPECT_TRUE(o_skeleton.roots[0].children[1].transform.translation.isSimilar(
                i_skeleton.roots[0].children[1].transform.translation));
        EXPECT_TRUE(o_skeleton.roots[0].children[1].transform.rotation.isSimilar(
                i_skeleton.roots[0].children[1].transform.rotation));
        EXPECT_TRUE(o_skeleton.roots[0].children[1].transform.scale.isSimilar(
                i_skeleton.roots[0].children[1].transform.scale));
    }
}

TEST(AlreadyInitialized, RawSkeletonSerialize) {
    RawSkeleton o_skeleton;
    o_skeleton.roots.resize(1);

    vox::io::MemoryStream stream;

    // Streams out.
    vox::io::OArchive o(&stream);
    o << o_skeleton;

    // Streams out a second time.
    o_skeleton.roots.resize(2);
    o << o_skeleton;

    // Streams in.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    RawSkeleton i_skeleton;
    i >> i_skeleton;
    ASSERT_EQ(i_skeleton.num_joints(), 1);

    // A second time
    i >> i_skeleton;
    ASSERT_EQ(i_skeleton.num_joints(), 2);
}
