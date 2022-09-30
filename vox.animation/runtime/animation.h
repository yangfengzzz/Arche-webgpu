//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/export.h"
#include "vox.base/io/archive_traits.h"
#include "vox.base/macros.h"
#include "vox.base/span.h"

namespace vox {
namespace io {
class IArchive;
class OArchive;
}  // namespace io
namespace animation {

// Forward declares the AnimationBuilder, used to instantiate an Animation.
namespace offline {
class AnimationBuilder;
}

// Forward declaration of key frame's type.
struct Float3Key;
struct QuaternionKey;

// Defines a runtime skeletal animation clip.
// The runtime animation data structure stores animation keyframes, for all the
// joints of a skeleton. This structure is usually filled by the
// AnimationBuilder and deserialized/loaded at runtime.
// For each transformation type (translation, rotation and scale), Animation
// structure stores a single array of keyframes that contains all the tracks
// required to animate all the joints of a skeleton, matching breadth-first
// joints order of the runtime skeleton structure. In order to optimize cache
// coherency when sampling the animation, Keyframes in this array are sorted by
// time, then by track number.
class VOX_ANIMATION_DLL Animation {
public:
    // Builds a default animation.
    Animation();

    // Allow moves.
    Animation(Animation&&) noexcept;
    Animation& operator=(Animation&&) noexcept;

    // Delete copies.
    Animation(Animation const&) = delete;
    Animation& operator=(Animation const&) = delete;

    // Declares the public non-virtual destructor.
    ~Animation();

    // Gets the animation clip duration.
    [[nodiscard]] float duration() const { return duration_; }

    // Gets the number of animated tracks.
    [[nodiscard]] int num_tracks() const { return num_tracks_; }

    // Returns the number of SoA elements matching the number of tracks of *this
    // animation. This value is useful to allocate SoA runtime data structures.
    [[nodiscard]] int num_soa_tracks() const { return (num_tracks_ + 3) / 4; }

    // Gets animation name.
    [[nodiscard]] const char* name() const { return name_ ? name_ : ""; }

    // Gets the buffer of translations keys.
    [[nodiscard]] span<const Float3Key> translations() const { return translations_; }

    // Gets the buffer of rotation keys.
    [[nodiscard]] span<const QuaternionKey> rotations() const { return rotations_; }

    // Gets the buffer of scale keys.
    [[nodiscard]] span<const Float3Key> scales() const { return scales_; }

    // Get the estimated animation's size in bytes.
    [[nodiscard]] size_t size() const;

    // Serialization functions.
    // Should not be called directly but through io::Archive << and >> operators.
    void Save(vox::io::OArchive& _archive) const;
    void Load(vox::io::IArchive& _archive, uint32_t _version);

private:
    // AnimationBuilder class is allowed to instantiate an Animation.
    friend class offline::AnimationBuilder;

    // Internal destruction function.
    void Allocate(size_t _name_len, size_t _translation_count, size_t _rotation_count, size_t _scale_count);
    void Deallocate();

    // Duration of the animation clip.
    float duration_{};

    // The number of joint tracks. Can differ from the data stored in translation/
    // rotation/scale buffers because of SoA requirements.
    int num_tracks_{};

    // Animation name.
    char* name_{};

    // Stores all translation/rotation/scale keys begin and end of buffers.
    span<Float3Key> translations_;
    span<QuaternionKey> rotations_;
    span<Float3Key> scales_;
};
}  // namespace animation

namespace io {
VOX_IO_TYPE_VERSION(6, animation::Animation)
VOX_IO_TYPE_TAG("ozz-animation", animation::Animation)
}  // namespace io
}  // namespace vox