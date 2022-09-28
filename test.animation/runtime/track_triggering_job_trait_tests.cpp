//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <algorithm>

#include "gtest/gtest.h"
#include "test.animation/gtest_helper.h"
#include "test.animation/gtest_math_helper.h"
#include "vox.base/memory/unique_ptr.h"
#include "vox.animation/offline/raw_track.h"
#include "vox.animation/offline/track_builder.h"
#include "vox.animation/runtime/track.h"
#include "vox.animation/runtime/track_triggering_job_trait.h"

using vox::animation::FloatTrack;
using vox::animation::TrackTriggeringJob;
using vox::animation::offline::RawFloatTrack;
using vox::animation::offline::RawTrackInterpolation;
using vox::animation::offline::TrackBuilder;

namespace {
bool IsRising(const TrackTriggeringJob::Edge& _edge) { return _edge.rising; }
}  // namespace

TEST(Algorithm, TrackEdgeTriggerJob) {
    TrackBuilder builder;

    vox::animation::offline::RawFloatTrack raw_track;

    // Keyframe values oscillate in range [0,2].
    const vox::animation::offline::RawFloatTrack::Keyframe key0 = {RawTrackInterpolation::kStep, 0.f, 0.f};
    raw_track.keyframes.push_back(key0);
    const vox::animation::offline::RawFloatTrack::Keyframe key1 = {RawTrackInterpolation::kStep, .5f, 2.f};
    raw_track.keyframes.push_back(key1);
    const vox::animation::offline::RawFloatTrack::Keyframe key2 = {RawTrackInterpolation::kStep, 1.f, 0.f};
    raw_track.keyframes.push_back(key2);

    // Builds track
    vox::unique_ptr<FloatTrack> track(builder(raw_track));
    ASSERT_TRUE(track);

    TrackTriggeringJob job;
    job.track = track.get();
    job.threshold = 1.f;

    job.from = 0.f;
    job.to = 2.f;
    TrackTriggeringJob::Iterator iterator;
    job.iterator = &iterator;
    ASSERT_TRUE(job.Run());

    {  // copy
        vox::vector<TrackTriggeringJob::Edge> edges;
        std::copy(iterator, job.end(), std::back_inserter(edges));
        EXPECT_EQ(edges.size(), 4u);
    }

    {  // count
        vox::vector<TrackTriggeringJob::Edge> edges;
        std::iterator_traits<TrackTriggeringJob::Iterator>::difference_type count =
                std::count_if(iterator, job.end(), IsRising);
        EXPECT_EQ(count, 2);

        std::iterator_traits<TrackTriggeringJob::Iterator>::difference_type count_end =
                std::count_if(job.end(), job.end(), IsRising);
        EXPECT_EQ(count_end, 0);
    }

    {  // for_each
        vox::vector<TrackTriggeringJob::Edge> edges;
        std::for_each(iterator, job.end(), IsRising);
    }

    {  // find_if
        vox::vector<TrackTriggeringJob::Edge> edges;
        TrackTriggeringJob::Iterator it_if = std::find_if(iterator, job.end(), IsRising);
        EXPECT_TRUE(it_if->rising);
    }
}
