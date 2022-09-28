//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/export.h"
#include "vox.base/memory/unique_ptr.h"

namespace vox::animation {

// Forward declares the runtime tracks type.
class FloatTrack;
class Float2Track;
class Float3Track;
class Float4Track;
class QuaternionTrack;

namespace offline {

// Forward declares the offline tracks type.
struct RawFloatTrack;
struct RawFloat2Track;
struct RawFloat3Track;
struct RawFloat4Track;
struct RawQuaternionTrack;

// Defines the class responsible for building runtime track instances from
// offline tracks.The input raw track is first validated. Runtime conversion of
// a validated raw track cannot fail. Note that no optimization is performed on
// the data at all.
class VOX_ANIMOFFLINE_DLL TrackBuilder {
public:
    // Creates a Track based on _raw_track and *this builder parameters.
    // Returns a track instance on success, an empty unique_ptr on failure. See
    // Raw*Track::Validate() for more details about failure reasons.
    // The track is returned as a unique_ptr as ownership is given back to the
    // caller.
    vox::unique_ptr<FloatTrack> operator()(const RawFloatTrack& _input) const;
    vox::unique_ptr<Float2Track> operator()(const RawFloat2Track& _input) const;
    vox::unique_ptr<Float3Track> operator()(const RawFloat3Track& _input) const;
    vox::unique_ptr<Float4Track> operator()(const RawFloat4Track& _input) const;
    vox::unique_ptr<QuaternionTrack> operator()(const RawQuaternionTrack& _input) const;

private:
    template <typename RawTrack, typename Track>
    vox::unique_ptr<Track> Build(const RawTrack& _input) const;
};
}  // namespace offline
}  // namespace vox::animation