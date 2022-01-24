//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_FUNCTORS_H_
#define INCLUDE_VOX_FUNCTORS_H_

#include <functional>

namespace vox {

//! Type casting operator.
template<typename T, typename U>
struct TypeCast {
    constexpr U operator()(const T &a) const;
};

//! Reverse minus operator.
template<typename T>
struct RMinus {
    constexpr T operator()(const T &a, const T &b) const;
};

//! Reverse divides operator.
template<typename T>
struct RDivides {
    constexpr T operator()(const T &a, const T &b) const;
};
}

#include "functors-inl.h"

#endif  // INCLUDE_VOX_FUNCTORS_H_
