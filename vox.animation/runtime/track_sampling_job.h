//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/export.h"
#include "vox.animation/runtime/track.h"

namespace vox::animation {

namespace internal {

// TrackSamplingJob internal implementation. See *TrackSamplingJob for more
// details.
template <typename Track>
struct TrackSamplingJob {
    typedef typename Track::ValueType ValueType;

    TrackSamplingJob();

    // Validates all parameters.
    [[nodiscard]] bool Validate() const;

    // Validates and executes sampling.
    [[nodiscard]] bool Run() const;

    // Ratio used to sample track, clamped in range [0,1] before job execution. 0
    // is the beginning of the track, 1 is the end. This is a ratio rather than a
    // ratio because tracks have no duration.
    float ratio;

    // Track to sample.
    const Track* track;

    // Job output.
    typename Track::ValueType* result;
};
}  // namespace internal

// Track sampling job implementation. Track sampling allows to query a track
// value for a specified ratio. This is a ratio rather than a time because
// tracks have no duration.
struct VOX_ANIMATION_DLL FloatTrackSamplingJob : public internal::TrackSamplingJob<FloatTrack> {};
struct VOX_ANIMATION_DLL Float2TrackSamplingJob : public internal::TrackSamplingJob<Float2Track> {};
struct VOX_ANIMATION_DLL Float3TrackSamplingJob : public internal::TrackSamplingJob<Float3Track> {};
struct VOX_ANIMATION_DLL Float4TrackSamplingJob : public internal::TrackSamplingJob<Float4Track> {};
struct VOX_ANIMATION_DLL QuaternionTrackSamplingJob : public internal::TrackSamplingJob<QuaternionTrack> {};

}  // namespace vox::animation