//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "vox.base/io/archive.h"
#include "vox.base/io/stream.h"
#include "test.animation/gtest_math_helper.h"
#include "vox.simd_math/soa_transform.h"
#include "vox.base/memory/unique_ptr.h"
#include "vox.animation/offline/animation_builder.h"
#include "vox.animation/offline/raw_animation.h"
#include "vox.animation/runtime/animation.h"
#include "vox.animation/runtime/sampling_job.h"

using vox::animation::Animation;
using vox::animation::offline::AnimationBuilder;
using vox::animation::offline::RawAnimation;

TEST(Empty, AnimationSerialize) {
    vox::io::MemoryStream stream;

    // Streams out.
    vox::io::OArchive o(&stream, vox::getNativeEndianness());

    Animation o_animation;
    o << o_animation;

    // Streams in.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    Animation i_animation;
    i >> i_animation;

    EXPECT_EQ(o_animation.num_tracks(), i_animation.num_tracks());
}

TEST(Filled, AnimationSerialize) {
    // Builds a valid animation.
    vox::unique_ptr<Animation> o_animation;
    {
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(1);

        RawAnimation::TranslationKey t_key0 = {0.f, vox::Vector3F(93.f, 58.f, 46.f)};
        raw_animation.tracks[0].translations.push_back(t_key0);
        RawAnimation::TranslationKey t_key1 = {.9f, vox::Vector3F(46.f, 58.f, 93.f)};
        raw_animation.tracks[0].translations.push_back(t_key1);

        RawAnimation::RotationKey r_key = {0.7f, vox::QuaternionF(0.f, 1.f, 0.f, 0.f)};
        raw_animation.tracks[0].rotations.push_back(r_key);

        RawAnimation::ScaleKey s_key = {0.1f, vox::Vector3F(99.f, 26.f, 14.f)};
        raw_animation.tracks[0].scales.push_back(s_key);

        AnimationBuilder builder;
        o_animation = builder(raw_animation);
        ASSERT_TRUE(o_animation);
    }

    for (int e = 0; e < 2; ++e) {
        vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;
        vox::io::MemoryStream stream;

        // Streams out.
        vox::io::OArchive o(&stream, endianess);
        o << *o_animation;

        // Streams in.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive i(&stream);

        Animation i_animation;
        i >> i_animation;

        ASSERT_FLOAT_EQ(o_animation->duration(), i_animation.duration());
        ASSERT_EQ(o_animation->num_tracks(), i_animation.num_tracks());
        EXPECT_EQ(o_animation->size(), i_animation.size());

        // Needs to sample to test the animation.
        vox::animation::SamplingJob job;
        vox::animation::SamplingJob::Context context(1);
        vox::simd_math::SoaTransform output[1];
        job.animation = o_animation.get();
        job.context = &context;
        job.output = output;

        // Samples and compares the two animations
        {  // Samples at t = 0
            job.ratio = 0.f;
            job.Run();
            EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 93.f, 0.f, 0.f, 0.f, 58.f, 0.f, 0.f, 0.f, 46.f, 0.f, 0.f,
                                    0.f);
            EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                                        0.f, 1.f, 1.f, 1.f);
            EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 99.f, 1.f, 1.f, 1.f, 26.f, 1.f, 1.f, 1.f, 14.f, 1.f, 1.f, 1.f);
        }
        {  // Samples at t = 1
            job.ratio = 1.f;
            job.Run();
            EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 46.f, 0.f, 0.f, 0.f, 58.f, 0.f, 0.f, 0.f, 93.f, 0.f, 0.f,
                                    0.f);
            EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                                        0.f, 1.f, 1.f, 1.f);
            EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 99.f, 1.f, 1.f, 1.f, 26.f, 1.f, 1.f, 1.f, 14.f, 1.f, 1.f, 1.f);
        }
    }
}

TEST(AlreadyInitialized, AnimationSerialize) {
    vox::io::MemoryStream stream;

    {
        vox::io::OArchive o(&stream);

        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(1);

        AnimationBuilder builder;
        vox::unique_ptr<Animation> o_animation(builder(raw_animation));
        ASSERT_TRUE(o_animation);
        o << *o_animation;

        raw_animation.duration = 2.f;
        raw_animation.tracks.resize(2);
        o_animation = builder(raw_animation);
        ASSERT_TRUE(o_animation);
        o << *o_animation;
    }

    {
        // Streams in.
        stream.Seek(0, vox::io::Stream::kSet);
        vox::io::IArchive i(&stream);

        // Reads and check the first animation.
        Animation i_animation;
        i >> i_animation;
        EXPECT_FLOAT_EQ(i_animation.duration(), 1.f);
        EXPECT_EQ(i_animation.num_tracks(), 1);

        // Reuse the animation a second time.
        i >> i_animation;
        EXPECT_FLOAT_EQ(i_animation.duration(), 2.f);
        ASSERT_EQ(i_animation.num_tracks(), 2);
    }
}
