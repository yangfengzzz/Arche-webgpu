//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "vox.animation/offline/raw_animation.h"
#include "vox.base/io/archive.h"
#include "vox.base/io/stream.h"

using vox::animation::offline::RawAnimation;

TEST(Empty, RawAnimationSerialize) {
    vox::io::MemoryStream stream;

    // Streams out.
    vox::io::OArchive o(&stream, vox::getNativeEndianness());

    RawAnimation o_animation;
    o << o_animation;

    // Streams in.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    RawAnimation i_animation;
    i >> i_animation;

    EXPECT_FLOAT_EQ(o_animation.duration, i_animation.duration);
    EXPECT_EQ(o_animation.num_tracks(), i_animation.num_tracks());
}

TEST(Filled, RawAnimationSerialize) {
    RawAnimation o_animation;
    o_animation.duration = 46.f;
    o_animation.tracks.resize(3);
    const RawAnimation::TranslationKey t_key = {0.f, vox::Vector3F(46.f, 93.f, 99.f)};
    o_animation.tracks[0].translations.push_back(t_key);
    const RawAnimation::RotationKey r_key = {46.f, vox::QuaternionF(0.f, 1.f, 0.f, 0.f)};
    o_animation.tracks[1].rotations.push_back(r_key);
    const RawAnimation::ScaleKey s_key = {1.f, vox::Vector3F(93.f, 46.f, 99.f)};
    o_animation.tracks[2].scales.push_back(s_key);

    EXPECT_TRUE(o_animation.Validate());
    EXPECT_EQ(o_animation.num_tracks(), 3);

    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;
        vox::io::MemoryStream stream;

        // Streams out.
        vox::io::OArchive o(&stream, endianess);
        o << o_animation;

        // Streams in.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive ia(&stream);

        RawAnimation i_animation;
        ia >> i_animation;

        EXPECT_TRUE(i_animation.Validate());
        EXPECT_FLOAT_EQ(o_animation.duration, i_animation.duration);
        ASSERT_EQ(o_animation.num_tracks(), i_animation.num_tracks());

        for (size_t i = 0; i < o_animation.tracks.size(); ++i) {
            const RawAnimation::JointTrack& o_track = o_animation.tracks[i];
            const RawAnimation::JointTrack& i_track = i_animation.tracks[i];
            for (size_t j = 0; j < o_track.translations.size(); ++j) {
                const RawAnimation::TranslationKey& o_key = o_track.translations[j];
                const RawAnimation::TranslationKey& i_key = i_track.translations[j];
                EXPECT_FLOAT_EQ(o_key.time, i_key.time);
                EXPECT_TRUE(o_key.value.isSimilar(i_key.value, 0.f));
            }
            for (size_t j = 0; j < o_track.rotations.size(); ++j) {
                const RawAnimation::RotationKey& o_key = o_track.rotations[j];
                const RawAnimation::RotationKey& i_key = i_track.rotations[j];
                EXPECT_FLOAT_EQ(o_key.time, i_key.time);
                EXPECT_TRUE(o_key.value.isSimilar(i_key.value, .1f));
            }
            for (size_t j = 0; j < o_track.scales.size(); ++j) {
                const RawAnimation::ScaleKey& o_key = o_track.scales[j];
                const RawAnimation::ScaleKey& i_key = i_track.scales[j];
                EXPECT_FLOAT_EQ(o_key.time, i_key.time);
                EXPECT_TRUE(o_key.value.isSimilar(i_key.value, 0.f));
            }
        }
    }
}

TEST(AlreadyInitialized, RawAnimationSerialize) {
    RawAnimation o_animation;
    o_animation.duration = 46.f;
    o_animation.tracks.resize(1);

    vox::io::MemoryStream stream;

    // Streams out.
    vox::io::OArchive o(&stream);
    o << o_animation;

    // Streams out a second time.
    o_animation.duration = 93.f;
    o_animation.tracks.resize(2);
    o << o_animation;

    // Streams in.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    RawAnimation i_animation;
    i >> i_animation;
    EXPECT_FLOAT_EQ(i_animation.duration, 46.f);
    ASSERT_EQ(i_animation.num_tracks(), 1);

    // A second time
    i >> i_animation;
    EXPECT_FLOAT_EQ(i_animation.duration, 93.f);
    ASSERT_EQ(i_animation.num_tracks(), 2);
}
