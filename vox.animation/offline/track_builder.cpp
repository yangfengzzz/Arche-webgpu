//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/track_builder.h"

#include <cassert>
#include <cstring>
#include <limits>

#include "vox.animation/offline/raw_track.h"
#include "vox.animation/runtime/track.h"

namespace vox::animation::offline {

namespace {

template <typename RawTrack>
void PatchBeginEndKeys(const RawTrack& _input, typename RawTrack::Keyframes* keyframes) {
    if (_input.keyframes.empty()) {
        const typename RawTrack::ValueType default_value =
                animation::internal::TrackPolicy<typename RawTrack::ValueType>::identity();

        const typename RawTrack::Keyframe begin = {RawTrackInterpolation::kLinear, 0.f, default_value};
        keyframes->push_back(begin);
        const typename RawTrack::Keyframe end = {RawTrackInterpolation::kLinear, 1.f, default_value};
        keyframes->push_back(end);
    } else if (_input.keyframes.size() == 1) {
        const typename RawTrack::Keyframe& src_key = _input.keyframes.front();
        const typename RawTrack::Keyframe begin = {RawTrackInterpolation::kLinear, 0.f, src_key.value};
        keyframes->push_back(begin);
        const typename RawTrack::Keyframe end = {RawTrackInterpolation::kLinear, 1.f, src_key.value};
        keyframes->push_back(end);
    } else {
        // Copy all source data.
        // Push an initial and last keys if they don't exist.
        if (_input.keyframes.front().ratio != 0.f) {
            const typename RawTrack::Keyframe& src_key = _input.keyframes.front();
            const typename RawTrack::Keyframe begin = {RawTrackInterpolation::kLinear, 0.f, src_key.value};
            keyframes->push_back(begin);
        }
        for (size_t i = 0; i < _input.keyframes.size(); ++i) {
            keyframes->push_back(_input.keyframes[i]);
        }
        if (_input.keyframes.back().ratio != 1.f) {
            const typename RawTrack::Keyframe& src_key = _input.keyframes.back();
            const typename RawTrack::Keyframe end = {RawTrackInterpolation::kLinear, 1.f, src_key.value};
            keyframes->push_back(end);
        }
    }
}

template <typename Keyframes>
void Fixup(Keyframes* _keyframes) {
    // Nothing to do by default.
    (void)_keyframes;
}
}  // namespace

// Ensures _input's validity and allocates _animation.
// An animation needs to have at least two key frames per joint, the first at
// t = 0 and the last at t = 1. If at least one of those keys are not
// in the RawAnimation then the builder creates it.
template <typename RawTrack, typename Track>
unique_ptr<Track> TrackBuilder::Build(const RawTrack& _input) const {
    // Tests _raw_animation validity.
    if (!_input.Validate()) {
        return unique_ptr<Track>();
    }

    // Everything is fine, allocates and fills the animation.
    // Nothing can fail now.
    unique_ptr<Track> track = make_unique<Track>();

    // Copy data to temporary prepared data structure
    typename RawTrack::Keyframes keyframes;
    // Guessing the worst size to avoid realloc.
    const size_t worst_size = _input.keyframes.size() * 2 +  // * 2 in case all keys are kStep
                              2;                             // + 2 for first and last keys
    keyframes.reserve(worst_size);

    // Ensure there's a key frame at the start and end of the track (required for
    // sampling).
    PatchBeginEndKeys(_input, &keyframes);

    // Fixup values, ex: successive opposite quaternions that would fail to take
    // the shortest path during the normalized-lerp.
    Fixup(&keyframes);

    // Allocates output track.
    const size_t name_len = _input.name.size();
    track->Allocate(keyframes.size(), _input.name.size());

    // Copy all keys to output.
    assert(keyframes.size() == track->ratios_.size() && keyframes.size() == track->values_.size() &&
           keyframes.size() <= track->steps_.size() * 8);
    memset(track->steps_.data(), 0, track->steps_.size_bytes());
    for (size_t i = 0; i < keyframes.size(); ++i) {
        const typename RawTrack::Keyframe& src_key = keyframes[i];
        track->ratios_[i] = src_key.ratio;
        track->values_[i] = src_key.value;
        track->steps_[i / 8] |= (src_key.interpolation == RawTrackInterpolation::kStep) << (i & 7);
    }

    // Copy track's name.
    if (name_len) {
        strcpy(track->name_, _input.name.c_str());
    }

    return track;  // Success.
}

unique_ptr<FloatTrack> TrackBuilder::operator()(const RawFloatTrack& _input) const {
    return Build<RawFloatTrack, FloatTrack>(_input);
}
unique_ptr<Float2Track> TrackBuilder::operator()(const RawFloat2Track& _input) const {
    return Build<RawFloat2Track, Float2Track>(_input);
}
unique_ptr<Float3Track> TrackBuilder::operator()(const RawFloat3Track& _input) const {
    return Build<RawFloat3Track, Float3Track>(_input);
}
unique_ptr<Float4Track> TrackBuilder::operator()(const RawFloat4Track& _input) const {
    return Build<RawFloat4Track, Float4Track>(_input);
}

namespace {
// Fixes-up successive opposite quaternions that would fail to take the shortest
// path during the lerp.
template <>
void Fixup<RawQuaternionTrack::Keyframes>(RawQuaternionTrack::Keyframes* _keyframes) {
    assert(_keyframes->size() >= 2);

    const QuaternionF identity = QuaternionF::makeIdentity();
    for (size_t i = 0; i < _keyframes->size(); ++i) {
        RawQuaternionTrack::ValueType& src_key = _keyframes->at(i).value;

        // Normalizes input quaternion.
        src_key = normalizeSafe(src_key, identity);

        // Ensures quaternions are all on the same hemisphere.
        if (i == 0) {
            if (src_key.w < 0.f) {
                src_key = -src_key;  // Q an -Q are the same rotation.
            }
        } else {
            RawQuaternionTrack::ValueType& prev_key = _keyframes->at(i - 1).value;
            const float dot =
                    src_key.x * prev_key.x + src_key.y * prev_key.y + src_key.z * prev_key.z + src_key.w * prev_key.w;
            if (dot < 0.f) {
                src_key = -src_key;  // Q an -Q are the same rotation.
            }
        }
    }
}
}  // namespace

unique_ptr<QuaternionTrack> TrackBuilder::operator()(const RawQuaternionTrack& _input) const {
    return Build<RawQuaternionTrack, QuaternionTrack>(_input);
}
}  // namespace vox::animation::offline
