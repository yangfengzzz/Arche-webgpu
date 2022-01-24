//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_SIZE_H_
#define INCLUDE_VOX_SIZE_H_

#include "point.h"

namespace vox {
//! \brief N-D size type.
template<size_t N> using Size = Point<size_t, N>;

}  // namespace vox

// #include "size-inl.h"

#endif  // INCLUDE_VOX_SIZE_H_

