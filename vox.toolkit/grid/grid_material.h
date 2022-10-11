//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/base_material.h"

namespace vox::grid {
class GridMaterial : public BaseMaterial {
public:
    struct alignas(16) GridData {
        float farClip = 0.1;
        float nearClip = 100;
        float primaryScale = 10;
        float secondaryScale = 1;
        float gridIntensity = 0.2;
        float axisIntensity = 0.1;
        float flipProgress = 0.0;
    };

    /**
     * Near clip plane - the closest point to the camera when rendering occurs.
     */
    [[nodiscard]] float getNearClipPlane() const;

    void setNearClipPlane(float value);

    /**
     * Far clip plane - the furthest point to the camera when rendering occurs.
     */
    [[nodiscard]] float getFarClipPlane() const;

    void setFarClipPlane(float value);

    /**
     * Primary scale of grid size.
     */
    [[nodiscard]] float getPrimaryScale() const;

    void setPrimaryScale(float value);

    /**
     * Secondary scale of grid size.
     */
    [[nodiscard]] float getSecondaryScale() const;

    void setSecondaryScale(float value);

    /**
     * Grid color intensity.
     */
    [[nodiscard]] float getGridIntensity() const;

    void setGridIntensity(float value);

    /**
     * Axis color intensity.
     */
    [[nodiscard]] float getAxisIntensity() const;

    void setAxisIntensity(float value);

    /**
     * 2D-3D flip progress.
     */
    [[nodiscard]] float getFlipProgress() const;

    void setFlipProgress(float value);

    /**
     * Create a grid material instance.
     */
    explicit GridMaterial(wgpu::Device& device, const std::string& name = "");

private:
    GridData _grid;
    static const std::string _gridProperty;
};
}  // namespace vox::grid