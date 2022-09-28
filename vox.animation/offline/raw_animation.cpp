//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/raw_animation.h"

#include "vox.animation/runtime/skeleton.h"

namespace vox::animation::offline {

RawAnimation::RawAnimation() : duration(1.f) {}

namespace {

// Implements key frames' time range and ordering checks.
// See AnimationBuilder::Create for more details.
template <typename Key>
static bool ValidateTrack(const typename vox::vector<Key>& _track, float _duration) {
    float previous_time = -1.f;
    for (size_t k = 0; k < _track.size(); ++k) {
        const float frame_time = _track[k].time;
        // Tests frame's time is in range [0:duration].
        if (frame_time < 0.f || frame_time > _duration) {
            return false;
        }
        // Tests that frames are sorted.
        if (frame_time <= previous_time) {
            return false;
        }
        previous_time = frame_time;
    }
    return true;  // Validated.
}
}  // namespace
bool RawAnimation::JointTrack::Validate(float _duration) const {
    return ValidateTrack<TranslationKey>(translations, _duration) && ValidateTrack<RotationKey>(rotations, _duration) &&
           ValidateTrack<ScaleKey>(scales, _duration);
}

bool RawAnimation::Validate() const {
    if (duration <= 0.f) {  // Tests duration is valid.
        return false;
    }
    if (tracks.size() > Skeleton::kMaxJoints) {  // Tests number of tracks.
        return false;
    }
    // Ensures that all key frames' time are valid, ie: in a strict ascending
    // order and within range [0:duration].
    bool valid = true;
    for (size_t i = 0; valid && i < tracks.size(); ++i) {
        valid = tracks[i].Validate(duration);
    }
    return valid;  // *this is valid.
}

size_t RawAnimation::size() const {
    size_t size = sizeof(*this);

    // Accumulates keyframes size.
    const size_t tracks_count = tracks.size();
    for (size_t i = 0; i < tracks_count; ++i) {
        size += tracks[i].translations.size() * sizeof(TranslationKey);
        size += tracks[i].rotations.size() * sizeof(RotationKey);
        size += tracks[i].scales.size() * sizeof(ScaleKey);
    }

    // Accumulates tracks.
    size += tracks_count * sizeof(JointTrack);
    size += name.size();

    return size;
}

}  // namespace vox::animation::offline
