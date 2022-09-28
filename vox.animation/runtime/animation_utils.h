//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/animation.h"
#include "vox.animation/runtime/export.h"

namespace vox::animation {

// Count translation, rotation or scale keyframes for a given track number. Use
// a negative _track value to count all tracks.
VOX_ANIMATION_DLL int CountTranslationKeyframes(const Animation& _animation, int _track = -1);
VOX_ANIMATION_DLL int CountRotationKeyframes(const Animation& _animation, int _track = -1);
VOX_ANIMATION_DLL int CountScaleKeyframes(const Animation& _animation, int _track = -1);
}  // namespace vox::animation