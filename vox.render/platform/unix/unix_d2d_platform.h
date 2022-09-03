//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/platform.h"

namespace vox {
class UnixD2DPlatform : public Platform {
public:
    UnixD2DPlatform(int argc, char **argv);

    virtual ~UnixD2DPlatform() = default;

    virtual const char *GetSurfaceExtension() override;

protected:
    virtual void CreateWindow(const Window::Properties &properties) override;
};

}  // namespace vox
