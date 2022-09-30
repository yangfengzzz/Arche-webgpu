//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/export.h"
#include "vox.base/containers/string.h"
#include "vox.base/containers/vector.h"
#include "vox.base/io/archive_traits.h"
#include "vox.math/quaternion.h"

namespace vox {
namespace animation::offline {

// Interpolation mode.
struct RawTrackInterpolation {
    enum Value {
        kStep,    // All values following this key, up to the next key, are equal.
        kLinear,  // All value between this key and the next are linearly
                  // interpolated.
    };
};

// Keyframe data structure.
template <typename T>
struct RawTrackKeyframe {
    typedef T ValueType;
    RawTrackInterpolation::Value interpolation;
    float ratio;
    ValueType value;
};

namespace internal {

// Offline user-channel animation track type implementation.
// This offline track data structure is meant to be used for user-channel
// tracks, aka animation of variables that aren't joint transformation. It is
// available for tracks of 1 to 4 floats (RawFloatTrack, RawFloat2Track, ...,
// RawFloat4Track) and quaternions (RawQuaternionTrack). Quaternions differ from
// float4 because of the specific interpolation and comparison treatment they
// require. As all other Raw data types, they are not intended to be used in run
// time. They are used to define the offline track object that can be converted
// to the runtime one using the vox::animation::offline::TrackBuilder. This
// animation structure exposes a single sequence of keyframes. Keyframes are
// defined with a ratio, a value and an interpolation mode:
// - Ratio: A track has no duration, so it uses ratios between 0 (beginning of
// the track) and 1 (the end), instead of times. This allows to avoid any
// discrepancy between the durations of tracks and the animation they match
// with.
// - Value: The animated value (float, ... float4, quaternion).
// - Interpolation mode (`vox::animation::offline::RawTrackInterpolation`):
// Defines how value is interpolated with the next key. Track structure is then
// a sorted vector of keyframes. RawTrack structure exposes a Validate()
// function to check that all the following rules are respected:
// 1. Keyframes' ratios are sorted in a strict ascending order.
// 2. Keyframes' ratios are all within [0,1] range.
// RawTrack that would fail this validation will fail to be converted by
// the RawTrackBuilder.
template <typename T>
struct VOX_ANIMOFFLINE_DLL RawTrack {
    typedef T ValueType;
    typedef RawTrackKeyframe<ValueType> Keyframe;

    // Validates that all the following rules are respected:
    //  1. Keyframes' ratios are sorted in a strict ascending order.
    //  2. Keyframes' ratios are all within [0,1] range.
    [[nodiscard]] bool Validate() const;

    // Uses intrusive serialization option, as a way to factorize code.
    // Version and Tag should still be defined for each specialization.
    void Save(io::OArchive& _archive) const;
    void Load(io::IArchive& _archive, uint32_t _version);

    // Sequence of keyframes, expected to be sorted.
    typedef typename vox::vector<Keyframe> Keyframes;
    Keyframes keyframes;

    // Name of the track.
    string name;
};
}  // namespace internal

// Offline user-channel animation track type instantiation.
struct VOX_ANIMOFFLINE_DLL RawFloatTrack : public internal::RawTrack<float> {};
struct VOX_ANIMOFFLINE_DLL RawFloat2Track : public internal::RawTrack<Vector2F> {};
struct VOX_ANIMOFFLINE_DLL RawFloat3Track : public internal::RawTrack<Vector3F> {};
struct VOX_ANIMOFFLINE_DLL RawFloat4Track : public internal::RawTrack<Vector4F> {};
struct VOX_ANIMOFFLINE_DLL RawQuaternionTrack : public internal::RawTrack<QuaternionF> {};
}  // namespace animation::offline

namespace io {
VOX_IO_TYPE_VERSION(1, animation::offline::RawFloatTrack)
VOX_IO_TYPE_TAG("ozz-raw_float_track", animation::offline::RawFloatTrack)
VOX_IO_TYPE_VERSION(1, animation::offline::RawFloat2Track)
VOX_IO_TYPE_TAG("ozz-raw_float2_track", animation::offline::RawFloat2Track)
VOX_IO_TYPE_VERSION(1, animation::offline::RawFloat3Track)
VOX_IO_TYPE_TAG("ozz-raw_float3_track", animation::offline::RawFloat3Track)
VOX_IO_TYPE_VERSION(1, animation::offline::RawFloat4Track)
VOX_IO_TYPE_TAG("ozz-raw_float4_track", animation::offline::RawFloat4Track)
VOX_IO_TYPE_VERSION(1, animation::offline::RawQuaternionTrack)
VOX_IO_TYPE_TAG("ozz-raw_quat_track", animation::offline::RawQuaternionTrack)
}  // namespace io
}  // namespace vox