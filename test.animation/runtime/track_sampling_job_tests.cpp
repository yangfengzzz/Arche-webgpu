//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "test.animation/gtest_math_helper.h"
#include "vox.base/memory/unique_ptr.h"
#include "vox.animation/offline/raw_track.h"
#include "vox.animation/offline/track_builder.h"
#include "vox.animation/runtime/track.h"
#include "vox.animation/runtime/track_sampling_job.h"

using vox::animation::Float2Track;
using vox::animation::Float3Track;
using vox::animation::Float4Track;
using vox::animation::FloatTrack;
using vox::animation::FloatTrackSamplingJob;
using vox::animation::QuaternionTrack;
using vox::animation::offline::RawFloatTrack;
using vox::animation::offline::RawTrackInterpolation;
using vox::animation::offline::TrackBuilder;

TEST(JobValidity, TrackSamplingJob) {
    // Instantiates a builder objects with default parameters.
    TrackBuilder builder;

    // Building default RawFloatTrack succeeds.
    RawFloatTrack raw_float_track;
    EXPECT_TRUE(raw_float_track.Validate());

    // Builds track
    vox::unique_ptr<FloatTrack> track(builder(raw_float_track));
    ASSERT_TRUE(track);

    {  // Empty/default job
        FloatTrackSamplingJob job;
        EXPECT_FALSE(job.Validate());
        EXPECT_FALSE(job.Run());
    }

    {  // Invalid output
        FloatTrackSamplingJob job;
        job.track = track.get();
        EXPECT_FALSE(job.Validate());
        EXPECT_FALSE(job.Run());
    }

    {  // Invalid track.
        FloatTrackSamplingJob job;
        float result;
        job.result = &result;
        EXPECT_FALSE(job.Validate());
        EXPECT_FALSE(job.Run());
    }

    {  // Valid
        FloatTrackSamplingJob job;
        job.track = track.get();
        float result;
        job.result = &result;
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());
    }
}

TEST(Default, TrackSamplingJob) {
    FloatTrack default_track;
    FloatTrackSamplingJob job;
    job.track = &default_track;
    float result = 1.f;
    job.result = &result;
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_FLOAT_EQ(result, 0.f);
}

TEST(Bounds, TrackSamplingJob) {
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
    ASSERT_TRUE(track);

    // Samples to verify build output.
    FloatTrackSamplingJob sampling;
    sampling.track = track.get();
    sampling.result = &result;

    sampling.ratio = 0.f - 1e-7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);

    sampling.ratio = 0.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);

    sampling.ratio = .5f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 46.f);

    sampling.ratio = 1.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);

    sampling.ratio = 1.f + 1e-7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);

    sampling.ratio = 1.5f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);
}

TEST(Float, TrackSamplingJob) {
    TrackBuilder builder;
    float result;

    vox::animation::offline::RawFloatTrack raw_track;

    const vox::animation::offline::RawFloatTrack::Keyframe key0 = {RawTrackInterpolation::kLinear, 0.f, 0.f};
    raw_track.keyframes.push_back(key0);
    const vox::animation::offline::RawFloatTrack::Keyframe key1 = {RawTrackInterpolation::kStep, .5f, 4.6f};
    raw_track.keyframes.push_back(key1);
    const vox::animation::offline::RawFloatTrack::Keyframe key2 = {RawTrackInterpolation::kLinear, .7f, 9.2f};
    raw_track.keyframes.push_back(key2);
    const vox::animation::offline::RawFloatTrack::Keyframe key3 = {RawTrackInterpolation::kLinear, .9f, 0.f};
    raw_track.keyframes.push_back(key3);

    // Builds track
    vox::unique_ptr<FloatTrack> track(builder(raw_track));
    ASSERT_TRUE(track);

    // Samples to verify build output.
    vox::animation::FloatTrackSamplingJob sampling;
    sampling.track = track.get();
    sampling.result = &result;

    sampling.ratio = 0.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);

    sampling.ratio = .25f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 2.3f);

    sampling.ratio = .5f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 4.6f);

    sampling.ratio = .6f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 4.6f);

    sampling.ratio = .7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 9.2f);

    sampling.ratio = .8f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 4.6f);

    sampling.ratio = .9f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);

    sampling.ratio = 1.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT_EQ(result, 0.f);
}

TEST(Float2, TrackSamplingJob) {
    TrackBuilder builder;
    vox::math::Float2 result;

    vox::animation::offline::RawFloat2Track raw_track;

    const vox::animation::offline::RawFloat2Track::Keyframe key0 = {RawTrackInterpolation::kLinear, 0.f,
                                                                    vox::Vector2F(0.f, 0.f)};
    raw_track.keyframes.push_back(key0);
    const vox::animation::offline::RawFloat2Track::Keyframe key1 = {RawTrackInterpolation::kStep, .5f,
                                                                    vox::Vector2F(2.3f, 4.6f)};
    raw_track.keyframes.push_back(key1);
    const vox::animation::offline::RawFloat2Track::Keyframe key2 = {RawTrackInterpolation::kLinear, .7f,
                                                                    vox::Vector2F(4.6f, 9.2f)};
    raw_track.keyframes.push_back(key2);
    const vox::animation::offline::RawFloat2Track::Keyframe key3 = {RawTrackInterpolation::kLinear, .9f,
                                                                    vox::Vector2F(0.f, 0.f)};
    raw_track.keyframes.push_back(key3);

    // Builds track
    vox::unique_ptr<Float2Track> track(builder(raw_track));
    ASSERT_TRUE(track);

    // Samples to verify build output.
    vox::animation::Float2TrackSamplingJob sampling;
    sampling.track = track.get();
    sampling.result = &result;

    sampling.ratio = 0.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT2_EQ(result, 0.f, 0.f);

    sampling.ratio = .25f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT2_EQ(result, 1.15f, 2.3f);

    sampling.ratio = .5f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT2_EQ(result, 2.3f, 4.6f);

    sampling.ratio = .6f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT2_EQ(result, 2.3f, 4.6f);

    sampling.ratio = .7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT2_EQ(result, 4.6f, 9.2f);

    sampling.ratio = .8f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT2_EQ(result, 2.3f, 4.6f);

    sampling.ratio = .9f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT2_EQ(result, 0.f, 0.f);

    sampling.ratio = 1.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT2_EQ(result, 0.f, 0.f);
}

TEST(Float3, TrackSamplingJob) {
    TrackBuilder builder;
    vox::Vector3F result;

    vox::animation::offline::RawFloat3Track raw_track;

    const vox::animation::offline::RawFloat3Track::Keyframe key0 = {RawTrackInterpolation::kLinear, 0.f,
                                                                    vox::Vector3F(0.f, 0.f, 0.f)};
    raw_track.keyframes.push_back(key0);
    const vox::animation::offline::RawFloat3Track::Keyframe key1 = {RawTrackInterpolation::kStep, .5f,
                                                                    vox::Vector3F(0.f, 2.3f, 4.6f)};
    raw_track.keyframes.push_back(key1);
    const vox::animation::offline::RawFloat3Track::Keyframe key2 = {RawTrackInterpolation::kLinear, .7f,
                                                                    vox::Vector3F(0.f, 4.6f, 9.2f)};
    raw_track.keyframes.push_back(key2);
    const vox::animation::offline::RawFloat3Track::Keyframe key3 = {RawTrackInterpolation::kLinear, .9f,
                                                                    vox::Vector3F(0.f, 0.f, 0.f)};
    raw_track.keyframes.push_back(key3);

    // Builds track
    vox::unique_ptr<Float3Track> track(builder(raw_track));
    ASSERT_TRUE(track);

    // Samples to verify build output.
    vox::animation::Float3TrackSamplingJob sampling;
    sampling.track = track.get();
    sampling.result = &result;

    sampling.ratio = 0.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT3_EQ(result, 0.f, 0.f, 0.f);

    sampling.ratio = .25f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT3_EQ(result, 0.f, 1.15f, 2.3f);

    sampling.ratio = .5f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT3_EQ(result, 0.f, 2.3f, 4.6f);

    sampling.ratio = .6f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT3_EQ(result, 0.f, 2.3f, 4.6f);

    sampling.ratio = .7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT3_EQ(result, 0.f, 4.6f, 9.2f);

    sampling.ratio = .8f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT3_EQ(result, 0.f, 2.3f, 4.6f);

    sampling.ratio = .9f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT3_EQ(result, 0.f, 0.f, 0.f);

    sampling.ratio = 1.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT3_EQ(result, 0.f, 0.f, 0.f);
}

TEST(Float4, TrackSamplingJob) {
    TrackBuilder builder;
    vox::Vector4F result;

    vox::animation::offline::RawFloat4Track raw_track;

    const vox::animation::offline::RawFloat4Track::Keyframe key0 = {RawTrackInterpolation::kLinear, 0.f,
                                                                    vox::Vector4F(0.f, 0.f, 0.f, 0.f)};
    raw_track.keyframes.push_back(key0);
    const vox::animation::offline::RawFloat4Track::Keyframe key1 = {RawTrackInterpolation::kStep, .5f,
                                                                    vox::Vector4F(0.f, 2.3f, 0.f, 4.6f)};
    raw_track.keyframes.push_back(key1);
    const vox::animation::offline::RawFloat4Track::Keyframe key2 = {RawTrackInterpolation::kLinear, .7f,
                                                                    vox::Vector4F(0.f, 4.6f, 0.f, 9.2f)};
    raw_track.keyframes.push_back(key2);
    const vox::animation::offline::RawFloat4Track::Keyframe key3 = {RawTrackInterpolation::kLinear, .9f,
                                                                    vox::Vector4F(0.f, 0.f, 0.f, 0.f)};
    raw_track.keyframes.push_back(key3);

    // Builds track
    vox::unique_ptr<Float4Track> track(builder(raw_track));
    ASSERT_TRUE(track);

    // Samples to verify build output.
    vox::animation::Float4TrackSamplingJob sampling;
    sampling.track = track.get();
    sampling.result = &result;

    sampling.ratio = 0.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT4_EQ(result, 0.f, 0.f, 0.f, 0.f);

    sampling.ratio = .25f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT4_EQ(result, 0.f, 1.15f, 0.f, 2.3f);

    sampling.ratio = .5f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT4_EQ(result, 0.f, 2.3f, 0.f, 4.6f);

    sampling.ratio = .6f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT4_EQ(result, 0.f, 2.3f, 0.f, 4.6f);

    sampling.ratio = .7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT4_EQ(result, 0.f, 4.6f, 0.f, 9.2f);

    sampling.ratio = .8f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT4_EQ(result, 0.f, 2.3f, 0.f, 4.6f);

    sampling.ratio = .9f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT4_EQ(result, 0.f, 0.f, 0.f, 0.f);

    sampling.ratio = 1.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_FLOAT4_EQ(result, 0.f, 0.f, 0.f, 0.f);
}

TEST(Quaternion, TrackSamplingJob) {
    TrackBuilder builder;
    vox::QuaternionF result;

    vox::animation::offline::RawQuaternionTrack raw_track;

    const vox::animation::offline::RawQuaternionTrack::Keyframe key0 = {
            RawTrackInterpolation::kLinear, 0.f, vox::QuaternionF(.70710677f, 0.f, 0.f, .70710677f)};
    raw_track.keyframes.push_back(key0);
    const vox::animation::offline::RawQuaternionTrack::Keyframe key1 = {
            RawTrackInterpolation::kStep, .5f, vox::QuaternionF(0.f, .70710677f, 0.f, .70710677f)};
    raw_track.keyframes.push_back(key1);
    const vox::animation::offline::RawQuaternionTrack::Keyframe key2 = {
            RawTrackInterpolation::kLinear, .7f, vox::QuaternionF(.70710677f, 0.f, 0.f, .70710677f)};
    raw_track.keyframes.push_back(key2);
    const vox::animation::offline::RawQuaternionTrack::Keyframe key3 = {RawTrackInterpolation::kLinear, .9f,
                                                                        vox::QuaternionF::makeIdentity()};
    raw_track.keyframes.push_back(key3);

    // Builds track
    vox::unique_ptr<QuaternionTrack> track(builder(raw_track));
    ASSERT_TRUE(track);
    // Samples to verify build output.
    vox::animation::QuaternionTrackSamplingJob sampling;
    sampling.track = track.get();
    sampling.result = &result;

    sampling.ratio = 0.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_QUATERNION_EQ(result, .70710677f, 0.f, 0.f, .70710677f);

    sampling.ratio = .1f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_QUATERNION_EQ(result, .61721331f, .15430345f, 0.f, .77151674f);

    sampling.ratio = .4999999f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_QUATERNION_EQ(result, 0.f, .70710677f, 0.f, .70710677f);

    sampling.ratio = .5f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_QUATERNION_EQ(result, 0.f, .70710677f, 0.f, .70710677f);

    sampling.ratio = .6f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_QUATERNION_EQ(result, 0.f, .70710677f, 0.f, .70710677f);

    sampling.ratio = .7f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_QUATERNION_EQ(result, .70710677f, 0.f, 0.f, .70710677f);

    sampling.ratio = .8f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_QUATERNION_EQ(result, .38268333f, 0.f, 0.f, .92387962f);

    sampling.ratio = .9f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_QUATERNION_EQ(result, 0.f, 0.f, 0.f, 1.f);

    sampling.ratio = 1.f;
    ASSERT_TRUE(sampling.Run());
    EXPECT_QUATERNION_EQ(result, 0.f, 0.f, 0.f, 1.f);
}
