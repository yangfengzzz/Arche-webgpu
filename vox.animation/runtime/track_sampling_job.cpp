//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/runtime/track_sampling_job.h"

#include <algorithm>
#include <cassert>

namespace vox::animation::internal {

template <typename Track>
TrackSamplingJob<Track>::TrackSamplingJob() : ratio(0.f), track(nullptr), result(nullptr) {}

template <typename Track>
bool TrackSamplingJob<Track>::Validate() const {
    bool success = true;
    success &= result != nullptr;
    success &= track != nullptr;
    return success;
}

template <typename Track>
bool TrackSamplingJob<Track>::Run() const {
    if (!Validate()) {
        return false;
    }

    // Clamps ratio in range [0,1].
    const float clamped_ratio = vox::clamp(0.f, ratio, 1.f);

    // Search keyframes to interpolate.
    const span<const float> ratios = track->ratios();
    const span<const ValueType> values = track->values();
    assert(ratios.size() == values.size() && track->steps().size() * 8 >= values.size());

    // Default track returns identity.
    if (ratios.empty()) {
        *result = internal::TrackPolicy<ValueType>::identity();
        return true;
    }

    // Search for the first key frame with a ratio value greater than input ratio.
    // Our ratio is between this one and the previous one.
    const float* ptk1 = std::upper_bound(ratios.begin(), ratios.end(), clamped_ratio);

    // Deduce keys indices.
    const size_t id1 = ptk1 - ratios.begin();
    const size_t id0 = id1 - 1;

    const bool id0step = (track->steps()[id0 / 8] & (1 << (id0 & 7))) != 0;
    if (id0step || ptk1 == ratios.end()) {
        *result = values[id0];
    } else {
        // Lerp relevant keys.
        const float tk0 = ratios[id0];
        const float tk1 = ratios[id1];
        assert(clamped_ratio >= tk0 && clamped_ratio < tk1 && tk0 != tk1);
        const float alpha = (clamped_ratio - tk0) / (tk1 - tk0);
        const ValueType& vk0 = values[id0];
        const ValueType& vk1 = values[id1];
        *result = internal::TrackPolicy<ValueType>::Lerp(vk0, vk1, alpha);
    }
    return true;
}

// Explicitly instantiate supported tracks.
template struct TrackSamplingJob<FloatTrack>;
template struct TrackSamplingJob<Float2Track>;
template struct TrackSamplingJob<Float3Track>;
template struct TrackSamplingJob<Float4Track>;
template struct TrackSamplingJob<QuaternionTrack>;
}  // namespace vox::animation::internal
