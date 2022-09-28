//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/raw_animation.h"
#include "vox.base/containers/string_archive.h"
#include "vox.base/containers/vector_archive.h"
#include "vox.base/logging.h"
#include "vox.math/math_archive.h"

namespace vox::io {

void Extern<animation::offline::RawAnimation>::Save(OArchive& _archive,
                                                    const animation::offline::RawAnimation* _animations,
                                                    size_t _count) {
    for (size_t i = 0; i < _count; ++i) {
        const animation::offline::RawAnimation& animation = _animations[i];
        _archive << animation.duration;
        _archive << animation.tracks;
        _archive << animation.name;
    }
}

void Extern<animation::offline::RawAnimation>::Load(IArchive& _archive,
                                                    animation::offline::RawAnimation* _animations,
                                                    size_t _count,
                                                    uint32_t _version) {
    if (_version < 3) {
        LOGE("Unsupported RawAnimation version {}", _version)
        return;
    }
    for (size_t i = 0; i < _count; ++i) {
        animation::offline::RawAnimation& animation = _animations[i];
        _archive >> animation.duration;
        _archive >> animation.tracks;
        _archive >> animation.name;
    }
}

// RawAnimation::*Keys' version can be declared locally as it will be saved from
// this cpp file only.

VOX_IO_TYPE_VERSION(1, animation::offline::RawAnimation::JointTrack)

template <>
struct Extern<animation::offline::RawAnimation::JointTrack> {
    static void Save(OArchive& _archive, const animation::offline::RawAnimation::JointTrack* _tracks, size_t _count) {
        for (size_t i = 0; i < _count; ++i) {
            const animation::offline::RawAnimation::JointTrack& track = _tracks[i];
            _archive << track.translations;
            _archive << track.rotations;
            _archive << track.scales;
        }
    }
    static void Load(IArchive& _archive,
                     animation::offline::RawAnimation::JointTrack* _tracks,
                     size_t _count,
                     uint32_t _version) {
        (void)_version;
        for (size_t i = 0; i < _count; ++i) {
            animation::offline::RawAnimation::JointTrack& track = _tracks[i];
            _archive >> track.translations;
            _archive >> track.rotations;
            _archive >> track.scales;
        }
    }
};

VOX_IO_TYPE_VERSION(1, animation::offline::RawAnimation::TranslationKey)

template <>
struct Extern<animation::offline::RawAnimation::TranslationKey> {
    static void Save(OArchive& _archive, const animation::offline::RawAnimation::TranslationKey* _keys, size_t _count) {
        for (size_t i = 0; i < _count; ++i) {
            const animation::offline::RawAnimation::TranslationKey& key = _keys[i];
            _archive << key.time;
            _archive << key.value;
        }
    }
    static void Load(IArchive& _archive,
                     animation::offline::RawAnimation::TranslationKey* _keys,
                     size_t _count,
                     uint32_t _version) {
        (void)_version;
        for (size_t i = 0; i < _count; ++i) {
            animation::offline::RawAnimation::TranslationKey& key = _keys[i];
            _archive >> key.time;
            _archive >> key.value;
        }
    }
};

VOX_IO_TYPE_VERSION(1, animation::offline::RawAnimation::RotationKey)

template <>
struct Extern<animation::offline::RawAnimation::RotationKey> {
    static void Save(OArchive& _archive, const animation::offline::RawAnimation::RotationKey* _keys, size_t _count) {
        for (size_t i = 0; i < _count; ++i) {
            const animation::offline::RawAnimation::RotationKey& key = _keys[i];
            _archive << key.time;
            _archive << key.value;
        }
    }
    static void Load(IArchive& _archive,
                     animation::offline::RawAnimation::RotationKey* _keys,
                     size_t _count,
                     uint32_t _version) {
        (void)_version;
        for (size_t i = 0; i < _count; ++i) {
            animation::offline::RawAnimation::RotationKey& key = _keys[i];
            _archive >> key.time;
            _archive >> key.value;
        }
    }
};

VOX_IO_TYPE_VERSION(1, animation::offline::RawAnimation::ScaleKey)

template <>
struct Extern<animation::offline::RawAnimation::ScaleKey> {
    static void Save(OArchive& _archive, const animation::offline::RawAnimation::ScaleKey* _keys, size_t _count) {
        for (size_t i = 0; i < _count; ++i) {
            const animation::offline::RawAnimation::ScaleKey& key = _keys[i];
            _archive << key.time;
            _archive << key.value;
        }
    }
    static void Load(IArchive& _archive,
                     animation::offline::RawAnimation::ScaleKey* _keys,
                     size_t _count,
                     uint32_t _version) {
        (void)_version;
        for (size_t i = 0; i < _count; ++i) {
            animation::offline::RawAnimation::ScaleKey& key = _keys[i];
            _archive >> key.time;
            _archive >> key.value;
        }
    }
};
}  // namespace vox::io
