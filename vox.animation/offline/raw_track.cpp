//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/raw_track.h"

#include <limits>

#include "vox.base/containers/string_archive.h"
#include "vox.base/containers/vector_archive.h"
#include "vox.math/math_archive.h"

namespace vox {
namespace io {

// Can be declared locally as it's only referenced from this file.
VOX_IO_TYPE_VERSION_T1(1, typename _ValueType, animation::offline::RawTrackKeyframe<_ValueType>)

template <typename ValueType>
struct Extern<animation::offline::RawTrackKeyframe<ValueType>> {
    static void Save(OArchive& _archive,
                     const animation::offline::RawTrackKeyframe<ValueType>* _keyframes,
                     size_t _count) {
        for (size_t i = 0; i < _count; ++i) {
            const animation::offline::RawTrackKeyframe<ValueType>& keyframe = _keyframes[i];
            const auto interp = static_cast<uint8_t>(keyframe.interpolation);
            _archive << interp;
            _archive << keyframe.ratio;
            _archive << keyframe.value;
        }
    }
    static void Load(IArchive& _archive,
                     animation::offline::RawTrackKeyframe<ValueType>* _keyframes,
                     size_t _count,
                     uint32_t _version) {
        (void)_version;
        for (size_t i = 0; i < _count; ++i) {
            animation::offline::RawTrackKeyframe<ValueType>& keyframe = _keyframes[i];
            uint8_t interp;
            _archive >> interp;
            keyframe.interpolation = static_cast<animation::offline::RawTrackInterpolation::Value>(interp);
            _archive >> keyframe.ratio;
            _archive >> keyframe.value;
        }
    }
};
}  // namespace io
namespace animation::offline::internal {

template <typename ValueType>
bool RawTrack<ValueType>::Validate() const {
    float previous_ratio = -1.f;
    for (size_t k = 0; k < keyframes.size(); ++k) {
        const float frame_ratio = keyframes[k].ratio;
        // Tests frame's ratio is in range [0:1].
        if (frame_ratio < 0.f || frame_ratio > 1.f) {
            return false;
        }
        // Tests that frames are sorted.
        if (frame_ratio <= previous_ratio) {
            return false;
        }
        previous_ratio = frame_ratio;
    }
    return true;  // Validated.
}

template <typename ValueType>
void RawTrack<ValueType>::Save(io::OArchive& _archive) const {
    _archive << keyframes;
    _archive << name;
}

template <typename ValueType>
void RawTrack<ValueType>::Load(io::IArchive& _archive, uint32_t _version) {
    (void)_version;
    assert(_version == 1);
    _archive >> keyframes;
    _archive >> name;
}

// Explicitly instantiate supported raw tracks.
template struct RawTrack<float>;
template struct RawTrack<Vector2F>;
template struct RawTrack<Vector3F>;
template struct RawTrack<Vector4F>;
template struct RawTrack<QuaternionF>;
}  // namespace animation::offline::internal
}  // namespace vox
