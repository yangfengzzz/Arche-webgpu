//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/matrix4x4.h"
#include "vox.math/color.h"
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

    /** Light Color */
    Color color = Color(1, 1, 1, 1);
    /** Light Intensity */
    float intensity = 1;

    /** whether enable shadow */
    bool enableShadow = false;
    /** Shadow bias.*/
    float shadowBias = 1;
    /** Shadow mapping normal-based bias. */
    float shadowNormalBias = 0;
    /** Near plane value to use for shadow frustums. */
    float shadowNearPlane = 0.1;
    /** Shadow intensity, the larger the value, the clearer and darker the shadow. */
    float shadowStrength = 1.0;

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
    virtual Matrix4x4F shadowProjectionMatrix() { return {}; }
};

}  // namespace vox
