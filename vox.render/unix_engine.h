//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "engine.h"

namespace vox {
enum UnixType {
    Mac,
    Linux
};

class UnixEngine : public Engine {
public:
    UnixEngine(const UnixType &type, int argc, char **argv);
    
    virtual ~UnixEngine() = default;
    
protected:
    virtual void createWindow(const Window::Properties &properties) override;
    
private:
    UnixType _type;
};
}        // namespace vox
