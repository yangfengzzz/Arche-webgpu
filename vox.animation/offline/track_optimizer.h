//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/export.h"
#include "vox.base/macros.h"

namespace vox::animation::offline {

// Forward declare offline track types.
struct RawFloatTrack;
struct RawFloat2Track;
struct RawFloat3Track;
struct RawFloat4Track;
struct RawQuaternionTrack;

// TrackOptimizer is responsible for optimizing an offline raw track instance.
// Optimization is a keyframe reduction process. Redundant and interpolable
// keyframes (within a tolerance value) are removed from the track. Default
// optimization tolerances are set in order to favor quality over runtime
// performances and memory footprint.
class VOX_ANIMOFFLINE_DLL TrackOptimizer {
public:
    // Initializes the optimizer with default tolerances (favoring quality).
    TrackOptimizer();

    // Optimizes _input using *these parameters.
    // Returns true on success and fills _output track with the optimized
    // version of _input track.
    // *_output must be a valid Raw*Track instance.
    // Returns false on failure and resets _output to an empty track.
    // See Raw*Track::Validate() for more details about failure reasons.
    bool operator()(const RawFloatTrack& _input, RawFloatTrack* _output) const;
    bool operator()(const RawFloat2Track& _input, RawFloat2Track* _output) const;
    bool operator()(const RawFloat3Track& _input, RawFloat3Track* _output) const;
    bool operator()(const RawFloat4Track& _input, RawFloat4Track* _output) const;
    bool operator()(const RawQuaternionTrack& _input, RawQuaternionTrack* _output) const;

    // Optimization tolerance.
    float tolerance;
};
}  // namespace vox::animation::offline