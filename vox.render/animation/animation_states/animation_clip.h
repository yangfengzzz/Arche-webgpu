//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/animation.h"
#include "vox.animation/runtime/sampling_job.h"
#include "vox.base/containers/vector.h"
#include "vox.render/animation/animation_state.h"

namespace vox {

class AnimationClip : public AnimationState {
public:
    explicit AnimationClip(const std::string& filename);

    // Allow moves.
    AnimationClip(AnimationClip&&) noexcept;
    AnimationClip& operator=(AnimationClip&&) noexcept;

    bool loadAnimation(const std::string& filename);

    void update(float dt) override;

    [[nodiscard]] const vox::vector<simd_math::SoaTransform>& locals() const override;

    void loadSkeleton(animation::Skeleton* skeleton) override;

    animation::Animation& animation();

public:
    // Playback speed, can be negative in order to play the animation backward.
    float playback_speed{1.f};

    // Animation play mode state: play/pause.
    bool play{true};

    // Animation loop mode.
    bool loop{true};

    // Sets animation current time.
    void setTimeRatio(float _time);

    // Gets animation current time.
    [[nodiscard]] float timeRatio() const;

    // Gets animation time ratio of last update. Useful when the range between
    // previous and current frame needs to pe processed.
    [[nodiscard]] float previousTimeRatio() const;

    // Resets all parameters to their default value.
    void reset();

private:
    animation::SamplingJob _sampling_job{};

    // Runtime animation.
    animation::Animation _animation{};

    // Sampling context.
    animation::SamplingJob::Context _context{};

    // Buffer of local transforms as sampled from main animation_.
    vox::vector<simd_math::SoaTransform> _locals;

    // Current animation time ratio, in the unit interval [0,1], where 0 is the
    // beginning of the animation, 1 is the end.
    float _time_ratio{0.f};

    // Time ratio of the previous update.
    float _previous_time_ratio{0.f};
};
}  // namespace vox
