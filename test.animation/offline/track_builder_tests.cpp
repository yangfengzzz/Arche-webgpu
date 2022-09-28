//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <limits>

#include "gtest/gtest.h"
#include "test.animation/gtest_math_helper.h"
#include "vox.animation/offline/raw_track.h"
#include "vox.animation/offline/track_builder.h"
#include "vox.animation/runtime/track.h"
#include "vox.animation/runtime/track_sampling_job.h"
#include "vox.base/memory/unique_ptr.h"

using vox::animation::Float2Track;
using vox::animation::Float3Track;
using vox::animation::Float4Track;
using vox::animation::FloatTrack;
using vox::animation::FloatTrackSamplingJob;
using vox::animation::QuaternionTrack;
using vox::animation::offline::RawFloatTrack;
using vox::animation::offline::RawTrackInterpolation;
using vox::animation::offline::TrackBuilder;

TEST(Default, TrackBuilder) {
    // Instantiates a builder objects with default parameters.
    TrackBuilder builder;

    {  // Building default RawFloatTrack succeeds.
        RawFloatTrack raw_float_track;
        EXPECT_TRUE(raw_float_track.Validate());

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);
    }
}

TEST(Build, TrackBuilder) {
    // Instantiates a builder objects with default parameters.
    TrackBuilder builder;

    {  // Building a track with unsorted keys fails.
        RawFloatTrack raw_float_track;

        // Adds 2 unordered keys
        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, .8f, 0.f};
        raw_float_track.keyframes.push_back(first_key);
        const RawFloatTrack::Keyframe second_key = {RawTrackInterpolation::kLinear, .2f, 0.f};
        raw_float_track.keyframes.push_back(second_key);

        // Builds track
        EXPECT_FALSE(raw_float_track.Validate());
        EXPECT_TRUE(!builder(raw_float_track));
    }

    {  // Building a track with invalid key frame's ratio fails.
        RawFloatTrack raw_float_track;

        // Adds 2 unordered keys
        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, 1.8f, 0.f};
        raw_float_track.keyframes.push_back(first_key);

        // Builds track
        EXPECT_FALSE(raw_float_track.Validate());
        EXPECT_TRUE(!builder(raw_float_track));
    }

    {  // Building a track with equal key frame's ratio fails.
        RawFloatTrack raw_float_track;

        // Adds 2 unordered keys
        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, .8f, 0.f};
        raw_float_track.keyframes.push_back(first_key);
        const RawFloatTrack::Keyframe second_key = {RawTrackInterpolation::kLinear, .8f, 1.f};
        raw_float_track.keyframes.push_back(second_key);

        // Builds track
        EXPECT_FALSE(raw_float_track.Validate());
        EXPECT_TRUE(!builder(raw_float_track));
    }

    {  // Building a valid track with 1 key succeeds.
        RawFloatTrack raw_float_track;
        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, .8f, 0.f};
        raw_float_track.keyframes.push_back(first_key);
        EXPECT_TRUE(raw_float_track.Validate());

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);
    }
}

TEST(Name, TrackBuilder) {
    // Instantiates a builder objects with default parameters.
    TrackBuilder builder;

    {  // No name
        RawFloatTrack raw_float_track;

        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        EXPECT_STREQ(track->name(), "");
    }

    {  // A name
        RawFloatTrack raw_float_track;
        raw_float_track.name = "test name";

        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        EXPECT_STREQ(track->name(), raw_float_track.name.c_str());
    }
}

TEST(Build0Keys, TrackBuilder) {
    // Instantiates a builder objects with default parameters.
    TrackBuilder builder;

    RawFloatTrack raw_float_track;

    // Builds track
    vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
    EXPECT_TRUE(track);

    // Samples to verify build output.
    FloatTrackSamplingJob sampling;
    sampling.track = track.get();
    float result;
    sampling.result = &result;
    sampling.ratio = 0.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);
}

TEST(BuildLinear, TrackBuilder) {
    // Instantiates a builder objects with default parameters.
    TrackBuilder builder;
    float result;

    {  // 1 key at the beginning
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, 0.f, 46.f};
        raw_float_track.keyframes.push_back(first_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);
    }

    {  // 1 key in the middle
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f, 46.f};
        raw_float_track.keyframes.push_back(first_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);
    }

    {  // 1 key at the end
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, 1.f, 46.f};
        raw_float_track.keyframes.push_back(first_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);
    }

    {  // 2 keys
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f, 46.f};
        raw_float_track.keyframes.push_back(first_key);
        const RawFloatTrack::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f, 0.f};
        raw_float_track.keyframes.push_back(second_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .6f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 23.f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);
    }

    {  // n keys with same value
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe key1 = {RawTrackInterpolation::kLinear, .5f, 46.f};
        raw_float_track.keyframes.push_back(key1);
        const RawFloatTrack::Keyframe key2 = {RawTrackInterpolation::kLinear, .7f, 46.f};
        raw_float_track.keyframes.push_back(key2);
        const RawFloatTrack::Keyframe key3 = {RawTrackInterpolation::kLinear, .8f, 46.f};
        raw_float_track.keyframes.push_back(key3);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .6f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .75f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);
    }
}

TEST(BuildStep, TrackBuilder) {
    // Instantiates a builder objects with default parameters.
    TrackBuilder builder;
    float result;

    {  // 1 key at the beginning
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kStep, 0.f, 46.f};
        raw_float_track.keyframes.push_back(first_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);
    }

    {  // 1 key in the middle
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kStep, .5f, 46.f};
        raw_float_track.keyframes.push_back(first_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);
    }

    {  // 1 key at the end
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kStep, 1.f, 46.f};
        raw_float_track.keyframes.push_back(first_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);
    }

    {  // 1 key at the beginning
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kStep, 0.f, 46.f};
        raw_float_track.keyframes.push_back(first_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);
    }

    {  // 2 keys
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kStep, 0.f, 46.f};
        raw_float_track.keyframes.push_back(first_key);
        const RawFloatTrack::Keyframe second_key = {RawTrackInterpolation::kStep, .7f, 0.f};
        raw_float_track.keyframes.push_back(second_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f + 1e-7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .6f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .7f - 1e-7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);
    }

    {  // 3 keys
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kStep, 0.f, 46.f};
        raw_float_track.keyframes.push_back(first_key);
        const RawFloatTrack::Keyframe second_key = {RawTrackInterpolation::kStep, .7f, 0.f};
        raw_float_track.keyframes.push_back(second_key);
        const RawFloatTrack::Keyframe third_key = {RawTrackInterpolation::kStep, 1.f, 99.f};
        raw_float_track.keyframes.push_back(third_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f + 1e-7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .6f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .7f - 1e-7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);

        sampling.ratio = .9f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 99.f);
    }

    {  // 2 close keys
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kStep, .5f, 46.f};
        raw_float_track.keyframes.push_back(first_key);
        const RawFloatTrack::Keyframe second_key = {RawTrackInterpolation::kStep,
                                                    .5f + 2.f * std::numeric_limits<float>::epsilon(), 0.f};
        raw_float_track.keyframes.push_back(second_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f + std::numeric_limits<float>::epsilon();
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f + 2.f * std::numeric_limits<float>::epsilon();
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);
    }

    {  // 3 keys
        RawFloatTrack raw_float_track;

        const RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kStep, .5f, 46.f};
        raw_float_track.keyframes.push_back(first_key);
        const RawFloatTrack::Keyframe second_key = {RawTrackInterpolation::kStep, .7f, 0.f};
        raw_float_track.keyframes.push_back(second_key);
        const RawFloatTrack::Keyframe third_key = {RawTrackInterpolation::kStep, 1.f, 99.f};
        raw_float_track.keyframes.push_back(third_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .5f + 1e-7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .6f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .7f - 1e-7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);

        sampling.ratio = .7f + 1e-7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);

        sampling.ratio = 1.f - 1e-7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 99.f);
    }
}

TEST(BuildMixed, TrackBuilder) {
    // Instantiates a builder objects with default parameters.
    TrackBuilder builder;
    float result;

    RawFloatTrack raw_float_track;

    const RawFloatTrack::Keyframe key0 = {RawTrackInterpolation::kLinear, 0.f, 0.f};
    raw_float_track.keyframes.push_back(key0);
    const RawFloatTrack::Keyframe key1 = {RawTrackInterpolation::kStep, .5f, 46.f};
    raw_float_track.keyframes.push_back(key1);
    const RawFloatTrack::Keyframe key2 = {RawTrackInterpolation::kLinear, .7f, 0.f};
    raw_float_track.keyframes.push_back(key2);

    // Builds track
    vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
    EXPECT_TRUE(track);

    // Samples to verify build output.
    FloatTrackSamplingJob sampling;
    sampling.track = track.get();
    sampling.result = &result;

    sampling.ratio = 0.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);

    sampling.ratio = .25f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 23.f);

    sampling.ratio = .5f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 46.f);

    sampling.ratio = .5f + 1e-7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 46.f);

    sampling.ratio = .6f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 46.f);

    sampling.ratio = .7f - 1e-7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 46.f);

    sampling.ratio = .7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);

    sampling.ratio = 1.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);
}
TEST(Move, TrackBuilder) {
    TrackBuilder builder;
    RawFloatTrack raw_float_track;

    const RawFloatTrack::Keyframe key0 = {RawTrackInterpolation::kLinear, 0.f, 0.f};
    raw_float_track.keyframes.push_back(key0);
    const RawFloatTrack::Keyframe key1 = {RawTrackInterpolation::kStep, .5f, 46.f};
    raw_float_track.keyframes.push_back(key1);
    const RawFloatTrack::Keyframe key2 = {RawTrackInterpolation::kLinear, .7f, 0.f};
    raw_float_track.keyframes.push_back(key2);

    {  // Move constructor
        raw_float_track.name = "track1";
        vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
        const FloatTrack ctrack(std::move(*track));
        EXPECT_STREQ(ctrack.name(), "track1");
    }

    {  // Move assignment
        raw_float_track.name = "track1";
        vox::unique_ptr<FloatTrack> track1(builder(raw_float_track));
        EXPECT_STREQ(track1->name(), "track1");

        raw_float_track.name = "track2";
        vox::unique_ptr<FloatTrack> track2(builder(raw_float_track));
        EXPECT_STREQ(track2->name(), "track2");

        *track2 = std::move(*track1);
        EXPECT_STREQ(track2->name(), "track1");
    }
}

TEST(Float, TrackBuilder) {
    TrackBuilder builder;
    vox::animation::offline::RawFloatTrack raw_track;

    {
        // Default value for quaternion is identity.
        vox::unique_ptr<FloatTrack> track(builder(raw_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        float result;
        vox::animation::FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 0.f);
    }

    {
        const vox::animation::offline::RawFloatTrack::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f, 23.f};
        raw_track.keyframes.push_back(first_key);
        const vox::animation::offline::RawFloatTrack::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f, 46.f};
        raw_track.keyframes.push_back(second_key);

        // Builds track
        vox::unique_ptr<FloatTrack> track(builder(raw_track));
        ASSERT_TRUE(track);

        // Samples to verify build output.
        float result;
        vox::animation::FloatTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 23.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 23.f);

        sampling.ratio = .6f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 34.5f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT_EQ(result, 46.f);
    }
}

TEST(Float2, TrackBuilder) {
    TrackBuilder builder;
    vox::animation::offline::RawFloat2Track raw_track;

    {
        // Default value for quaternion is identity.
        vox::unique_ptr<Float2Track> track(builder(raw_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        vox::Vector2F result;
        vox::animation::Float2TrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT2_EQ(result, 0.f, 0.f);
    }

    {
        const vox::animation::offline::RawFloat2Track::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f,
                                                                             vox::Vector2F(0.f, 23.f)};
        raw_track.keyframes.push_back(first_key);
        const vox::animation::offline::RawFloat2Track::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f,
                                                                              vox::Vector2F(23.f, 46.f)};
        raw_track.keyframes.push_back(second_key);

        // Builds track
        vox::unique_ptr<Float2Track> track(builder(raw_track));
        ASSERT_TRUE(track);

        // Samples to verify build output.
        vox::Vector2F result;
        vox::animation::Float2TrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT2_EQ(result, 0.f, 23.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT2_EQ(result, 0.f, 23.f);

        sampling.ratio = .6f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT2_EQ(result, 11.5f, 34.5f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT2_EQ(result, 23.f, 46.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT2_EQ(result, 23.f, 46.f);
    }
}

TEST(Float3, TrackBuilder) {
    TrackBuilder builder;
    vox::animation::offline::RawFloat3Track raw_track;

    {
        // Default value for quaternion is identity.
        vox::unique_ptr<Float3Track> track(builder(raw_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        vox::Vector3F result;
        vox::animation::Float3TrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT3_EQ(result, 0.f, 0.f, 0.f);
    }

    {
        const vox::animation::offline::RawFloat3Track::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f,
                                                                             vox::Vector3F(0.f, 23.f, 46.f)};
        raw_track.keyframes.push_back(first_key);
        const vox::animation::offline::RawFloat3Track::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f,
                                                                              vox::Vector3F(23.f, 46.f, 92.f)};
        raw_track.keyframes.push_back(second_key);

        // Builds track
        vox::unique_ptr<Float3Track> track(builder(raw_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        vox::Vector3F result;
        vox::animation::Float3TrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT3_EQ(result, 0.f, 23.f, 46.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT3_EQ(result, 0.f, 23.f, 46.f);

        sampling.ratio = .6f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT3_EQ(result, 11.5f, 34.5f, 69.f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT3_EQ(result, 23.f, 46.f, 92.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT3_EQ(result, 23.f, 46.f, 92.f);
    }
}

TEST(Float4, TrackBuilder) {
    TrackBuilder builder;
    vox::animation::offline::RawFloat4Track raw_track;

    {
        // Default value for quaternion is identity.
        vox::unique_ptr<Float4Track> track(builder(raw_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        vox::Vector4F result;
        vox::animation::Float4TrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT4_EQ(result, 0.f, 0.f, 0.f, 0.f);
    }

    {
        const vox::animation::offline::RawFloat4Track::Keyframe first_key = {RawTrackInterpolation::kLinear, .5f,
                                                                             vox::Vector4F(0.f, 23.f, 46.f, 5.f)};
        raw_track.keyframes.push_back(first_key);
        const vox::animation::offline::RawFloat4Track::Keyframe second_key = {RawTrackInterpolation::kLinear, .7f,
                                                                              vox::Vector4F(23.f, 46.f, 92.f, 25.f)};
        raw_track.keyframes.push_back(second_key);

        // Builds track
        vox::unique_ptr<Float4Track> track(builder(raw_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        vox::Vector4F result;
        vox::animation::Float4TrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT4_EQ(result, 0.f, 23.f, 46.f, 5.f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT4_EQ(result, 0.f, 23.f, 46.f, 5.f);

        sampling.ratio = .6f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT4_EQ(result, 11.5f, 34.5f, 69.f, 15.f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT4_EQ(result, 23.f, 46.f, 92.f, 25.f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_FLOAT4_EQ(result, 23.f, 46.f, 92.f, 25.f);
    }
}

TEST(Quaternion, TrackBuilder) {
    TrackBuilder builder;
    vox::animation::offline::RawQuaternionTrack raw_track;

    {
        // Default value for quaternion is identity.
        vox::unique_ptr<QuaternionTrack> track(builder(raw_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        vox::QuaternionF result;
        vox::animation::QuaternionTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_QUATERNION_EQ(result, 0.f, 0.f, 0.f, 1.f);
    }

    {
        const vox::animation::offline::RawQuaternionTrack::Keyframe key0 = {
                RawTrackInterpolation::kLinear, .5f,
                vox::QuaternionF(-.70710677f, -0.f, -0.f,
                                 -.70710677f)};  // Will be opposed to be on the other hemisphere
        raw_track.keyframes.push_back(key0);
        const vox::animation::offline::RawQuaternionTrack::Keyframe key1 = {
                RawTrackInterpolation::kLinear, .7f, vox::QuaternionF(0.f, .70710677f, 0.f, .70710677f)};
        raw_track.keyframes.push_back(key1);
        const vox::animation::offline::RawQuaternionTrack::Keyframe key2 = {
                RawTrackInterpolation::kLinear, .8f, vox::QuaternionF(-0.f, -.70710677f, -0.f, -.70710677f)};
        raw_track.keyframes.push_back(key2);

        // Builds track
        vox::unique_ptr<QuaternionTrack> track(builder(raw_track));
        EXPECT_TRUE(track);

        // Samples to verify build output.
        vox::QuaternionF result;
        vox::animation::QuaternionTrackSamplingJob sampling;
        sampling.track = track.get();
        sampling.result = &result;

        sampling.ratio = 0.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_QUATERNION_EQ(result, .70710677f, 0.f, 0.f, .70710677f);

        sampling.ratio = .5f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_QUATERNION_EQ(result, .70710677f, 0.f, 0.f, .70710677f);

        sampling.ratio = .54f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_QUATERNION_EQ(result, .61721331f, .15430345f, 0.f, .77151674f);

        sampling.ratio = .7f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_QUATERNION_EQ(result, 0.f, .70710677f, 0.f, .70710677f);

        sampling.ratio = .75f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_QUATERNION_EQ(result, 0.f, .70710677f, 0.f, .70710677f);

        sampling.ratio = .8f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_QUATERNION_EQ(result, 0.f, .70710677f, 0.f, .70710677f);

        sampling.ratio = 1.f;
        ASSERT_TRUE(sampling.Run());
        EXPECT_QUATERNION_EQ(result, 0.f, .70710677f, 0.f, .70710677f);
    }
}
