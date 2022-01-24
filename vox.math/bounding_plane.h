//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef plane_h
#define plane_h

#include "vector.h"

namespace vox {

//!
//! \brief      Class for ray.
//!
//! \tparam     T     The value type.
//! \tparam     N     The dimension.
//!
template<typename T, size_t N>
class BoundingPlane {
    static_assert(N != 2 && N != 3, "Not implemented.");
    static_assert(std::is_floating_point<T>::value,
                  "Plane only can be instantiated with floating point types");
};

}  // namespace vox

#endif /* plane_h */
