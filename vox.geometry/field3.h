//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

namespace vox {

//! Abstract base class for 3-D fields.
class Field3 {
public:
    Field3();

    virtual ~Field3();
};

typedef std::shared_ptr<Field3> Field3Ptr;

}  // namespace vox