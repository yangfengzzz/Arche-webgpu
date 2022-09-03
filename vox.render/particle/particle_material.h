//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector3.h"
#include "vox.render/material/base_material.h"

namespace vox {
class ParticleMaterial : public BaseMaterial {
public:
    enum class ColorMode : uint32_t { DEFAULT, GRADIENT, kNumColorMode };

    struct ParticleData {
        Vector3F birthGradient;
        float minParticleSize;
        Vector3F deathGradient;
        float maxParticleSize;
        ColorMode colorMode = ColorMode::DEFAULT;
        float fadeCoefficient;
        float debugDraw;
        float _pad;
    };

    [[nodiscard]] float minParticleSize() const;

    void setMinParticleSize(float size);

    [[nodiscard]] float maxParticleSize() const;

    void setMaxParticleSize(float size);

    [[nodiscard]] ColorMode colorMode() const;

    void setColorMode(ColorMode mode);

    [[nodiscard]] Vector3F birthGradient() const;

    void setBirthGradient(const Vector3F& gradient);

    [[nodiscard]] Vector3F deathGradient() const;

    void setDeathGradient(const Vector3F& gradient);

    [[nodiscard]] float fadeCoefficient() const;

    void setFadeCoefficient(float coeff);

    [[nodiscard]] bool debugDraw() const;

    void setDebugDraw(bool flag);

    explicit ParticleMaterial(wgpu::Device& device);

private:
    ParticleData _particleData;
    ShaderProperty _particleDataProp;
};

}  // namespace vox