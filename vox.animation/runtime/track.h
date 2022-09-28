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
#include "vox.math/quaternion.h"

namespace vox {
namespace animation {

// Forward declares the TrackBuilder, used to instantiate a Track.
namespace offline {
class TrackBuilder;
}

namespace internal {
// Runtime user-channel track internal implementation.
// The runtime track data structure exists for 1 to 4 float types (FloatTrack,
// ..., Float4Track) and quaternions (QuaternionTrack). See RawTrack for more
// details on track content. The runtime track data structure is optimized for
// the processing of vox::animation::TrackSamplingJob and
// vox::animation::TrackTriggeringJob. Keyframe ratios, values and interpolation
// mode are all store as separate buffers in order to access the cache
// coherently. Ratios are usually accessed/read alone from the jobs that all
// start by looking up the keyframes to interpolate indeed.
template <typename T>
class VOX_ANIMATION_DLL Track {
public:
    typedef T ValueType;

    Track();

    // Allow move.
    Track(Track&& _other) noexcept;
    Track& operator=(Track&& _other) noexcept;

    // Disables copy and assignation.
    Track(Track const&) = delete;
    void operator=(Track const&) = delete;

    ~Track();

    // Keyframe accessors.
    [[nodiscard]] span<const float> ratios() const { return ratios_; }
    [[nodiscard]] span<const T> values() const { return values_; }
    [[nodiscard]] span<const uint8_t> steps() const { return steps_; }

    // Get the estimated track's size in bytes.
    [[nodiscard]] size_t size() const;

    // Get track name.
    [[nodiscard]] const char* name() const { return name_ ? name_ : ""; }

    // Serialization functions.
    // Should not be called directly but through io::Archive << and >> operators.
    void Save(vox::io::OArchive& _archive) const;
    void Load(vox::io::IArchive& _archive, uint32_t _version);

private:
    // TrackBuilder class is allowed to allocate a Track.
    friend class offline::TrackBuilder;

    // Internal destruction function.
    void Allocate(size_t _keys_count, size_t _name_len);
    void Deallocate();

    // Keyframe ratios (0 is the beginning of the track, 1 is the end).
    span<float> ratios_;

    // Keyframe values.
    span<T> values_;

    // Keyframe modes (1 bit per key): 1 for step, 0 for linear.
    span<uint8_t> steps_;

    // Track name.
    char* name_ = nullptr;
};

// Definition of operations policies per track value type.
template <typename ValueType>
struct TrackPolicy {
    inline static ValueType Lerp(const ValueType& _a, const ValueType& _b, float _alpha) {
        return vox::lerp(_a, _b, _alpha);
    }
    inline static float Distance(const ValueType& _a, const ValueType& _b) { return (_a - _b).length(); }
    inline static ValueType identity() { return ValueType(0.f); }
};

// Specialization for float policy.
template <>
inline float TrackPolicy<float>::Distance(const float& _a, const float& _b) {
    return std::abs(_a - _b);
}

// Specialization for quaternions policy.
template <>
inline QuaternionF TrackPolicy<QuaternionF>::Lerp(const QuaternionF& _a, const QuaternionF& _b, float _alpha) {
    // Uses NLerp to favor speed. This same function is used when optimizing the
    // curve (key frame reduction), so "constant speed" interpolation can still be
    // approximated with a lower tolerance value if it matters.
    return vox::nLerp(_a, _b, _alpha);
}
template <>
inline float TrackPolicy<QuaternionF>::Distance(const QuaternionF& _a, const QuaternionF& _b) {
    const float cos_half_angle = _a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w;
    // Return value is 1 - half cosine, so the closer the quaternions, the closer
    // to 0.
    return 1.f - std::min(1.f, std::abs(cos_half_angle));
}
template <>
inline QuaternionF TrackPolicy<QuaternionF>::identity() {
    return QuaternionF::makeIdentity();
}
}  // namespace internal

// Runtime track data structure instantiation.
class VOX_ANIMATION_DLL FloatTrack : public internal::Track<float> {};
class VOX_ANIMATION_DLL Float2Track : public internal::Track<Vector2F> {};
class VOX_ANIMATION_DLL Float3Track : public internal::Track<Vector3F> {};
class VOX_ANIMATION_DLL Float4Track : public internal::Track<Vector4F> {};
class VOX_ANIMATION_DLL QuaternionTrack : public internal::Track<QuaternionF> {};

}  // namespace animation
namespace io {
VOX_IO_TYPE_VERSION(1, animation::FloatTrack)
VOX_IO_TYPE_TAG("vox-float_track", animation::FloatTrack)
VOX_IO_TYPE_VERSION(1, animation::Float2Track)
VOX_IO_TYPE_TAG("vox-float2_track", animation::Float2Track)
VOX_IO_TYPE_VERSION(1, animation::Float3Track)
VOX_IO_TYPE_TAG("vox-float3_track", animation::Float3Track)
VOX_IO_TYPE_VERSION(1, animation::Float4Track)
VOX_IO_TYPE_TAG("vox-float4_track", animation::Float4Track)
VOX_IO_TYPE_VERSION(1, animation::QuaternionTrack)
VOX_IO_TYPE_TAG("vox-quat_track", animation::QuaternionTrack)
}  // namespace io
}  // namespace vox