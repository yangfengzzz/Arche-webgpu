//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/script.h"
#include "vox.toolkit/grid/grid_material.h"

namespace vox::grid {
class GridControl : public Script {
public:
    static ModelMeshPtr createGridPlane();

    explicit GridControl(Entity* entity);

    /**
     * Flip speed
     */
    float speed = 10.0;

    /**
     * Camera
     */
    Camera* camera{nullptr};

    /**
     * Grid Material.
     */
    std::shared_ptr<GridMaterial> getMaterial();

    /**
     * Is 2D Grid.
     */
    [[nodiscard]] bool is2DGrid() const;

    void setIs2DGrid(bool value);

    void onAwake() override;

    void onUpdate(float deltaTime) override;

private:
    std::shared_ptr<GridMaterial> _material{nullptr};
    float _progress = 0;
    bool _is2DGrid = false;
    bool _flipGrid = false;
};
}  // namespace vox::grid