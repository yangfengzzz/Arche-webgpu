//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/array.h"

namespace vox {

//!
//! \brief Generic N-D nearest array sampler class.
//!
//! \tparam T - The value type to sample.
//! \tparam R - The real number type.
//! \tparam N - Dimension.
//!
template <typename T, typename R, size_t N>
class NearestArraySampler final {
public:
    static_assert(N < 1 || N > 3, "Not implemented - N should be either 1, 2 or 3.");
};

//!
//! \brief Generic N-D linear array sampler class.
//!
//! \tparam T - The value type to sample.
//! \tparam R - The real number type.
//! \tparam N - Dimension.
//!
template <typename T, typename R, size_t N>
class LinearArraySampler final {
public:
    static_assert(N < 1 || N > 3, "Not implemented - N should be either 1, 2 or 3.");
};

//!
//! \brief Generic N-D cubic array sampler class.
//!
//! \tparam T - The value type to sample.
//! \tparam R - The real number type.
//! \tparam N - Dimension.
//!
template <typename T, typename R, size_t N>
class CubicArraySampler final {
public:
    static_assert(N < 1 || N > 3, "Not implemented - N should be either 1, 2 or 3.");
};

}  // namespace vox