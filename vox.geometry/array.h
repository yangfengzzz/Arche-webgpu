//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/size.h"

namespace vox {

//!
//! \brief Generic N-dimensional array class interface.
//!
//! This class provides generic template class for N-dimensional array where N
//! must be either 1, 2 or 3. This particular class exists to provide generic
//! interface for 1, 2 or 3 dimensional arrays using template specialization
//! only, but it cannot create any instance by itself.
//!
//! \tparam T - Real number type.
//! \tparam N - Dimension.
//!
template <typename T, size_t N>
class Array final {
public:
    static_assert(N < 1 || N > 3, "Not implemented - N should be either 1, 2 or 3.");
};

}  // namespace vox