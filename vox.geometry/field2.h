//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

namespace vox {

//! Abstract base class for 2-D fields.
class Field2 {
public:
    Field2();

    virtual ~Field2();
};

typedef std::shared_ptr<Field2> Field2Ptr;

}  // namespace vox