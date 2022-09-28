//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/export.h"
#include "vox.base/macros.h"
#include "vox.base/span.h"

namespace vox {

// Forward declaration of math structures.
namespace simd_math {
struct SoaTransform;
}

namespace animation {

// Forward declares the animation type to sample.
class Animation;

// Samples an animation at a given time ratio in the unit interval [0,1] (where
// 0 is the beginning of the animation, 1 is the end), to output the
// corresponding posture in local-space.
// SamplingJob uses a context (aka SamplingJob::Context) to store intermediate
// values (decompressed animation keyframes...) while sampling. This context
// also stores pre-computed values that allows drastic optimization while
// playing/sampling the animation forward. Backward sampling works, but isn't
// optimized through the context. The job does not own the buffers (in/output)
// and will thus not delete them during job's destruction.
struct VOX_ANIMATION_DLL SamplingJob {
    // Default constructor, initializes default values.
    SamplingJob();

    // Validates job parameters. Returns true for a valid job, or false otherwise:
    // -if any input pointer is nullptr
    // -if output range is invalid.
    [[nodiscard]] bool Validate() const;

    // Runs job's sampling task.
    // The job is validated before any operation is performed, see Validate() for
    // more details.
    // Returns false if *this job is not valid.
    [[nodiscard]] bool Run() const;

    // Time ratio in the unit interval [0,1] used to sample animation (where 0 is
    // the beginning of the animation, 1 is the end). It should be computed as the
    // current time in the animation , divided by animation duration.
    // This ratio is clamped before job execution in order to resolve any
    // approximation issue on range bounds.
    float ratio;

    // The animation to sample.
    const Animation* animation;

    // Forward declares the context object used by the SamplingJob.
    class Context;

    // A context object that must be big enough to sample *this animation.
    Context* context;

    // Job output.
    // The output range to be filled with sampled joints during job execution.
    // If there are fewer joints in the animation compared to the output range,
    // then remaining SoaTransform are left unchanged.
    // If there are more joints in the animation, then the last joints are not
    // sampled.
    span<vox::simd_math::SoaTransform> output;
};

namespace internal {
// Soa hot data to interpolate.
struct InterpSoaFloat3;
struct InterpSoaQuaternion;
}  // namespace internal

// Declares the context object used by the workload to take advantage of the
// frame coherency of animation sampling.
class VOX_ANIMATION_DLL SamplingJob::Context {
public:
    // Constructs an empty context. The context needs to be resized with the
    // appropriate number of tracks before it can be used with a SamplingJob.
    Context();

    // Constructs a context that can be used to sample any animation with at most
    // _max_tracks tracks. _num_tracks is internally aligned to a multiple of
    // soa size, which means max_tracks() can return a different (but bigger)
    // value than _max_tracks.
    explicit Context(int _max_tracks);

    // Disables copy and assignation.
    Context(Context const&) = delete;
    Context& operator=(Context const&) = delete;

    // Deallocates context.
    ~Context();

    // Resize the number of joints that the context can support.
    // This also implicitly invalidate the context.
    void Resize(int _max_tracks);

    // Invalidate the context.
    // The SamplingJob automatically invalidates a context when required
    // during sampling. This automatic mechanism is based on the animation
    // address and sampling time ratio. The weak point is that it can result in a
    // crash if ever the address of an animation is used again with another
    // animation (could be the result of successive call to delete / new).
    // Therefore, it is recommended to manually invalidate a context when it is
    // known that this context will not be used for with an animation again.
    void Invalidate();

    // The maximum number of tracks that the context can handle.
    [[nodiscard]] int max_tracks() const { return max_soa_tracks_ * 4; }
    [[nodiscard]] int max_soa_tracks() const { return max_soa_tracks_; }

private:
    friend struct SamplingJob;

    // Steps the context in order to use it for a potentially new animation and
    // ratio. If the _animation is different from the animation currently cached,
    // or if the _ratio shows that the animation is played backward, then the
    // context is invalidated and reset for the new _animation and _ratio.
    void Step(const Animation& _animation, float _ratio);

    // The animation this context refers to. nullptr means that the context is
    // invalid.
    const Animation* animation_{};

    // The current time ratio in the animation.
    float ratio_{};

    // The number of soa tracks that can store this context.
    int max_soa_tracks_;

    // Soa hot data to interpolate.
    internal::InterpSoaFloat3* soa_translations_;
    internal::InterpSoaQuaternion* soa_rotations_{};
    internal::InterpSoaFloat3* soa_scales_{};

    // Points to the keys in the animation that are valid for the current time
    // ratio.
    int* translation_keys_{};
    int* rotation_keys_{};
    int* scale_keys_{};

    // Current cursors in the animation. 0 means that the context is invalid.
    int translation_cursor_{};
    int rotation_cursor_{};
    int scale_cursor_{};

    // Outdated soa entries. One bit per soa entry (32 joints per byte).
    uint8_t* outdated_translations_{};
    uint8_t* outdated_rotations_{};
    uint8_t* outdated_scales_{};
};
}  // namespace animation
}  // namespace vox