//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "test.animation/gtest_helper.h"
#include "test.animation/gtest_math_helper.h"
#include "vox.animation/offline/raw_animation.h"
#include "vox.animation/offline/raw_animation_utils.h"

using vox::animation::offline::RawAnimation;

TEST(SamplingTrackEmpty, Utils) {
    RawAnimation::JointTrack track;
    vox::ScalableTransform output;

    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, 0.f, &output));

    EXPECT_FLOAT3_EQ(output.translation, 0.f, 0.f, 0.f);
    EXPECT_QUATERNION_EQ(output.rotation, 0.f, 0.f, 0.f, 1.f);
    EXPECT_FLOAT3_EQ(output.scale, 1.f, 1.f, 1.f);
}

TEST(SamplingTrackInvalid, Utils) {
    // Key order
    {
        RawAnimation::JointTrack track;

        RawAnimation::TranslationKey t0 = {.9f, vox::Vector3F(1.f, 2.f, 4.f)};
        track.translations.push_back(t0);
        RawAnimation::TranslationKey t1 = {.1f, vox::Vector3F(2.f, 4.f, 8.f)};
        track.translations.push_back(t1);

        vox::ScalableTransform output;
        EXPECT_FALSE(vox::animation::offline::SampleTrack(track, 0.f, &output));
    }

    // Negative time
    {
        RawAnimation::JointTrack track;

        RawAnimation::TranslationKey t0 = {-1.f, vox::Vector3F(1.f, 2.f, 4.f)};
        track.translations.push_back(t0);

        vox::ScalableTransform output;
        EXPECT_FALSE(vox::animation::offline::SampleTrack(track, 0.f, &output));
    }
}

TEST(SamplingTrack, Utils) {
    RawAnimation::JointTrack track;

    RawAnimation::TranslationKey t0 = {.1f, vox::Vector3F(1.f, 2.f, 4.f)};
    track.translations.push_back(t0);
    RawAnimation::TranslationKey t1 = {.9f, vox::Vector3F(2.f, 4.f, 8.f)};
    track.translations.push_back(t1);

    RawAnimation::RotationKey r0 = {0.f, vox::QuaternionF(.70710677f, 0.f, 0.f, .70710677f)};
    track.rotations.push_back(r0);
    RawAnimation::RotationKey r1 = {// /!\ Negated (other hemisphepre) quaternion
                                    .5f, -vox::QuaternionF(0.f, .70710677f, 0.f, .70710677f)};
    track.rotations.push_back(r1);
    RawAnimation::RotationKey r2 = {1.f, vox::QuaternionF(0.f, .70710677f, 0.f, .70710677f)};
    track.rotations.push_back(r2);

    RawAnimation::ScaleKey s0 = {.5f, vox::Vector3F(-1.f, -2.f, -4.f)};
    track.scales.push_back(s0);

    vox::ScalableTransform output;

    // t = -.1
    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, -.1f, &output));
    EXPECT_FLOAT3_EQ(output.translation, 1.f, 2.f, 4.f);
    EXPECT_QUATERNION_EQ(output.rotation, .70710677f, 0.f, 0.f, .70710677f);
    EXPECT_FLOAT3_EQ(output.scale, -1.f, -2.f, -4.f);

    // t = 0
    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, 0.f, &output));
    EXPECT_FLOAT3_EQ(output.translation, 1.f, 2.f, 4.f);
    EXPECT_QUATERNION_EQ(output.rotation, .70710677f, 0.f, 0.f, .70710677f);
    EXPECT_FLOAT3_EQ(output.scale, -1.f, -2.f, -4.f);

    // t = .1
    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, .1f, &output));
    EXPECT_FLOAT3_EQ(output.translation, 1.f, 2.f, 4.f);
    EXPECT_QUATERNION_EQ(output.rotation, .6172133f, .1543033f, 0.f, .7715167f);
    EXPECT_FLOAT3_EQ(output.scale, -1.f, -2.f, -4.f);

    // t = .4999999
    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, .4999999f, &output));
    EXPECT_FLOAT3_EQ(output.translation, 1.5f, 3.f, 6.f);
    EXPECT_QUATERNION_EQ(output.rotation, 0.f, .70710677f, 0.f, .70710677f);
    EXPECT_FLOAT3_EQ(output.scale, -1.f, -2.f, -4.f);

    // t = .5
    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, .5f, &output));
    EXPECT_FLOAT3_EQ(output.translation, 1.5f, 3.f, 6.f);
    EXPECT_QUATERNION_EQ(output.rotation, 0.f, .70710677f, 0.f, .70710677f);
    EXPECT_FLOAT3_EQ(output.scale, -1.f, -2.f, -4.f);

    // t = .75
    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, .75f, &output));
    // Fixed up based on dot with previous quaternion
    EXPECT_QUATERNION_EQ(output.rotation, 0.f, -.70710677f, 0.f, -.70710677f);
    EXPECT_FLOAT3_EQ(output.scale, -1.f, -2.f, -4.f);

    // t= .9
    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, .9f, &output));
    EXPECT_FLOAT3_EQ(output.translation, 2.f, 4.f, 8.f);
    EXPECT_QUATERNION_EQ(output.rotation, 0.f, -.70710677f, 0.f, -.70710677f);
    EXPECT_FLOAT3_EQ(output.scale, -1.f, -2.f, -4.f);

    // t= 1.
    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, 1.f, &output));
    EXPECT_FLOAT3_EQ(output.translation, 2.f, 4.f, 8.f);
    EXPECT_QUATERNION_EQ(output.rotation, 0.f, .70710677f, 0.f, .70710677f);
    EXPECT_FLOAT3_EQ(output.scale, -1.f, -2.f, -4.f);

    // t= 1.1
    ASSERT_TRUE(vox::animation::offline::SampleTrack(track, 1.1f, &output));
    EXPECT_FLOAT3_EQ(output.translation, 2.f, 4.f, 8.f);
    EXPECT_QUATERNION_EQ(output.rotation, 0.f, .70710677f, 0.f, .70710677f);
    EXPECT_FLOAT3_EQ(output.scale, -1.f, -2.f, -4.f);
}

TEST(SamplingAnimation, Utils) {
    // Building an Animation with unsorted keys fails.
    RawAnimation raw_animation;
    raw_animation.duration = 2.f;
    raw_animation.tracks.resize(2);

    RawAnimation::TranslationKey a = {.2f, vox::Vector3F(-1.f, 0.f, 0.f)};
    raw_animation.tracks[0].translations.push_back(a);

    RawAnimation::TranslationKey b = {0.f, vox::Vector3F(2.f, 0.f, 0.f)};
    raw_animation.tracks[1].translations.push_back(b);
    RawAnimation::TranslationKey c = {0.2f, vox::Vector3F(6.f, 0.f, 0.f)};
    raw_animation.tracks[1].translations.push_back(c);
    RawAnimation::TranslationKey d = {0.4f, vox::Vector3F(8.f, 0.f, 0.f)};
    raw_animation.tracks[1].translations.push_back(d);

    vox::ScalableTransform output[2];

    // Too small
    {
        vox::ScalableTransform small[1];
        EXPECT_FALSE(vox::animation::offline::SampleAnimation(raw_animation, 0.f, small));
    }

    // Invalid, cos track are longer than duration
    {
        raw_animation.duration = .1f;
        EXPECT_FALSE(vox::animation::offline::SampleAnimation(raw_animation, 0.f, output));
        raw_animation.duration = 2.f;
    }

    ASSERT_TRUE(vox::animation::offline::SampleAnimation(raw_animation, -.1f, output));
    EXPECT_FLOAT3_EQ(output[0].translation, -1.f, 0.f, 0.f);
    EXPECT_QUATERNION_EQ(output[0].rotation, 0.f, 0.f, 0.f, 1.f);
    EXPECT_FLOAT3_EQ(output[0].scale, 1.f, 1.f, 1.f);
    EXPECT_FLOAT3_EQ(output[1].translation, 2.f, 0.f, 0.f);
    EXPECT_QUATERNION_EQ(output[1].rotation, 0.f, 0.f, 0.f, 1.f);
    EXPECT_FLOAT3_EQ(output[1].scale, 1.f, 1.f, 1.f);

    ASSERT_TRUE(vox::animation::offline::SampleAnimation(raw_animation, 0.f, output));
    EXPECT_FLOAT3_EQ(output[0].translation, -1.f, 0.f, 0.f);
    EXPECT_FLOAT3_EQ(output[1].translation, 2.f, 0.f, 0.f);

    ASSERT_TRUE(vox::animation::offline::SampleAnimation(raw_animation, .2f, output));
    EXPECT_FLOAT3_EQ(output[0].translation, -1.f, 0.f, 0.f);
    EXPECT_FLOAT3_EQ(output[1].translation, 6.f, 0.f, 0.f);

    ASSERT_TRUE(vox::animation::offline::SampleAnimation(raw_animation, .3f, output));
    EXPECT_FLOAT3_EQ(output[0].translation, -1.f, 0.f, 0.f);
    EXPECT_FLOAT3_EQ(output[1].translation, 7.f, 0.f, 0.f);

    ASSERT_TRUE(vox::animation::offline::SampleAnimation(raw_animation, .4f, output));
    EXPECT_FLOAT3_EQ(output[0].translation, -1.f, 0.f, 0.f);
    EXPECT_FLOAT3_EQ(output[1].translation, 8.f, 0.f, 0.f);

    ASSERT_TRUE(vox::animation::offline::SampleAnimation(raw_animation, 2.f, output));
    EXPECT_FLOAT3_EQ(output[0].translation, -1.f, 0.f, 0.f);
    EXPECT_FLOAT3_EQ(output[1].translation, 8.f, 0.f, 0.f);

    ASSERT_TRUE(vox::animation::offline::SampleAnimation(raw_animation, 3.f, output));
    EXPECT_FLOAT3_EQ(output[0].translation, -1.f, 0.f, 0.f);
    EXPECT_FLOAT3_EQ(output[1].translation, 8.f, 0.f, 0.f);
}

TEST(FixedRateSamplingTime, Utils) {
    {  // From 0
        vox::animation::offline::FixedRateSamplingTime it(1.f, 30.f);
        EXPECT_EQ(it.num_keys(), 31u);

        EXPECT_EQ(it.time(0), 0.f);
        EXPECT_FLOAT_EQ(it.time(1), 1.f / 30.f);
        EXPECT_FLOAT_EQ(it.time(2), 2.f / 30.f);
        EXPECT_FLOAT_EQ(it.time(29), 29.f / 30.f);
        EXPECT_EQ(it.time(30), 1.f);
        EXPECT_ASSERTION(it.time(31), "_key < num_keys");
    }

    {  // Offset
        vox::animation::offline::FixedRateSamplingTime it(3.f, 100.f);
        EXPECT_EQ(it.num_keys(), 301u);

        EXPECT_EQ(it.time(0), 0.f);
        EXPECT_FLOAT_EQ(it.time(1), 1.f / 100.f);
        EXPECT_FLOAT_EQ(it.time(2), 2.f / 100.f);
        EXPECT_FLOAT_EQ(it.time(299), 299.f / 100.f);
        EXPECT_EQ(it.time(300), 3.f);
    }

    {  // Ceil
        vox::animation::offline::FixedRateSamplingTime it(1.001f, 30.f);
        EXPECT_EQ(it.num_keys(), 32u);

        EXPECT_EQ(it.time(0), 0.f);
        EXPECT_FLOAT_EQ(it.time(1), 1.f / 30.f);
        EXPECT_FLOAT_EQ(it.time(2), 2.f / 30.f);
        EXPECT_FLOAT_EQ(it.time(29), 29.f / 30.f);
        EXPECT_FLOAT_EQ(it.time(30), 1.f);
        EXPECT_EQ(it.time(31), 1.001f);
    }

    {  // Long
        vox::animation::offline::FixedRateSamplingTime it(1000.f, 30.f);
        EXPECT_EQ(it.num_keys(), 30001u);

        EXPECT_EQ(it.time(0), 0.f);
        EXPECT_FLOAT_EQ(it.time(1), 1.f / 30.f);
        EXPECT_FLOAT_EQ(it.time(2), 2.f / 30.f);
        EXPECT_FLOAT_EQ(it.time(29999), 29999.f / 30.f);
        EXPECT_EQ(it.time(30000), 1000.f);
    }
}
