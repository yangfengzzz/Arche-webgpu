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

// Offline animation type.
// This animation type is not intended to be used in run time. It is used to
// define the offline animation object that can be converted to the runtime
// animation using the AnimationBuilder.
// This animation structure exposes tracks of keyframes. Keyframes are defined
// with a time and a value which can either be a translation (3 float x, y, z),
// a rotation (a quaternion) or scale coefficient (3 floats x, y, z). Tracks are
// defined as a set of three different std::vectors (translation, rotation and
// scales). Animation structure is then a vector of tracks, along with a
// duration value.
// Finally, the RawAnimation structure exposes Validate() function to check that
// it is valid, meaning that all the following rules are respected:
//  1. Animation duration is greater than 0.
//  2. Keyframes' time are sorted in a strict ascending order.
//  3. Keyframes' time are all within [0,animation duration] range.
// Animations that would fail this validation will fail to be converted by the
// AnimationBuilder.
struct VOX_ANIMOFFLINE_DLL RawAnimation {
    // Constructs a valid RawAnimation with a 1s default duration.
    RawAnimation();

    // Tests for *this validity.
    // Returns true if animation data (duration, tracks) is valid:
    //  1. Animation duration is greater than 0.
    //  2. Keyframes' time are sorted in a strict ascending order.
    //  3. Keyframes' time are all within [0,animation duration] range.
    [[nodiscard]] bool Validate() const;

    // Get the estimated animation's size in bytes.
    [[nodiscard]] size_t size() const;

    // Defines a raw translation key frame.
    struct TranslationKey {
        // Key frame time.
        float time;

        // Key frame value.
        typedef Vector3F Value;
        Value value;

        // Provides identity transformation for a translation key.
        static Vector3F identity() { return {}; }
    };

    // Defines a raw rotation key frame.
    struct RotationKey {
        // Key frame time.
        float time;

        // Key frame value.
        typedef QuaternionF Value;
        QuaternionF value;

        // Provides identity transformation for a rotation key.
        static QuaternionF identity() { return QuaternionF::makeIdentity(); }
    };

    // Defines a raw scaling key frame.
    struct ScaleKey {
        // Key frame time.
        float time;

        // Key frame value.
        typedef Vector3F Value;
        Vector3F value;

        // Provides identity transformation for a scale key.
        static Vector3F identity() { return {1, 1, 1}; }
    };

    // Defines a track of key frames for a bone, including translation, rotation
    // and scale.
    struct JointTrack {
        typedef vox::vector<TranslationKey> Translations;
        Translations translations;
        typedef vox::vector<RotationKey> Rotations;
        Rotations rotations;
        typedef vox::vector<ScaleKey> Scales;
        Scales scales;

        // Validates track. See RawAnimation::Validate for more details.
        // Use an infinite value for _duration if unknown. This will validate
        // keyframe orders, but not maximum duration.
        [[nodiscard]] bool Validate(float _duration) const;
    };

    // Returns the number of tracks of this animation.
    [[nodiscard]] int num_tracks() const { return static_cast<int>(tracks.size()); }

    // Stores per joint JointTrack, ie: per joint animation key-frames.
    // tracks_.size() gives the number of animated joints.
    vox::vector<JointTrack> tracks;

    // The duration of the animation. All the keys of a valid RawAnimation are in
    // the range [0,duration].
    float duration;

    // Name of the animation.
    vox::string name;
};
}  // namespace animation::offline
namespace io {
VOX_IO_TYPE_VERSION(3, animation::offline::RawAnimation)
VOX_IO_TYPE_TAG("ozz-raw_animation", animation::offline::RawAnimation)

// Should not be called directly but through io::Archive << and >> operators.
template <>
struct VOX_ANIMOFFLINE_DLL Extern<animation::offline::RawAnimation> {
    static void Save(OArchive& _archive, const animation::offline::RawAnimation* _animations, size_t _count);
    static void Load(IArchive& _archive,
                     animation::offline::RawAnimation* _animations,
                     size_t _count,
                     uint32_t _version);
};
}  // namespace io
}  // namespace vox