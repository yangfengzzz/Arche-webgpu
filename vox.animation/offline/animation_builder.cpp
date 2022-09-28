//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/animation_builder.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <limits>

#include "vox.animation/offline/raw_animation.h"
#include "vox.animation/runtime/animation.h"
#include "vox.simd_math/simd_math.h"

// Internal include file
#define VOX_INCLUDE_PRIVATE_HEADER  // Allows to include private headers.
#include "vox.animation/runtime/animation_keyframe.h"

namespace vox::animation::offline {
namespace {

struct SortingTranslationKey {
    uint16_t track;
    float prev_key_time;
    RawAnimation::TranslationKey key;
};

struct SortingRotationKey {
    uint16_t track;
    float prev_key_time;
    RawAnimation::RotationKey key;
};

struct SortingScaleKey {
    uint16_t track;
    float prev_key_time;
    RawAnimation::ScaleKey key;
};

// Keyframe sorting. Stores first by time and then track number.
template <typename Key>
bool SortingKeyLess(const Key& _left, const Key& _right) {
    const float time_diff = _left.prev_key_time - _right.prev_key_time;
    return time_diff < 0.f || (time_diff == 0.f && _left.track < _right.track);
}

template <typename SrcKey, typename DestTrack>
void PushBackIdentityKey(uint16_t _track, float _time, DestTrack* _dest) {
    typedef typename DestTrack::value_type DestKey;
    float prev_time = -1.f;
    if (!_dest->empty() && _dest->back().track == _track) {
        prev_time = _dest->back().key.time;
    }
    const DestKey key = {_track, prev_time, {_time, SrcKey::identity()}};
    _dest->push_back(key);
}

// Copies a track from a RawAnimation to an Animation.
// Also fixes up the front (t = 0) and back keys (t = duration).
template <typename SrcTrack, typename DestTrack>
void CopyRaw(const SrcTrack& _src, uint16_t _track, float _duration, DestTrack* _dest) {
    typedef typename SrcTrack::value_type SrcKey;
    typedef typename DestTrack::value_type DestKey;

    if (_src.size() == 0) {  // Adds 2 new keys.
        PushBackIdentityKey<SrcKey, DestTrack>(_track, 0.f, _dest);
        PushBackIdentityKey<SrcKey, DestTrack>(_track, _duration, _dest);
    } else if (_src.size() == 1) {  // Adds 1 new key.
        const SrcKey& raw_key = _src.front();
        assert(raw_key.time >= 0 && raw_key.time <= _duration);
        const DestKey first = {_track, -1.f, {0.f, raw_key.value}};
        _dest->push_back(first);
        const DestKey last = {_track, 0.f, {_duration, raw_key.value}};
        _dest->push_back(last);
    } else {  // Copies all keys, and fixes up first and last keys.
        float prev_time = -1.f;
        if (_src.front().time != 0.f) {  // Needs a key at t = 0.f.
            const DestKey first = {_track, prev_time, {0.f, _src.front().value}};
            _dest->push_back(first);
            prev_time = 0.f;
        }
        for (size_t k = 0; k < _src.size(); ++k) {  // Copies all keys.
            const SrcKey& raw_key = _src[k];
            assert(raw_key.time >= 0 && raw_key.time <= _duration);
            const DestKey key = {_track, prev_time, {raw_key.time, raw_key.value}};
            _dest->push_back(key);
            prev_time = raw_key.time;
        }
        if (_src.back().time - _duration != 0.f) {  // Needs a key at t = _duration.
            const DestKey last = {_track, prev_time, {_duration, _src.back().value}};
            _dest->push_back(last);
        }
    }
    assert(_dest->front().key.time == 0.f && _dest->back().key.time - _duration == 0.f);
}

template <typename SortingKey>
void CopyToAnimation(vox::vector<SortingKey>* _src, vox::span<Float3Key>* _dest, float _inv_duration) {
    const size_t src_count = _src->size();
    if (!src_count) {
        return;
    }

    // Sort animation keys to favor cache coherency.
    std::sort(&_src->front(), (&_src->back()) + 1, &SortingKeyLess<SortingKey>);

    // Fills output.
    const SortingKey* src = &_src->front();
    for (size_t i = 0; i < src_count; ++i) {
        Float3Key& key = (*_dest)[i];
        key.ratio = src[i].key.time * _inv_duration;
        key.track = src[i].track;
        key.value[0] = vox::simd_math::FloatToHalf(src[i].key.value.x);
        key.value[1] = vox::simd_math::FloatToHalf(src[i].key.value.y);
        key.value[2] = vox::simd_math::FloatToHalf(src[i].key.value.z);
    }
}

// Compares float absolute values.
bool LessAbs(float _left, float _right) { return std::abs(_left) < std::abs(_right); }

// Compresses quaternion to vox::animation::RotationKey format.
// The 3 smallest components of the quaternion are quantized to 16 bits
// integers, while the largest is recomputed thanks to quaternion normalization
// property (x^2+y^2+z^2+w^2 = 1). Because the 3 components are the 3 smallest,
// their value cannot be greater than sqrt(2)/2. Thus, quantization quality is
// improved by pre-multiplying each componenent by sqrt(2).
void CompressQuat(const vox::QuaternionF& _src, vox::animation::QuaternionKey* _dest) {
    // Finds the largest quaternion component.
    const float quat[4] = {_src.x, _src.y, _src.z, _src.w};
    const ptrdiff_t largest = std::max_element(quat, quat + 4, LessAbs) - quat;
    assert(largest <= 3);
    _dest->largest = largest & 0x3;

    // Stores the sign of the largest component.
    _dest->sign = quat[largest] < 0.f;

    // Quantize the 3 smallest components on 16 bits signed integers.
    const float kFloat2Int = 32767.f * vox::kSqrt2;
    const int kMapping[4][3] = {{1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2}};
    const int* map = kMapping[largest];
    const int a = static_cast<int>(std::floor(quat[map[0]] * kFloat2Int + .5f));
    const int b = static_cast<int>(std::floor(quat[map[1]] * kFloat2Int + .5f));
    const int c = static_cast<int>(std::floor(quat[map[2]] * kFloat2Int + .5f));
    _dest->value[0] = vox::clamp(-32767, a, 32767) & 0xffff;
    _dest->value[1] = vox::clamp(-32767, b, 32767) & 0xffff;
    _dest->value[2] = vox::clamp(-32767, c, 32767) & 0xffff;
}

// Specialize for rotations in order to normalize quaternions.
// Consecutive opposite quaternions are also fixed up in order to avoid checking
// for the smallest path during the NLerp runtime algorithm.
void CopyToAnimation(vox::vector<SortingRotationKey>* _src, vox::span<QuaternionKey>* _dest, float _inv_duration) {
    const size_t src_count = _src->size();
    if (!src_count) {
        return;
    }

    // Normalize quaternions.
    // Also, fixes-up successive opposite quaternions that would fail to take the
    // shortest path during the normalized-lerp.
    // Note that keys are still sorted per-track at that point, which allows this
    // algorithm to process all consecutive keys.
    size_t track = std::numeric_limits<size_t>::max();
    const QuaternionF identity = QuaternionF::makeIdentity();
    SortingRotationKey* src = &_src->front();
    for (size_t i = 0; i < src_count; ++i) {
        QuaternionF normalized = normalizeSafe(src[i].key.value, identity);
        if (track != src[i].track) {       // First key of the track.
            if (normalized.w < 0.f) {      // .w eq to a dot with identity quaternion.
                normalized = -normalized;  // Q an -Q are the same rotation.
            }
        } else {  // Still on the same track: so fixes-up quaternion.
            const Vector4F prev(src[i - 1].key.value.x, src[i - 1].key.value.y, src[i - 1].key.value.z,
                                src[i - 1].key.value.w);
            const Vector4F curr(normalized.x, normalized.y, normalized.z, normalized.w);
            if (prev.dot(curr) < 0.f) {
                normalized = -normalized;  // Q an -Q are the same rotation.
            }
        }
        // Stores fixed-up quaternion.
        src[i].key.value = normalized;
        track = src[i].track;
    }

    // Sort.
    std::sort(array_begin(*_src), array_end(*_src), &SortingKeyLess<SortingRotationKey>);

    // Fills rotation keys output.
    for (size_t i = 0; i < src_count; ++i) {
        const SortingRotationKey& skey = src[i];
        QuaternionKey& dkey = (*_dest)[i];
        dkey.ratio = skey.key.time * _inv_duration;
        dkey.track = skey.track;

        // Compress quaternion to destination container.
        CompressQuat(skey.key.value, &dkey);
    }
}
}  // namespace

// Ensures _input's validity and allocates _animation.
// An animation needs to have at least two key frames per joint, the first at
// t = 0 and the last at t = duration. If at least one of those keys are not
// in the RawAnimation then the builder creates it.
unique_ptr<Animation> AnimationBuilder::operator()(const RawAnimation& _input) const {
    // Tests _raw_animation validity.
    if (!_input.Validate()) {
        return nullptr;
    }

    // Everything is fine, allocates and fills the animation.
    // Nothing can fail now.
    unique_ptr<Animation> animation = make_unique<Animation>();

    // Sets duration.
    const float duration = _input.duration;
    const float inv_duration = 1.f / _input.duration;
    animation->duration_ = duration;
    // A _duration == 0 would create some division by 0 during sampling.
    // Also, we need at least to keys with different times, which cannot be done
    // if duration is 0.
    assert(duration > 0.f);  // This case is handled by Validate().

    // Sets tracks count. Can be safely cast to uint16_t as number of tracks as
    // already been validated.
    const auto num_tracks = static_cast<uint16_t>(_input.num_tracks());
    animation->num_tracks_ = num_tracks;
    const uint16_t num_soa_tracks = align(num_tracks, 4);

    // Declares and preallocates tracks to sort.
    size_t translations = 0, rotations = 0, scales = 0;
    for (uint16_t i = 0; i < num_tracks; ++i) {
        const RawAnimation::JointTrack& raw_track = _input.tracks[i];
        translations += raw_track.translations.size() + 2;  // +2 because worst case
        rotations += raw_track.rotations.size() + 2;        // needs to add the
        scales += raw_track.scales.size() + 2;              // first and last keys.
    }
    vox::vector<SortingTranslationKey> sorting_translations;
    sorting_translations.reserve(translations);
    vox::vector<SortingRotationKey> sorting_rotations;
    sorting_rotations.reserve(rotations);
    vox::vector<SortingScaleKey> sorting_scales;
    sorting_scales.reserve(scales);

    // Filters RawAnimation keys and copies them to the output sorting structure.
    uint16_t i = 0;
    for (; i < num_tracks; ++i) {
        const RawAnimation::JointTrack& raw_track = _input.tracks[i];
        CopyRaw(raw_track.translations, i, duration, &sorting_translations);
        CopyRaw(raw_track.rotations, i, duration, &sorting_rotations);
        CopyRaw(raw_track.scales, i, duration, &sorting_scales);
    }

    // Add enough identity keys to match soa requirements.
    for (; i < num_soa_tracks; ++i) {
        typedef RawAnimation::TranslationKey SrcTKey;
        PushBackIdentityKey<SrcTKey>(i, 0.f, &sorting_translations);
        PushBackIdentityKey<SrcTKey>(i, duration, &sorting_translations);

        typedef RawAnimation::RotationKey SrcRKey;
        PushBackIdentityKey<SrcRKey>(i, 0.f, &sorting_rotations);
        PushBackIdentityKey<SrcRKey>(i, duration, &sorting_rotations);

        typedef RawAnimation::ScaleKey SrcSKey;
        PushBackIdentityKey<SrcSKey>(i, 0.f, &sorting_scales);
        PushBackIdentityKey<SrcSKey>(i, duration, &sorting_scales);
    }

    // Allocate animation members.
    animation->Allocate(_input.name.length(), sorting_translations.size(), sorting_rotations.size(),
                        sorting_scales.size());

    // Copy sorted keys to final animation.
    CopyToAnimation(&sorting_translations, &animation->translations_, inv_duration);
    CopyToAnimation(&sorting_rotations, &animation->rotations_, inv_duration);
    CopyToAnimation(&sorting_scales, &animation->scales_, inv_duration);

    // Copy animation's name.
    if (animation->name_) {
        strcpy(animation->name_, _input.name.c_str());
    }

    return animation;  // Success.
}
}  // namespace vox::animation::offline
