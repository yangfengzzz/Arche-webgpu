//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/point.h"

namespace vox {
//! \brief N-D size type.
template <size_t N>
using Size = Point<size_t, N>;

}  // namespace vox

// #include "vox.math/size-inl.h"
