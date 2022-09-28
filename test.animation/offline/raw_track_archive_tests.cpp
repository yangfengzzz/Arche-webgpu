//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "vox.animation/offline/raw_track.h"
#include "vox.base/io/archive.h"

using vox::animation::offline::RawFloat2Track;
using vox::animation::offline::RawFloat3Track;
using vox::animation::offline::RawFloat4Track;
using vox::animation::offline::RawFloatTrack;
using vox::animation::offline::RawQuaternionTrack;
using vox::animation::offline::RawTrackInterpolation;

TEST(TrackEmpty, RawTrackSerialize) {
    vox::io::MemoryStream stream;

    // Streams out.
    vox::io::OArchive o(&stream, vox::getNativeEndianness());

    RawFloatTrack o_track;
    o << o_track;

    // Streams in.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    RawFloatTrack i_track;
    i >> i_track;

    EXPECT_EQ(o_track.keyframes.size(), i_track.keyframes.size());
    EXPECT_STREQ(o_track.name.c_str(), i_track.name.c_str());
}

TEST(TrackFilled, RawAnimationSerialize) {
    RawFloatTrack o_track;
    o_track.name = "test track";

    const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f, 46.f};
    o_track.keyframes.push_back(first_key);
    const RawFloatTrack::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f, 0.f};
    o_track.keyframes.push_back(second_key);

    EXPECT_TRUE(o_track.Validate());
    EXPECT_EQ(o_track.keyframes.size(), 2u);

    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;
        vox::io::MemoryStream stream;

        // Streams out.
        vox::io::OArchive o(&stream, endianess);
        o << o_track;

        // Streams in.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive ia(&stream);

        RawFloatTrack i_track;
        ia >> i_track;

        EXPECT_TRUE(i_track.Validate());
        ASSERT_EQ(o_track.keyframes.size(), i_track.keyframes.size());
        EXPECT_STREQ(o_track.name.c_str(), i_track.name.c_str());

        for (size_t i = 0; i < o_track.keyframes.size(); ++i) {
            const RawFloatTrack::Keyframe& o_key = o_track.keyframes[i];
            const RawFloatTrack::Keyframe& i_key = i_track.keyframes[i];

            EXPECT_EQ(o_key.interpolation, i_key.interpolation);
            EXPECT_FLOAT_EQ(o_key.ratio, i_key.ratio);
            EXPECT_EQ(o_key.value, i_key.value);
        }
    }
}

TEST(TrackAlreadyInitialized, RawAnimationSerialize) {
    RawFloatTrack o_track;

    vox::io::MemoryStream stream;

    // Streams out.
    vox::io::OArchive o(&stream);
    o_track.name = "test track";
    o << o_track;

    // Streams out a second ratio.
    o_track.keyframes.resize(2);
    o_track.name = "test track 2";
    o << o_track;

    // Streams in.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    RawFloatTrack i_track;
    i >> i_track;
    ASSERT_EQ(i_track.keyframes.size(), 0u);

    // A second ratio
    i >> i_track;
    ASSERT_EQ(i_track.keyframes.size(), 2u);
    EXPECT_STREQ(o_track.name.c_str(), i_track.name.c_str());
}

TEST(TrackFloat2, RawAnimationSerialize) {
    RawFloat2Track o_track;
    o_track.name = "test track";

    const RawFloat2Track::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f, vox::Vector2F(46.f, 99.f)};
    o_track.keyframes.push_back(first_key);
    const RawFloat2Track::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f, vox::Vector2F(16.f, 93.f)};
    o_track.keyframes.push_back(second_key);

    EXPECT_TRUE(o_track.Validate());
    EXPECT_EQ(o_track.keyframes.size(), 2u);

    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;
        vox::io::MemoryStream stream;

        // Streams out.
        vox::io::OArchive o(&stream, endianess);
        o << o_track;

        // Streams in.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive ia(&stream);

        RawFloat2Track i_track;
        ia >> i_track;

        EXPECT_TRUE(i_track.Validate());
        ASSERT_EQ(o_track.keyframes.size(), i_track.keyframes.size());
        EXPECT_STREQ(o_track.name.c_str(), i_track.name.c_str());

        for (size_t i = 0; i < o_track.keyframes.size(); ++i) {
            const RawFloat2Track::Keyframe& o_key = o_track.keyframes[i];
            const RawFloat2Track::Keyframe& i_key = i_track.keyframes[i];

            EXPECT_EQ(o_key.interpolation, i_key.interpolation);
            EXPECT_FLOAT_EQ(o_key.ratio, i_key.ratio);
            EXPECT_EQ(o_key.value, i_key.value);
        }
    }
}

TEST(TrackFloat3, RawAnimationSerialize) {
    RawFloat3Track o_track;

    const RawFloat3Track::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f, vox::Vector3F(46.f, 99.f, 25.f)};
    o_track.keyframes.push_back(first_key);
    const RawFloat3Track::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f, vox::Vector3F(16.f, 93.f, 4.f)};
    o_track.keyframes.push_back(second_key);

    EXPECT_TRUE(o_track.Validate());
    EXPECT_EQ(o_track.keyframes.size(), 2u);

    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;
        vox::io::MemoryStream stream;

        // Streams out.
        vox::io::OArchive o(&stream, endianess);
        o << o_track;

        // Streams in.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive ia(&stream);

        RawFloat3Track i_track;
        ia >> i_track;

        EXPECT_TRUE(i_track.Validate());
        ASSERT_EQ(o_track.keyframes.size(), i_track.keyframes.size());

        for (size_t i = 0; i < o_track.keyframes.size(); ++i) {
            const RawFloat3Track::Keyframe& o_key = o_track.keyframes[i];
            const RawFloat3Track::Keyframe& i_key = i_track.keyframes[i];

            EXPECT_EQ(o_key.interpolation, i_key.interpolation);
            EXPECT_FLOAT_EQ(o_key.ratio, i_key.ratio);
            EXPECT_EQ(o_key.value, i_key.value);
        }
    }
}

TEST(TrackFloat4, RawAnimationSerialize) {
    RawFloat4Track o_track;

    const RawFloat4Track::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f,
                                                vox::Vector4F(46.f, 99.f, 25.f, 5.f)};
    o_track.keyframes.push_back(first_key);
    const RawFloat4Track::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f,
                                                 vox::Vector4F(16.f, 93.f, 4.f, 46.f)};
    o_track.keyframes.push_back(second_key);

    EXPECT_TRUE(o_track.Validate());
    EXPECT_EQ(o_track.keyframes.size(), 2u);

    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;
        vox::io::MemoryStream stream;

        // Streams out.
        vox::io::OArchive o(&stream, endianess);
        o << o_track;

        // Streams in.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive ia(&stream);

        RawFloat4Track i_track;
        ia >> i_track;

        EXPECT_TRUE(i_track.Validate());
        ASSERT_EQ(o_track.keyframes.size(), i_track.keyframes.size());

        for (size_t i = 0; i < o_track.keyframes.size(); ++i) {
            const RawFloat4Track::Keyframe& o_key = o_track.keyframes[i];
            const RawFloat4Track::Keyframe& i_key = i_track.keyframes[i];

            EXPECT_EQ(o_key.interpolation, i_key.interpolation);
            EXPECT_FLOAT_EQ(o_key.ratio, i_key.ratio);
            EXPECT_EQ(o_key.value, i_key.value);
        }
    }
}

TEST(TrackQuaternion, RawAnimationSerialize) {
    RawQuaternionTrack o_track;

    const RawQuaternionTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f,
                                                    vox::QuaternionF(0.f, .70710677f, 0.f, .70710677f)};
    o_track.keyframes.push_back(first_key);
    const RawQuaternionTrack::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f,
                                                     vox::QuaternionF(.6172133f, .1543033f, 0.f, .7715167f)};
    o_track.keyframes.push_back(second_key);

    EXPECT_TRUE(o_track.Validate());
    EXPECT_EQ(o_track.keyframes.size(), 2u);

    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;
        vox::io::MemoryStream stream;

        // Streams out.
        vox::io::OArchive o(&stream, endianess);
        o << o_track;

        // Streams in.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive ia(&stream);

        RawQuaternionTrack i_track;
        ia >> i_track;

        EXPECT_TRUE(i_track.Validate());
        ASSERT_EQ(o_track.keyframes.size(), i_track.keyframes.size());

        for (size_t i = 0; i < o_track.keyframes.size(); ++i) {
            const RawQuaternionTrack::Keyframe& o_key = o_track.keyframes[i];
            const RawQuaternionTrack::Keyframe& i_key = i_track.keyframes[i];

            EXPECT_EQ(o_key.interpolation, i_key.interpolation);
            EXPECT_FLOAT_EQ(o_key.ratio, i_key.ratio);
            EXPECT_EQ(o_key.value, i_key.value);
        }
    }
}
