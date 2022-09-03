//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.geometry/array_accessor2.h"
#include "vox.geometry/array_samplers.h"
#include "vox.math/point2.h"

namespace vox {

//!
//! \brief 2-D nearest array sampler class.
//!
//! This class provides nearest sampling interface for a given 2-D array.
//!
//! \tparam T - The value type to sample.
//! \tparam R - The real number type.
//!
template <typename T, typename R>
class NearestArraySampler<T, R, 2> final {
public:
    static_assert(std::is_floating_point<R>::value, "Samplers only can be instantiated with floating point types");

    //!
    //! \brief      Constructs a sampler using array accessor, spacing between
    //!     the elements, and the position of the first array element.
    //!
    //! \param[in]  accessor    The array accessor.
    //! \param[in]  gridSpacing The grid spacing.
    //! \param[in]  gridOrigin  The grid origin.
    //!
    explicit NearestArraySampler(const ConstArrayAccessor2<T> &accessor,
                                 const Vector2<R> &gridSpacing,
                                 const Point2<R> &gridOrigin);

    //! Copy constructor.
    NearestArraySampler(const NearestArraySampler &other);

    //! Returns sampled value at point \p pt.
    T operator()(const Point2<R> &x) const;

    //! Returns the nearest array index for point \p x.
    void getCoordinate(const Point2<R> &x, Point2UI *index) const;

    //! Returns a funtion object that wraps this instance.
    std::function<T(const Point2<R> &)> functor() const;

private:
    Vector2<R> _gridSpacing;
    Point2<R> _origin;
    ConstArrayAccessor2<T> _accessor;
};

//! Type alias for 2-D nearest array sampler.
template <typename T, typename R>
using NearestArraySampler2 = NearestArraySampler<T, R, 2>;

//!
//! \brief 2-D linear array sampler class.
//!
//! This class provides linear sampling interface for a given 2-D array.
//!
//! \tparam T - The value type to sample.
//! \tparam R - The real number type.
//!
template <typename T, typename R>
class LinearArraySampler<T, R, 2> final {
public:
    static_assert(std::is_floating_point<R>::value, "Samplers only can be instantiated with floating point types");

    //!
    //! \brief      Constructs a sampler using array accessor, spacing between
    //!     the elements, and the position of the first array element.
    //!
    //! \param[in]  accessor    The array accessor.
    //! \param[in]  gridSpacing The grid spacing.
    //! \param[in]  gridOrigin  The grid origin.
    //!
    explicit LinearArraySampler(const ConstArrayAccessor2<T> &accessor,
                                const Vector2<R> &gridSpacing,
                                const Point2<R> &gridOrigin);

    //! Copy constructor.
    LinearArraySampler(const LinearArraySampler &other);

    //! Returns sampled value at point \p pt.
    T operator()(const Point2<R> &x) const;

    //! Returns the indices of points and their sampling weight for given point.
    void getCoordinatesAndWeights(const Point2<R> &x,
                                  std::array<Point2UI, 4> *indices,
                                  std::array<R, 4> *weights) const;

    //! Returns the indices of points and their gradient of sampling weight for
    //! given point.
    void getCoordinatesAndGradientWeights(const Point2<R> &x,
                                          std::array<Point2UI, 4> *indices,
                                          std::array<Vector2<R>, 4> *weights) const;

    //! Returns a funtion object that wraps this instance.
    std::function<T(const Point2<R> &)> functor() const;

private:
    Vector2<R> _gridSpacing;
    Vector2<R> _invGridSpacing;
    Point2<R> _origin;
    ConstArrayAccessor2<T> _accessor;
};

//! Type alias for 2-D linear array sampler.
template <typename T, typename R>
using LinearArraySampler2 = LinearArraySampler<T, R, 2>;

//!
//! \brief 2-D cubic array sampler class.
//!
//! This class provides cubic sampling interface for a given 2-D array.
//!
//! \tparam T - The value type to sample.
//! \tparam R - The real number type.
//!
template <typename T, typename R>
class CubicArraySampler<T, R, 2> final {
public:
    static_assert(std::is_floating_point<R>::value, "Samplers only can be instantiated with floating point types");

    //!
    //! \brief      Constructs a sampler using array accessor, spacing between
    //!     the elements, and the position of the first array element.
    //!
    //! \param[in]  accessor    The array accessor.
    //! \param[in]  gridSpacing The grid spacing.
    //! \param[in]  gridOrigin  The grid origin.
    //!
    explicit CubicArraySampler(const ConstArrayAccessor2<T> &accessor,
                               const Vector2<R> &gridSpacing,
                               const Point2<R> &gridOrigin);

    //! Copy constructor.
    CubicArraySampler(const CubicArraySampler &other);

    //! Returns sampled value at point \p pt.
    T operator()(const Point2<R> &x) const;

    //! Returns a funtion object that wraps this instance.
    std::function<T(const Point2<R> &)> functor() const;

private:
    Vector2<R> _gridSpacing;
    Point2<R> _origin;
    ConstArrayAccessor2<T> _accessor;
};

//! Type alias for 2-D cubic array sampler.
template <typename T, typename R>
using CubicArraySampler2 = CubicArraySampler<T, R, 2>;

}  // namespace vox

#include "vox.geometry/array_samplers2-inl.h"