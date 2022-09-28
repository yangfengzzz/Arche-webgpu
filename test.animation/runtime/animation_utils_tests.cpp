//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "vox.animation/offline/animation_builder.h"
#include "vox.animation/offline/raw_animation.h"
#include "vox.animation/runtime/animation_utils.h"
#include "vox.base/memory/unique_ptr.h"

using vox::animation::Animation;
using vox::animation::offline::AnimationBuilder;
using vox::animation::offline::RawAnimation;

TEST(CountKeyframes, AnimationUtils) {
    // Builds a valid animation.
    vox::unique_ptr<Animation> animation;

    {
        RawAnimation raw_animation;
        raw_animation.duration = 1.f;
        raw_animation.tracks.resize(2);

        RawAnimation::TranslationKey t_key0 = {0.f, vox::Vector3F(93.f, 58.f, 46.f)};
        raw_animation.tracks[0].translations.push_back(t_key0);
        RawAnimation::TranslationKey t_key1 = {.9f, vox::Vector3F(46.f, 58.f, 93.f)};
        raw_animation.tracks[0].translations.push_back(t_key1);
        RawAnimation::TranslationKey t_key2 = {1.f, vox::Vector3F(46.f, 58.f, 99.f)};
        raw_animation.tracks[0].translations.push_back(t_key2);

        RawAnimation::RotationKey r_key = {0.7f, vox::QuaternionF(0.f, 1.f, 0.f, 0.f)};
        raw_animation.tracks[0].rotations.push_back(r_key);

        RawAnimation::ScaleKey s_key = {0.1f, vox::Vector3F(99.f, 26.f, 14.f)};
        raw_animation.tracks[1].scales.push_back(s_key);

        AnimationBuilder builder;
        animation = builder(raw_animation);
        ASSERT_TRUE(animation);
    }

    // 4 more tracks than expected due to SoA
    EXPECT_EQ(vox::animation::CountTranslationKeyframes(*animation, -1), 9);
    EXPECT_EQ(vox::animation::CountTranslationKeyframes(*animation, 0), 3);
    EXPECT_EQ(vox::animation::CountTranslationKeyframes(*animation, 1), 2);

    EXPECT_EQ(vox::animation::CountRotationKeyframes(*animation, -1), 8);
    EXPECT_EQ(vox::animation::CountRotationKeyframes(*animation, 0), 2);
    EXPECT_EQ(vox::animation::CountRotationKeyframes(*animation, 1), 2);

    EXPECT_EQ(vox::animation::CountScaleKeyframes(*animation, -1), 8);
    EXPECT_EQ(vox::animation::CountScaleKeyframes(*animation, 0), 2);
    EXPECT_EQ(vox::animation::CountScaleKeyframes(*animation, 1), 2);
}
