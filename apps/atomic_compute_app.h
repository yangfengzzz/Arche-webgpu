//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
#include "vox.render/rendering/compute_pass.h"

namespace vox {
class AtomicComputeApp : public ForwardApplication {
public:
    bool prepare(Platform& platform) override;

    void loadScene() override;

    void updateGPUTask(wgpu::CommandEncoder& commandEncoder) override;

private:
    std::shared_ptr<Material> _material{nullptr};
    std::unique_ptr<ComputePass> _pass{nullptr};
};
}  // namespace vox