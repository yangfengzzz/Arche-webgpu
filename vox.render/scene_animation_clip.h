//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "vox.render/entity.h"

namespace vox {
class SceneAnimationClip {
public:
    struct AnimationChannel {
        enum PathType { TRANSLATION, ROTATION, SCALE };
        PathType path;
        Entity *node;
        uint32_t samplerIndex;
    };

    struct AnimationSampler {
        enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
        InterpolationType interpolation;
        std::vector<float> inputs;
        std::vector<Vector4F> outputsVec4;
    };

public:
    explicit SceneAnimationClip(const std::string &name);

    void update(float deltaTime);

    [[nodiscard]] const std::string &name() const;

    [[nodiscard]] float start() const;

    void setStart(float time);

    [[nodiscard]] float end() const;

    void setEnd(float time);

    void addSampler(const AnimationSampler &sampler);

    void addChannel(const AnimationChannel &channel);

private:
    std::string _name;
    std::vector<AnimationSampler> _samplers;
    std::vector<AnimationChannel> _channels;
    float _start = std::numeric_limits<float>::max();
    float _end = std::numeric_limits<float>::min();

    float _currentTime = 0.0f;
};

}  // namespace vox