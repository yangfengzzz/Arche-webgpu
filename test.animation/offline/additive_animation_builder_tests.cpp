//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "test.animation/gtest_math_helper.h"
#include "vox.animation/offline/additive_animation_builder.h"
#include "vox.animation/offline/raw_animation.h"
#include "vox.base/constants.h"
#include "vox.math/scalable_transform.h"

using vox::animation::offline::AdditiveAnimationBuilder;
using vox::animation::offline::RawAnimation;

TEST(Error, AdditiveAnimationBuilder) {
    AdditiveAnimationBuilder builder;

    {  // nullptr output.
        RawAnimation input;
        EXPECT_TRUE(input.Validate());

        // Builds animation
        EXPECT_FALSE(builder(input, nullptr));
    }

    {  // Invalid input animation.
        RawAnimation input;
        input.duration = -1.f;
        EXPECT_FALSE(input.Validate());

        // Builds animation
        RawAnimation output;
        output.duration = -1.f;
        output.tracks.resize(1);
        EXPECT_FALSE(builder(input, &output));
        EXPECT_FLOAT_EQ(output.duration, RawAnimation().duration);
        EXPECT_EQ(output.num_tracks(), 0);
    }

    {  // Invalid input animation with custom refpose
        RawAnimation input;
        input.duration = 1.f;
        input.tracks.resize(1);

        // Builds animation
        RawAnimation output;
        output.duration = -1.f;
        output.tracks.resize(1);

        vox::span<vox::ScalableTransform> empty_ref_pose_range;

        EXPECT_FALSE(builder(input, empty_ref_pose_range, &output));
        EXPECT_FLOAT_EQ(output.duration, RawAnimation().duration);
        EXPECT_EQ(output.num_tracks(), 0);
    }
}

TEST(Build, AdditiveAnimationBuilder) {
    AdditiveAnimationBuilder builder;

    RawAnimation input;
    input.name = "test";
    input.duration = 1.f;
    input.tracks.resize(3);

    // First track is empty
    {
            // input.tracks[0]
    }

    // 2nd track
    // 1 key at the beginning
    {
        const RawAnimation::TranslationKey key = {0.f, vox::Vector3F(2.f, 3.f, 4.f)};
        input.tracks[1].translations.push_back(key);
    }
    {
        const RawAnimation::RotationKey key = {0.f, vox::QuaternionF(.70710677f, 0.f, 0.f, .70710677f)};
        input.tracks[1].rotations.push_back(key);
    }
    {
        const RawAnimation::ScaleKey key = {0.f, vox::Vector3F(5.f, 6.f, 7.f)};
        input.tracks[1].scales.push_back(key);
    }

    // 3rd track
    // 2 keys after the beginning
    {
        const RawAnimation::TranslationKey key0 = {.5f, vox::Vector3F(2.f, 3.f, 4.f)};
        input.tracks[2].translations.push_back(key0);
        const RawAnimation::TranslationKey key1 = {.7f, vox::Vector3F(20.f, 30.f, 40.f)};
        input.tracks[2].translations.push_back(key1);
    }
    {
        const RawAnimation::RotationKey key0 = {.5f, vox::QuaternionF(.70710677f, 0.f, 0.f, .70710677f)};
        input.tracks[2].rotations.push_back(key0);
        const RawAnimation::RotationKey key1 = {.7f, vox::QuaternionF(-.70710677f, 0.f, 0.f, .70710677f)};
        input.tracks[2].rotations.push_back(key1);
    }
    {
        const RawAnimation::ScaleKey key0 = {.5f, vox::Vector3F(5.f, 6.f, 7.f)};
        input.tracks[2].scales.push_back(key0);
        const RawAnimation::ScaleKey key1 = {.7f, vox::Vector3F(50.f, 60.f, 70.f)};
        input.tracks[2].scales.push_back(key1);
    }

    // Builds animation with very little tolerance.
    {
        RawAnimation output;
        ASSERT_TRUE(builder(input, &output));
        EXPECT_STREQ(output.name.c_str(), input.name.c_str());
        EXPECT_EQ(output.num_tracks(), 3);

        // 1st track.
        {
            EXPECT_EQ(output.tracks[0].translations.size(), 0u);
            EXPECT_EQ(output.tracks[0].rotations.size(), 0u);
            EXPECT_EQ(output.tracks[0].scales.size(), 0u);
        }

        // 2nd track.
        {
            const RawAnimation::JointTrack::Translations& translations = output.tracks[1].translations;
            EXPECT_EQ(translations.size(), 1u);
            EXPECT_FLOAT_EQ(translations[0].time, 0.f);
            EXPECT_FLOAT3_EQ(translations[0].value, 0.f, 0.f, 0.f);
            const RawAnimation::JointTrack::Rotations& rotations = output.tracks[1].rotations;
            EXPECT_EQ(rotations.size(), 1u);
            EXPECT_FLOAT_EQ(rotations[0].time, 0.f);
            EXPECT_QUATERNION_EQ(rotations[0].value, 0.f, 0.f, 0.f, 1.f);
            const RawAnimation::JointTrack::Scales& scales = output.tracks[1].scales;
            EXPECT_EQ(scales.size(), 1u);
            EXPECT_FLOAT_EQ(scales[0].time, 0.f);
            EXPECT_FLOAT3_EQ(scales[0].value, 1.f, 1.f, 1.f);
        }

        // 3rd track.
        {
            const RawAnimation::JointTrack::Translations& translations = output.tracks[2].translations;
            EXPECT_EQ(translations.size(), 2u);
            EXPECT_FLOAT_EQ(translations[0].time, .5f);
            EXPECT_FLOAT3_EQ(translations[0].value, 0.f, 0.f, 0.f);
            EXPECT_FLOAT_EQ(translations[1].time, .7f);
            EXPECT_FLOAT3_EQ(translations[1].value, 18.f, 27.f, 36.f);
            const RawAnimation::JointTrack::Rotations& rotations = output.tracks[2].rotations;
            EXPECT_EQ(rotations.size(), 2u);
            EXPECT_FLOAT_EQ(rotations[0].time, .5f);
            EXPECT_QUATERNION_EQ(rotations[0].value, 0.f, 0.f, 0.f, 1.f);
            EXPECT_FLOAT_EQ(rotations[1].time, .7f);
            EXPECT_QUATERNION_EQ(rotations[1].value, -1.f, 0.f, 0.f, 0.f);
            const RawAnimation::JointTrack::Scales& scales = output.tracks[2].scales;
            EXPECT_EQ(scales.size(), 2u);
            EXPECT_FLOAT_EQ(scales[0].time, .5f);
            EXPECT_FLOAT3_EQ(scales[0].value, 1.f, 1.f, 1.f);
            EXPECT_FLOAT_EQ(scales[1].time, .7f);
            EXPECT_FLOAT3_EQ(scales[1].value, 10.f, 10.f, 10.f);
        }
    }
}

TEST(BuildRefPose, AdditiveAnimationBuilder) {
    AdditiveAnimationBuilder builder;

    RawAnimation input;
    input.duration = 1.f;
    input.tracks.resize(3);
    input.name = "test";

    // First track is empty
    {
            // input.tracks[0]
    }

    // 2nd track
    // 1 key at the beginning
    {
        const RawAnimation::TranslationKey key = {0.f, vox::Vector3F(2.f, 3.f, 4.f)};
        input.tracks[1].translations.push_back(key);
    }
    {
        const RawAnimation::RotationKey key = {0.f, vox::QuaternionF(.70710677f, 0.f, 0.f, .70710677f)};
        input.tracks[1].rotations.push_back(key);
    }
    {
        const RawAnimation::ScaleKey key = {0.f, vox::Vector3F(5.f, 6.f, 7.f)};
        input.tracks[1].scales.push_back(key);
    }

    // 3rd track
    // 2 keys after the beginning
    {
        const RawAnimation::TranslationKey key0 = {.5f, vox::Vector3F(2.f, 3.f, 4.f)};
        input.tracks[2].translations.push_back(key0);
        const RawAnimation::TranslationKey key1 = {.7f, vox::Vector3F(20.f, 30.f, 40.f)};
        input.tracks[2].translations.push_back(key1);
    }
    {
        const RawAnimation::RotationKey key0 = {.5f, vox::QuaternionF(.70710677f, 0.f, 0.f, .70710677f)};
        input.tracks[2].rotations.push_back(key0);
        const RawAnimation::RotationKey key1 = {.7f, vox::QuaternionF(-.70710677f, 0.f, 0.f, .70710677f)};
        input.tracks[2].rotations.push_back(key1);
    }
    {
        const RawAnimation::ScaleKey key0 = {.5f, vox::Vector3F(5.f, 6.f, 7.f)};
        input.tracks[2].scales.push_back(key0);
        const RawAnimation::ScaleKey key1 = {.7f, vox::Vector3F(50.f, 60.f, 70.f)};
        input.tracks[2].scales.push_back(key1);
    }

    // Builds animation with a custom refpose & very little tolerance
    {
        vox::ScalableTransform ref_pose[3];
        ref_pose[0] = vox::ScalableTransform::identity();
        ref_pose[1].translation = vox::Vector3F(1.f, 1.f, 1.f);
        ref_pose[1].rotation = vox::QuaternionF(0.f, 0.f, .70710677f, .70710677f);
        ref_pose[1].scale = vox::Vector3F(1.f, -1.f, 2.f);
        ref_pose[2].translation = input.tracks[2].translations[0].value;
        ref_pose[2].rotation = input.tracks[2].rotations[0].value;
        ref_pose[2].scale = input.tracks[2].scales[0].value;

        RawAnimation output;
        ASSERT_TRUE(builder(input, vox::span<vox::ScalableTransform>(ref_pose), &output));
        EXPECT_STREQ(output.name.c_str(), input.name.c_str());
        EXPECT_EQ(output.num_tracks(), 3);

        // 1st track.
        {
            EXPECT_EQ(output.tracks[0].translations.size(), 0u);
            EXPECT_EQ(output.tracks[0].rotations.size(), 0u);
            EXPECT_EQ(output.tracks[0].scales.size(), 0u);
        }

        // 2nd track.
        {
            const RawAnimation::JointTrack::Translations& translations = output.tracks[1].translations;
            EXPECT_EQ(translations.size(), 1u);
            EXPECT_FLOAT_EQ(translations[0].time, 0.f);
            EXPECT_FLOAT3_EQ(translations[0].value, 1.f, 2.f, 3.f);
            const RawAnimation::JointTrack::Rotations& rotations = output.tracks[1].rotations;
            EXPECT_EQ(rotations.size(), 1u);
            EXPECT_FLOAT_EQ(rotations[0].time, 0.f);
            EXPECT_QUATERNION_EQ(rotations[0].value, .5f, .5f, -.5f, .5f);
            const RawAnimation::JointTrack::Scales& scales = output.tracks[1].scales;
            EXPECT_EQ(scales.size(), 1u);
            EXPECT_FLOAT_EQ(scales[0].time, 0.f);
            EXPECT_FLOAT3_EQ(scales[0].value, 5.f, -6.f, 3.5f);
        }

        // 3rd track.
        {
            const RawAnimation::JointTrack::Translations& translations = output.tracks[2].translations;
            EXPECT_EQ(translations.size(), 2u);
            EXPECT_FLOAT_EQ(translations[0].time, .5f);
            EXPECT_FLOAT3_EQ(translations[0].value, 0.f, 0.f, 0.f);
            EXPECT_FLOAT_EQ(translations[1].time, .7f);
            EXPECT_FLOAT3_EQ(translations[1].value, 18.f, 27.f, 36.f);
            const RawAnimation::JointTrack::Rotations& rotations = output.tracks[2].rotations;
            EXPECT_EQ(rotations.size(), 2u);
            EXPECT_FLOAT_EQ(rotations[0].time, .5f);
            EXPECT_QUATERNION_EQ(rotations[0].value, 0.f, 0.f, 0.f, 1.f);
            EXPECT_FLOAT_EQ(rotations[1].time, .7f);
            EXPECT_QUATERNION_EQ(rotations[1].value, -1.f, 0.f, 0.f, 0.f);
            const RawAnimation::JointTrack::Scales& scales = output.tracks[2].scales;
            EXPECT_EQ(scales.size(), 2u);
            EXPECT_FLOAT_EQ(scales[0].time, .5f);
            EXPECT_FLOAT3_EQ(scales[0].value, 1.f, 1.f, 1.f);
            EXPECT_FLOAT_EQ(scales[1].time, .7f);
            EXPECT_FLOAT3_EQ(scales[1].value, 10.f, 10.f, 10.f);
        }
    }
}
