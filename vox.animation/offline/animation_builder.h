//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/export.h"
#include "vox.base/memory/unique_ptr.h"

namespace vox::animation {

// Forward declares the runtime animation type.
class Animation;

namespace offline {

// Forward declares the offline animation type.
struct RawAnimation;

// Defines the class responsible for building runtime animation instances from
// offline raw animations.
// No optimization at all is performed on the raw animation.
class VOX_ANIMOFFLINE_DLL AnimationBuilder {
public:
    // Creates an Animation based on _raw_animation and *this builder parameters.
    // Returns a valid Animation on success.
    // See RawAnimation::Validate() for more details about failure reasons.
    // The animation is returned as a unique_ptr as ownership is given back to
    // the caller.
    unique_ptr<Animation> operator()(const RawAnimation& _raw_animation) const;
};
}  // namespace offline
}  // namespace vox::animation