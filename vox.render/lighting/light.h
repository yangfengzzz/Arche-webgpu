//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/matrix4x4.h"
#include "vox.render/component.h"

namespace vox {
/**
 * Light base class.
 */
class Light : public Component {
public:
    /**
     * Each type of light source is at most 10, beyond which it will not take effect.
     * */
    static constexpr uint32_t MAX_LIGHT = 10;

    explicit Light(Entity *entity);

    /**
     * View matrix.
     */
    Matrix4x4F viewMatrix();

    /**
     * Inverse view matrix.
     */
    Matrix4x4F inverseViewMatrix();

public:
    [[nodiscard]] bool enableShadow() const;

    void setEnableShadow(bool enabled);

    /**
     * Shadow bias.
     */
    [[nodiscard]] float shadowBias() const;

    void setShadowBias(float value);

    /**
     * Shadow intensity, the larger the value, the clearer and darker the shadow.
     */
    [[nodiscard]] float shadowIntensity() const;

    void setShadowIntensity(float value);

    /**
     * Pixel range used for shadow PCF interpolation.
     */
    [[nodiscard]] float shadowRadius() const;

    void setShadowRadius(float value);

    virtual Matrix4x4F shadowProjectionMatrix() = 0;

private:
    bool _enableShadow = false;
    float _shadowBias = 0.005;
    float _shadowIntensity = 0.2;
    float _shadowRadius = 1;
};

}  // namespace vox
