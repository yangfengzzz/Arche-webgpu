//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/base_material.h"
#include "vox.render/mesh/buffer_mesh.h"
#include "vox.render/script.h"

namespace vox {
// MARK: - SpriteDebugMaterial
class SpriteDebugMaterial : public BaseMaterial {
public:
    SpriteDebugMaterial(wgpu::Device &device, bool isSpotLight);
};

class SpriteDebug : public Script {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit SpriteDebug(Entity *entity);

    void onUpdate(float deltaTime) override;

private:
    Entity *_spotEntity{nullptr};
    std::shared_ptr<BufferMesh> _spotLightMesh{nullptr};

    Entity *_pointEntity{nullptr};
    std::shared_ptr<BufferMesh> _pointLightMesh{nullptr};
};
}  // namespace vox