//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/export.h"
#include "vox.base/macros.h"
#include "vox.base/span.h"

namespace vox {

struct ScalableTransform;

namespace animation::offline {

// Forward declare offline animation type.
struct RawAnimation;

// Defines the class responsible for building a delta animation from an offline
// raw animation. This is used to create animations compatible with additive
// blending.
class VOX_ANIMOFFLINE_DLL AdditiveAnimationBuilder {
public:
    // Initializes the builder.
    AdditiveAnimationBuilder();

    // Builds delta animation from _input.
    // Returns true on success and fills _output_animation with the delta
    // version of _input animation.
    // *_output must be a valid RawAnimation instance. Uses first frame as
    // reference pose Returns false on failure and resets _output to an empty
    // animation. See RawAnimation::Validate() for more details about failure
    // reasons.
    bool operator()(const RawAnimation& _input, RawAnimation* _output) const;

    // Builds delta animation from _input.
    // Returns true on success and fills _output_animation with the delta
    // *_output must be a valid RawAnimation instance.
    // version of _input animation.
    // *_reference_pose used as the base pose to calculate deltas from
    // Returns false on failure and resets _output to an empty animation.
    bool operator()(const RawAnimation& _input,
                    const span<const ScalableTransform>& _reference_pose,
                    RawAnimation* _output) const;
};
}  // namespace animation::offline
}  // namespace vox