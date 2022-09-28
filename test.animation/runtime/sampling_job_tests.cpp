//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "test.animation/gtest_math_helper.h"
#include "vox.animation/offline/animation_builder.h"
#include "vox.animation/offline/raw_animation.h"
#include "vox.animation/runtime/animation.h"
#include "vox.animation/runtime/sampling_job.h"
#include "vox.simd_math/soa_transform.h"

using vox::animation::Animation;
using vox::animation::SamplingJob;
using vox::animation::offline::AnimationBuilder;
using vox::animation::offline::RawAnimation;

TEST(JobValidity, SamplingJob) {
    RawAnimation raw_animation;
    raw_animation.duration = 1.f;
    raw_animation.tracks.resize(1);

    AnimationBuilder builder;
    vox::unique_ptr<Animation> animation(builder(raw_animation));
    ASSERT_TRUE(animation);

    // Allocates context.
    SamplingJob::Context context(1);

    {  // Empty/default job
        SamplingJob job;
        EXPECT_FALSE(job.Validate());
        EXPECT_FALSE(job.Run());
    }

    {  // Invalid output
        SamplingJob job;
        job.animation = animation.get();
        job.context = &context;
        EXPECT_FALSE(job.Validate());
        EXPECT_FALSE(job.Run());
    }

    {  // Invalid animation.
        vox::simd_math::SoaTransform output[1];

        SamplingJob job;
        job.context = &context;
        job.output = output;
        EXPECT_FALSE(job.Validate());
        EXPECT_FALSE(job.Run());
    }

    {  // Invalid context.
        vox::simd_math::SoaTransform output[1];

        SamplingJob job;
        job.animation = animation.get();
        job.output = output;
        EXPECT_FALSE(job.Validate());
        EXPECT_FALSE(job.Run());
    }

    {  // Invalid context size.
        SamplingJob::Context zero_cache(0);
        vox::simd_math::SoaTransform output[1];

        SamplingJob job;
        job.animation = animation.get();
        job.context = &zero_cache;
        job.output = output;
        EXPECT_FALSE(job.Validate());
        EXPECT_FALSE(job.Run());
    }

    {  // Invalid job with empty output.
        vox::simd_math::SoaTransform* output = nullptr;
        SamplingJob job;
        job.ratio = 2155.f;  // Any time ratio can be set, it's clamped in unit interval.
        job.animation = animation.get();
        job.context = &context;
        job.output = vox::span<vox::simd_math::SoaTransform>(output, size_t(0));
        EXPECT_FALSE(job.Validate());
        EXPECT_FALSE(job.Run());
    }

    {  // valid job with output smaller than animation, but not empty.
        RawAnimation big_raw_animation;
        big_raw_animation.duration = 1.f;
        big_raw_animation.tracks.resize(2);
        vox::unique_ptr<Animation> big_animation(builder(big_raw_animation));
        ASSERT_TRUE(big_animation);

        vox::simd_math::SoaTransform output[1];
        SamplingJob job;
        job.ratio = 2155.f;  // Any time ratio can be set, it's clamped in unit interval.
        job.animation = big_animation.get();
        job.context = &context;
        job.output = output;
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());
    }

    {  // Valid job.
        vox::simd_math::SoaTransform output[1];
        SamplingJob job;
        job.ratio = 2155.f;  // Any time can be set.
        job.animation = animation.get();
        job.context = &context;
        job.output = output;
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());
    }

    {  // Valid job with bigger context.
        SamplingJob::Context big_cache(2);
        vox::simd_math::SoaTransform output[1];
        SamplingJob job;
        job.ratio = 2155.f;  // Any time can be set.
        job.animation = animation.get();
        job.context = &big_cache;
        job.output = output;
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());
    }

    {  // Valid job with bigger output.
        vox::simd_math::SoaTransform output[2];
        SamplingJob job;
        job.ratio = 2155.f;  // Any time can be set.
        job.animation = animation.get();
        job.context = &context;
        job.output = output;
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());
    }

    {  // Default animation.
        vox::simd_math::SoaTransform output[1];
        Animation default_animation;
        SamplingJob job;
        job.animation = &default_animation;
        job.context = &context;
        job.output = output;
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());
    }
}

TEST(Sampling, SamplingJob) {
    // Instantiates a builder objects with default parameters.
    AnimationBuilder builder;

    // Building an Animation with unsorted keys fails.
    RawAnimation raw_animation;
    raw_animation.duration = 1.f;
    raw_animation.tracks.resize(4);

    SamplingJob::Context context(4);

    // Raw animation inputs.
    //     0                 1
    // -----------------------
    // 0 - |  A              |
    // 1 - |                 |
    // 2 - B  C   D   E      F
    // 3 - |  G       H      |

    // Final animation.
    //     0                 1
    // -----------------------
    // 0 - A-1               4
    // 1 - 1                 5
    // 2 - B2 C6  D8 E10    F11
    // 3 - 3  G7     H9      12

    struct {
        float sample_time;
        float trans[12];
    } result[] = {{-.2f, {-1.f, 0.f, 2.f, 7.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.0f, {-1.f, 0.f, 2.f, 7.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.0000001f, {-1.f, 0.f, 2.f, 7.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.1f, {-1.f, 0.f, 4.f, 7.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.2f, {-1.f, 0.f, 6.f, 7.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.3f, {-1.f, 0.f, 7.f, 7.5, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.4f, {-1.f, 0.f, 8.f, 8.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.3999999f, {-1.f, 0.f, 8.f, 8.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.4000001f, {-1.f, 0.f, 8.f, 8.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.5f, {-1.f, 0.f, 9.f, 8.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.6f, {-1.f, 0.f, 10.f, 9.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.9999999f, {-1.f, 0.f, 11.f, 9.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {1.f, {-1.f, 0.f, 11.f, 9.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {1.000001f, {-1.f, 0.f, 11.f, 9.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.5f, {-1.f, 0.f, 9.f, 8.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.9999999f, {-1.f, 0.f, 11.f, 9.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}},
                  {.0000001f, {-1.f, 0.f, 2.f, 7.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}}};

    RawAnimation::TranslationKey a = {.2f, vox::Vector3F(-1.f, 0.f, 0.f)};
    raw_animation.tracks[0].translations.push_back(a);

    RawAnimation::TranslationKey b = {0.f, vox::Vector3F(2.f, 0.f, 0.f)};
    raw_animation.tracks[2].translations.push_back(b);
    RawAnimation::TranslationKey c = {0.2f, vox::Vector3F(6.f, 0.f, 0.f)};
    raw_animation.tracks[2].translations.push_back(c);
    RawAnimation::TranslationKey d = {0.4f, vox::Vector3F(8.f, 0.f, 0.f)};
    raw_animation.tracks[2].translations.push_back(d);
    RawAnimation::TranslationKey e = {0.6f, vox::Vector3F(10.f, 0.f, 0.f)};
    raw_animation.tracks[2].translations.push_back(e);
    RawAnimation::TranslationKey f = {1.f, vox::Vector3F(11.f, 0.f, 0.f)};
    raw_animation.tracks[2].translations.push_back(f);

    RawAnimation::TranslationKey g = {0.2f, vox::Vector3F(7.f, 0.f, 0.f)};
    raw_animation.tracks[3].translations.push_back(g);
    RawAnimation::TranslationKey h = {0.6f, vox::Vector3F(9.f, 0.f, 0.f)};
    raw_animation.tracks[3].translations.push_back(h);

    // Builds animation
    vox::unique_ptr<Animation> animation(builder(raw_animation));
    ASSERT_TRUE(animation);

    vox::simd_math::SoaTransform output[1];

    SamplingJob job;
    job.animation = animation.get();
    job.context = &context;
    job.output = output;

    for (auto& i : result) {
        memset(output, 0xde, sizeof(output));
        job.ratio = i.sample_time / animation->duration();
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());

        EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, i.trans[0], i.trans[1], i.trans[2], i.trans[3], i.trans[4],
                                i.trans[5], i.trans[6], i.trans[7], i.trans[8], i.trans[9], i.trans[10], i.trans[11]);
        EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                    1.f, 1.f, 1.f);
        EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);
    }
}

TEST(SamplingNoTrack, SamplingJob) {
    RawAnimation raw_animation;
    raw_animation.duration = 46.f;

    SamplingJob::Context context(1);

    AnimationBuilder builder;
    vox::unique_ptr<Animation> animation(builder(raw_animation));
    ASSERT_TRUE(animation);

    vox::simd_math::SoaTransform test_output[1];
    vox::simd_math::SoaTransform output[1];
    memset(test_output, 0xde, sizeof(test_output));
    memset(output, 0xde, sizeof(output));

    SamplingJob job;
    job.ratio = 0.f;
    job.animation = animation.get();
    job.context = &context;
    job.output = output;
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());

    // Tests output.
    EXPECT_EQ(memcmp(test_output, output, sizeof(output)), 0);
}

TEST(Sampling1Track0Key, SamplingJob) {
    RawAnimation raw_animation;
    raw_animation.duration = 46.f;
    raw_animation.tracks.resize(1);  // Adds a joint.

    SamplingJob::Context context(1);

    AnimationBuilder builder;
    vox::unique_ptr<Animation> animation(builder(raw_animation));
    ASSERT_TRUE(animation);

    vox::simd_math::SoaTransform output[1];

    SamplingJob job;
    job.animation = animation.get();
    job.context = &context;
    job.output = output;

    for (float t = -.2f; t < 1.2f; t += .1f) {
        memset(output, 0xde, sizeof(output));
        job.ratio = t;
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());
        EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                    1.f, 1.f, 1.f);
        EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);
    }
}

TEST(Sampling1Track1Key, SamplingJob) {
    RawAnimation raw_animation;
    raw_animation.duration = 46.f;
    raw_animation.tracks.resize(1);  // Adds a joint.

    SamplingJob::Context context(1);

    const RawAnimation::TranslationKey tkey = {.3f, vox::Vector3F(1.f, -1.f, 5.f)};
    raw_animation.tracks[0].translations.push_back(tkey);  // Adds a key.

    AnimationBuilder builder;
    vox::unique_ptr<Animation> animation(builder(raw_animation));
    ASSERT_TRUE(animation);

    vox::simd_math::SoaTransform output[1];

    SamplingJob job;
    job.animation = animation.get();
    job.context = &context;
    job.output = output;

    for (float t = -.2f; t < 1.2f; t += .1f) {
        memset(output, 0xde, sizeof(output));
        job.ratio = t;
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());
        EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, 0.f, 5.f, 0.f, 0.f, 0.f);
        EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                    1.f, 1.f, 1.f);
        EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);
    }
}

TEST(Sampling1Track2Keys, SamplingJob) {
    RawAnimation raw_animation;
    raw_animation.duration = 46.f;
    raw_animation.tracks.resize(1);  // Adds a joint.

    SamplingJob::Context context(1);

    const RawAnimation::TranslationKey tkey0 = {.5f, vox::Vector3F(1.f, 2.f, 4.f)};
    raw_animation.tracks[0].translations.push_back(tkey0);  // Adds a key.
    const RawAnimation::TranslationKey tkey1 = {.8f, vox::Vector3F(2.f, 4.f, 8.f)};
    raw_animation.tracks[0].translations.push_back(tkey1);  // Adds a key.

    AnimationBuilder builder;
    vox::unique_ptr<Animation> animation(builder(raw_animation));
    ASSERT_TRUE(animation);

    vox::simd_math::SoaTransform output[1];
    memset(output, 0xde, sizeof(output));

    SamplingJob job;
    job.animation = animation.get();
    job.context = &context;
    job.output = output;

    // Samples at t = 0.
    job.ratio = 0.f;
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.f, 0.f, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f, 4.f, 0.f, 0.f, 0.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                1.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);

    // Samples at t = tkey0.
    job.ratio = tkey0.time / animation->duration();
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.f, 0.f, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f, 4.f, 0.f, 0.f, 0.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                1.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);

    // Samples at t = tkey1.
    job.ratio = tkey1.time / animation->duration();
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 2.f, 0.f, 0.f, 0.f, 4.f, 0.f, 0.f, 0.f, 8.f, 0.f, 0.f, 0.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                1.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);

    // Samples at t = end.
    job.ratio = 1.f;
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 2.f, 0.f, 0.f, 0.f, 4.f, 0.f, 0.f, 0.f, 8.f, 0.f, 0.f, 0.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                1.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);

    // Samples at tkey0.time < t < tkey1.time.
    job.ratio = (tkey0.time / animation->duration() + tkey1.time / animation->duration()) / 2.f;
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.5f, 0.f, 0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 6.f, 0.f, 0.f, 0.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                1.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);
}

TEST(Sampling4Track2Keys, SamplingJob) {
    RawAnimation raw_animation;
    raw_animation.duration = 1.f;
    raw_animation.tracks.resize(4);  // Adds a joint.

    SamplingJob::Context context(1);

    const RawAnimation::TranslationKey tkey00 = {.5f, vox::Vector3F(1.f, 2.f, 4.f)};
    raw_animation.tracks[0].translations.push_back(tkey00);  // Adds a key.
    const RawAnimation::TranslationKey tkey01 = {.8f, vox::Vector3F(2.f, 4.f, 8.f)};
    raw_animation.tracks[0].translations.push_back(tkey01);  // Adds a key.

    // This quaternion will be negated as the builder ensures that the first key
    // is in identity quaternion hemisphere.
    const RawAnimation::RotationKey rkey10 = {0.f, vox::QuaternionF(0.f, 0.f, 0.f, -1.f)};
    raw_animation.tracks[1].rotations.push_back(rkey10);  // Adds a key.
    const RawAnimation::RotationKey rkey11 = {1.f, vox::QuaternionF(0.f, 1.f, 0.f, 0.f)};
    raw_animation.tracks[1].rotations.push_back(rkey11);  // Adds a key.

    const RawAnimation::ScaleKey skey20 = {.5f, vox::Vector3F(0.f, 0.f, 0.f)};
    raw_animation.tracks[2].scales.push_back(skey20);  // Adds a key.
    const RawAnimation::ScaleKey skey21 = {.8f, vox::Vector3F(-1.f, -1.f, -1.f)};
    raw_animation.tracks[2].scales.push_back(skey21);  // Adds a key.

    const RawAnimation::TranslationKey tkey30 = {0.f, vox::Vector3F(-1.f, -2.f, -4.f)};
    raw_animation.tracks[3].translations.push_back(tkey30);  // Adds a key.
    const RawAnimation::TranslationKey tkey31 = {1.f, vox::Vector3F(-2.f, -4.f, -8.f)};
    raw_animation.tracks[3].translations.push_back(tkey31);  // Adds a key.

    AnimationBuilder builder;
    vox::unique_ptr<Animation> animation(builder(raw_animation));
    ASSERT_TRUE(animation);

    vox::simd_math::SoaTransform output[1];
    memset(output, 0xde, sizeof(output));

    SamplingJob job;
    job.animation = animation.get();
    job.context = &context;
    job.output = output;

    // Samples at t = 0.
    job.ratio = 0.f;
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.f, 0.f, 0.f, -1.f, 2.f, 0.f, 0.f, -2.f, 4.f, 0.f, 0.f, -4.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                1.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 1.f);

    // Samples at t = tkey00.
    job.ratio = tkey00.time / animation->duration();
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.f, 0.f, 0.f, -1.5f, 2.f, 0.f, 0.f, -3.f, 4.f, 0.f, 0.f, -6.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, .7071067f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                                1.f, .7071067f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 1.f);

    // Samples at t = end.
    job.ratio = 1.f;
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 2.f, 0.f, 0.f, -2.f, 4.f, 0.f, 0.f, -4.f, 8.f, 0.f, 0.f, -8.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                0.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, -1.f, 1.f, 1.f, 1.f, -1.f, 1.f, 1.f, 1.f, -1.f, 1.f);
}

TEST(Cache, SamplingJob) {
    RawAnimation raw_animation;
    raw_animation.duration = 46.f;
    raw_animation.tracks.resize(1);  // Adds a joint.
    const RawAnimation::TranslationKey empty_key = {0.f, RawAnimation::TranslationKey::identity()};
    raw_animation.tracks[0].translations.push_back(empty_key);

    SamplingJob::Context context(1);
    vox::unique_ptr<Animation> animations[2];

    {
        const RawAnimation::TranslationKey tkey = {.3f, vox::Vector3F(1.f, -1.f, 5.f)};
        raw_animation.tracks[0].translations[0] = tkey;

        AnimationBuilder builder;
        animations[0] = builder(raw_animation);
        ASSERT_TRUE(animations[0]);
    }
    {
        const RawAnimation::TranslationKey tkey = {.3f, vox::Vector3F(-1.f, 1.f, -5.f)};
        raw_animation.tracks[0].translations[0] = tkey;

        AnimationBuilder builder;
        animations[1] = builder(raw_animation);
        ASSERT_TRUE(animations[1]);
    }

    vox::simd_math::SoaTransform output[1];

    SamplingJob job;
    job.animation = animations[0].get();
    job.context = &context;
    job.ratio = 0.f;
    job.output = output;

    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, 0.f, 5.f, 0.f, 0.f, 0.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                1.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);

    // Re-uses context.
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, 0.f, 5.f, 0.f, 0.f, 0.f);

    // Invalidates context.
    context.Invalidate();

    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, 0.f, 5.f, 0.f, 0.f, 0.f);

    // Changes animation.
    job.animation = animations[1].get();
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, -5.f, 0.f, 0.f, 0.f);
    EXPECT_SOAQUATERNION_EQ_EST(output[0].rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                1.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ_EST(output[0].scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);

    // Invalidates and changes animation.
    job.animation = animations[1].get();
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());
    EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, -5.f, 0.f, 0.f, 0.f);
}

TEST(CacheResize, SamplingJob) {
    RawAnimation raw_animation;
    raw_animation.duration = 46.f;
    raw_animation.tracks.resize(7);

    AnimationBuilder builder;
    vox::unique_ptr<Animation> animation(builder(raw_animation));
    ASSERT_TRUE(animation);

    // Empty context by default
    SamplingJob::Context context;

    vox::simd_math::SoaTransform output[7];

    SamplingJob job;
    job.animation = animation.get();
    job.context = &context;
    job.ratio = 0.f;
    job.output = output;

    // Cache is too small
    EXPECT_FALSE(job.Validate());

    // Cache is ok.
    context.Resize(7);
    EXPECT_TRUE(job.Validate());
    EXPECT_TRUE(job.Run());

    // Cache is too small
    context.Resize(1);
    EXPECT_FALSE(job.Validate());
}
