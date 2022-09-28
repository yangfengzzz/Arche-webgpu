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
#include "vox.animation/runtime/skeleton.h"
#include "vox.simd_math/soa_transform.h"

using vox::animation::Animation;
using vox::animation::offline::AnimationBuilder;
using vox::animation::offline::RawAnimation;

TEST(Error, AnimationBuilder) {
    // Instantiates a builder objects with default parameters.
    AnimationBuilder builder;

    {  // Building an empty Animation fails because animation duration
        // must be >= 0.
        RawAnimation raw_animation;
        raw_animation.duration = -1.f;  // Negative duration.
        EXPECT_FALSE(raw_animation.Validate());

        // Builds animation
        EXPECT_TRUE(!builder(raw_animation));
    }

    {  // Building an empty Animation fails because animation duration
        // must be >= 0.
        RawAnimation raw_animation;
        raw_animation.duration = 0.f;  // Invalid duration.
        EXPECT_FALSE(raw_animation.Validate());

        // Builds animation
        EXPECT_TRUE(!builder(raw_animation));
    }

    {  // Building an animation with too much tracks fails.
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(vox::animation::Skeleton::kMaxJoints + 1);
        EXPECT_FALSE(raw_animation.Validate());

        // Builds animation
        EXPECT_TRUE(!builder(raw_animation));
    }

    {  // Building default animation succeeds.
        RawAnimation raw_animation;
        EXPECT_EQ(raw_animation.duration, 1.f);
        EXPECT_TRUE(raw_animation.Validate());

        // Builds animation
        vox::unique_ptr<Animation> anim(builder(raw_animation));
        EXPECT_TRUE(anim);
    }

    {  // Building an animation with max joints succeeds.
        RawAnimation raw_animation;
        raw_animation.tracks.resize(vox::animation::Skeleton::kMaxJoints);
        EXPECT_EQ(raw_animation.num_tracks(), vox::animation::Skeleton::kMaxJoints);
        EXPECT_TRUE(raw_animation.Validate());

        // Builds animation
        vox::unique_ptr<Animation> anim(builder(raw_animation));
        EXPECT_TRUE(anim);
    }
}

TEST(Build, AnimationBuilder) {
    // Instantiates a builder objects with default parameters.
    AnimationBuilder builder;

    {  // Building an Animation with unsorted keys fails.
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(1);

        // Adds 2 unordered keys
        RawAnimation::TranslationKey first_key = {.8f, vox::Vector3F()};
        raw_animation.tracks[0].translations.push_back(first_key);
        RawAnimation::TranslationKey second_key = {.2f, vox::Vector3F()};
        raw_animation.tracks[0].translations.push_back(second_key);

        // Builds animation
        EXPECT_FALSE(builder(raw_animation));
    }

    {  // Building an Animation with invalid key frame's time fails.
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(1);

        // Adds a key with a time greater than animation duration.
        RawAnimation::TranslationKey first_key = {2.f, vox::Vector3F()};
        raw_animation.tracks[0].translations.push_back(first_key);

        // Builds animation
        EXPECT_FALSE(builder(raw_animation));
    }

    {  // Building an Animation with unsorted key frame's time fails.
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(2);

        // Adds 2 unsorted keys.
        RawAnimation::TranslationKey first_key = {0.7f, vox::Vector3F()};
        raw_animation.tracks[0].translations.push_back(first_key);
        RawAnimation::TranslationKey second_key = {0.1f, vox::Vector3F()};
        raw_animation.tracks[0].translations.push_back(second_key);

        // Builds animation
        EXPECT_FALSE(builder(raw_animation));
    }

    {  // Building an Animation with equal key frame's time fails.
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(2);

        // Adds 2 unsorted keys.
        RawAnimation::TranslationKey key = {0.7f, vox::Vector3F()};
        raw_animation.tracks[0].translations.push_back(key);
        raw_animation.tracks[0].translations.push_back(key);

        // Builds animation
        EXPECT_FALSE(builder(raw_animation));
    }

    {  // Building a valid Animation with empty tracks succeeds.
        RawAnimation raw_animation;
        raw_animation.duration = 46.f;
        raw_animation.tracks.resize(46);

        // Builds animation
        vox::unique_ptr<Animation> anim(builder(raw_animation));
        EXPECT_TRUE(anim);
        EXPECT_EQ(anim->duration(), 46.f);
        EXPECT_EQ(anim->num_tracks(), 46);
    }

    {  // Building a valid Animation with 1 track succeeds.
        RawAnimation raw_animation;
        raw_animation.duration = 46.f;
        raw_animation.tracks.resize(1);

        RawAnimation::TranslationKey first_key = {0.7f, vox::Vector3F()};
        raw_animation.tracks[0].translations.push_back(first_key);

        // Builds animation
        vox::unique_ptr<Animation> anim(builder(raw_animation));
        EXPECT_TRUE(anim);
        EXPECT_EQ(anim->duration(), 46.f);
        EXPECT_EQ(anim->num_tracks(), 1);
    }
}

TEST(Name, AnimationBuilder) {
    // Instantiates a builder objects with default parameters.
    AnimationBuilder builder;

    {  // Building an unnamed animation.
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(46);

        // Builds animation
        vox::unique_ptr<Animation> anim(builder(raw_animation));
        EXPECT_TRUE(anim);

        // Should
        EXPECT_STREQ(anim->name(), "");
    }

    {  // Building an unnamed animation.
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(46);
        raw_animation.name = "46";

        // Builds animation
        vox::unique_ptr<Animation> anim(builder(raw_animation));
        EXPECT_TRUE(anim);

        // Should
        EXPECT_STREQ(anim->name(), "46");
    }
}

TEST(Move, AnimationBuilder) {
    AnimationBuilder builder;
    RawAnimation raw_animation;

    {  // Move constructor
        raw_animation.name = "anim1";
        raw_animation.duration = 46.f;
        raw_animation.tracks.resize(46);
        vox::unique_ptr<Animation> anim1(builder(raw_animation));
        const Animation canim(std::move(*anim1));
        EXPECT_FLOAT_EQ(canim.duration(), 46.f);
        EXPECT_STREQ(canim.name(), "anim1");
    }

    {  // Move assignment
        raw_animation.name = "anim1";
        raw_animation.duration = 46.f;
        raw_animation.tracks.resize(46);
        vox::unique_ptr<Animation> anim1(builder(raw_animation));
        EXPECT_STREQ(anim1->name(), "anim1");
        EXPECT_EQ(anim1->num_tracks(), 46);

        raw_animation.name = "anim2";
        raw_animation.duration = 93.f;
        raw_animation.tracks.resize(93);
        vox::unique_ptr<Animation> anim2(builder(raw_animation));
        EXPECT_STREQ(anim2->name(), "anim2");
        EXPECT_EQ(anim2->num_tracks(), 93);

        *anim2 = std::move(*anim1);
        EXPECT_FLOAT_EQ(anim2->duration(), 46.f);
        EXPECT_EQ(anim2->num_tracks(), 46);
        EXPECT_STREQ(anim2->name(), "anim1");
    }
}

TEST(Sort, AnimationBuilder) {
    // Instantiates a builder objects with default parameters.
    AnimationBuilder builder;

    {
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(4);

        // Raw animation inputs.
        //     0              1
        // --------------------
        // 0 - A     B        |
        // 1 - C  D  E        |
        // 2 - F  G     H  I  J
        // 3 - K  L  M  N     |

        // Final animation.
        //     0              1
        // --------------------
        // 0 - 0     4       11
        // 1 - 1  5  8       12
        // 2 - 2  6     9 14 16
        // 3 - 3  7 10 13    15

        RawAnimation::TranslationKey a = {0.f * raw_animation.duration, vox::Vector3F(1.f, 0.f, 0.f)};
        raw_animation.tracks[0].translations.push_back(a);
        RawAnimation::TranslationKey b = {.4f * raw_animation.duration, vox::Vector3F(3.f, 0.f, 0.f)};
        raw_animation.tracks[0].translations.push_back(b);

        RawAnimation::TranslationKey c = {0.f * raw_animation.duration, vox::Vector3F(2.f, 0.f, 0.f)};
        raw_animation.tracks[1].translations.push_back(c);
        RawAnimation::TranslationKey d = {0.2f * raw_animation.duration, vox::Vector3F(6.f, 0.f, 0.f)};
        raw_animation.tracks[1].translations.push_back(d);
        RawAnimation::TranslationKey e = {0.4f * raw_animation.duration, vox::Vector3F(8.f, 0.f, 0.f)};
        raw_animation.tracks[1].translations.push_back(e);

        RawAnimation::TranslationKey f = {0.f * raw_animation.duration, vox::Vector3F(12.f, 0.f, 0.f)};
        raw_animation.tracks[2].translations.push_back(f);
        RawAnimation::TranslationKey g = {.2f * raw_animation.duration, vox::Vector3F(11.f, 0.f, 0.f)};
        raw_animation.tracks[2].translations.push_back(g);
        RawAnimation::TranslationKey h = {.6f * raw_animation.duration, vox::Vector3F(9.f, 0.f, 0.f)};
        raw_animation.tracks[2].translations.push_back(h);
        RawAnimation::TranslationKey i = {.8f * raw_animation.duration, vox::Vector3F(7.f, 0.f, 0.f)};
        raw_animation.tracks[2].translations.push_back(i);
        RawAnimation::TranslationKey j = {1.f * raw_animation.duration, vox::Vector3F(5.f, 0.f, 0.f)};
        raw_animation.tracks[2].translations.push_back(j);

        RawAnimation::TranslationKey k = {0.f * raw_animation.duration, vox::Vector3F(1.f, 0.f, 0.f)};
        raw_animation.tracks[3].translations.push_back(k);
        RawAnimation::TranslationKey l = {.2f * raw_animation.duration, vox::Vector3F(2.f, 0.f, 0.f)};
        raw_animation.tracks[3].translations.push_back(l);
        RawAnimation::TranslationKey m = {.4f * raw_animation.duration, vox::Vector3F(3.f, 0.f, 0.f)};
        raw_animation.tracks[3].translations.push_back(m);
        RawAnimation::TranslationKey n = {.6f * raw_animation.duration, vox::Vector3F(4.f, 0.f, 0.f)};
        raw_animation.tracks[3].translations.push_back(n);

        // Builds animation
        vox::unique_ptr<Animation> animation(builder(raw_animation));
        ASSERT_TRUE(animation);

        // Duration must be maintained.
        EXPECT_EQ(animation->duration(), raw_animation.duration);

        // Needs to sample to test the animation.
        vox::animation::SamplingJob job;
        vox::animation::SamplingJob::Context context(1);
        vox::simd_math::SoaTransform output[1];
        job.animation = animation.get();
        job.context = &context;
        job.output = output;

        // Samples and compares the two animations
        {  // Samples at t = 0
            job.ratio = 0.f;
            job.Run();
            EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 1.f, 2.f, 12.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        }
        {  // Samples at t = .2
            job.ratio = .2f;
            job.Run();
            EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 2.f, 6.f, 11.f, 2.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        }
        {  // Samples at t = .4
            job.ratio = .4f;
            job.Run();
            EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 3.f, 8.f, 10.f, 3.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        }
        {  // Samples at t = .6
            job.ratio = .6f;
            job.Run();
            EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 3.f, 8.f, 9.f, 4.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        }
        {  // Samples at t = .8
            job.ratio = .8f;
            job.Run();
            EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 3.f, 8.f, 7.f, 4.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        }
        {  // Samples at t = 1
            job.ratio = 1.f;
            job.Run();
            EXPECT_SOAFLOAT3_EQ_EST(output[0].translation, 3.f, 8.f, 5.f, 4.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        }
    }
}
