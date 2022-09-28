//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/runtime/animation_utils.h"

// Internal include file
#define VOX_INCLUDE_PRIVATE_HEADER  // Allows to include private headers.
#include "vox.animation/runtime/animation_keyframe.h"

namespace vox::animation {

template <typename Key>
inline int CountKeyframesImpl(const span<const Key>& _keys, int _track) {
    if (_track < 0) {
        return static_cast<int>(_keys.size());
    }

    int count = 0;
    for (const Key& key : _keys) {
        if (key.track == _track) {
            ++count;
        }
    }
    return count;
}

int CountTranslationKeyframes(const Animation& _animation, int _track) {
    return CountKeyframesImpl(_animation.translations(), _track);
}
int CountRotationKeyframes(const Animation& _animation, int _track) {
    return CountKeyframesImpl(_animation.rotations(), _track);
}
int CountScaleKeyframes(const Animation& _animation, int _track) {
    return CountKeyframesImpl(_animation.scales(), _track);
}
}  // namespace vox::animation
