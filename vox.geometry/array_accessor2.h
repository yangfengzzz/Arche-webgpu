//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.geometry/array_accessor.h"
#include "vox.math/point2.h"
#include "vox.math/size2.h"

namespace vox {

//!
//! \brief 2-D array accessor class.
//!
//! This class represents 2-D array accessor. Array accessor provides array-like
//! data read/write functions, but does not handle memory management. Thus, it
//! is more like a random access iterator, but with multi-dimension support.
//! Similar to Array<T, 2>, this class interprets a linear array as a 2-D array
//! using i-major indexing.
//!
//! \see Array<T, 2>
//!
//! \tparam T - Array value type.
//!
template <typename T>
class ArrayAccessor<T, 2> final {
public:
    //! Constructs empty 2-D array accessor.
    ArrayAccessor();

    //! Constructs an array accessor that wraps given array.
    //! \param size Size of the 2-D array.
    //! \param data Raw array pointer.
    ArrayAccessor(const Size2 &size, T *data);

    //! Constructs an array accessor that wraps given array.
    //! \param width Width of the 2-D array.
    //! \param height Height of the 2-D array.
    //! \param data Raw array pointer.
    ArrayAccessor(size_t width, size_t height, T *data);

    //! Copy constructor.
    ArrayAccessor(const ArrayAccessor &other);

    //! Replaces the content with given \p other array accessor.
    void set(const ArrayAccessor &other);

    //! Resets the array.
    void reset(const Size2 &size, T *data);

    //! Resets the array.
    void reset(size_t width, size_t height, T *data);

    //! Returns the reference to the i-th element.
    T &at(size_t i);

    //! Returns the const reference to the i-th element.
    const T &at(size_t i) const;

    //! Returns the reference to the element at (pt.x, pt.y).
    T &at(const Point2UI &pt);

    //! Returns the const reference to the element at (pt.x, pt.y).
    const T &at(const Point2UI &pt) const;

    //! Returns the reference to the element at (i, j).
    T &at(size_t i, size_t j);

    //! Returns the const reference to the element at (i, j).
    const T &at(size_t i, size_t j) const;

    //! Returns the begin iterator of the array.
    T *begin() const;

    //! Returns the end iterator of the array.
    T *end() const;

    //! Returns the begin iterator of the array.
    T *begin();

    //! Returns the end iterator of the array.
    T *end();

    //! Returns the size of the array.
    [[nodiscard]] Size2 size() const;

    //! Returns the width of the array.
    [[nodiscard]] size_t width() const;

    //! Returns the height of the array.
    [[nodiscard]] size_t height() const;

    //! Returns the raw pointer to the array data.
    T *data() const;

    //! Swaps the content of with \p other array accessor.
    void swap(ArrayAccessor &other);

    //!
    //! \brief Iterates the array and invoke given \p func for each index.
    //!
    //! This function iterates the array elements and invoke the callback
    //! function \p func. The callback function takes array's element as its
    //! input. The order of execution will be the same as the nested for-loop
    //! below:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ArrayAccessor<int, 2> acc(2, 3, data);
    //! for (size_t j = 0; j < acc.height(); ++j) {
    //!     for (size_t i = 0; i < acc.width(); ++i) {
    //!         func(acc(i, j));
    //!     }
    //! }
    //! \endcode
    //!
    //! Below is the sample usage:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ArrayAccessor<int, 2> acc(2, 3, data);
    //! acc.forEach([](int elem) {
    //!     printf("%d\n", elem);
    //! });
    //! \endcode
    //!
    template <typename Callback>
    void forEach(Callback func) const;

    //!
    //! \brief Iterates the array and invoke given \p func for each index.
    //!
    //! This function iterates the array elements and invoke the callback
    //! function \p func. The callback function takes two parameters which are
    //! the (i, j) indices of the array. The order of execution will be the same
    //! as the nested for-loop below:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ArrayAccessor<int, 2> acc(2, 3, data);
    //! for (size_t j = 0; j < acc.height(); ++j) {
    //!     for (size_t i = 0; i < acc.width(); ++i) {
    //!         func(i, j);
    //!     }
    //! }
    //! \endcode
    //!
    //! Below is the sample usage:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ArrayAccessor<int, 2> acc(2, 3, data);
    //! acc.forEachIndex([&](size_t i, size_t j) {
    //!     acc(i, j) = 4.f * i + 7.f * j + 1.5f;
    //! });
    //! \endcode
    //!
    template <typename Callback>
    void forEachIndex(Callback func) const;

    //!
    //! \brief Iterates the array and invoke given \p func for each index in
    //!     parallel.
    //!
    //! This function iterates the array elements and invoke the callback
    //! function \p func. The callback function takes array's element as its
    //! input. The order of execution will be non-deterministic since it runs in
    //! parallel. Below is the sample usage:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ArrayAccessor<int, 2> acc(2, 3, data);
    //! acc.parallelForEach([](int& elem) {
    //!     elem *= 2;
    //! });
    //! \endcode
    //!
    //! The parameter type of the callback function doesn't have to be T&, but
    //! const T& or T can be used as well.
    //!
    template <typename Callback>
    void parallelForEach(Callback func);

    //!
    //! \brief Iterates the array and invoke given \p func for each index in
    //!     parallel using multi-threading.
    //!
    //! This function iterates the array elements and invoke the callback
    //! function \p func in parallel using multi-threading. The callback
    //! function takes two parameters which are the (i, j) indices of the array.
    //! The order of execution will be non-deterministic since it runs in
    //! parallel. Below is the sample usage:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ArrayAccessor<int, 2> acc(2, 3, data);
    //! acc.parallelForEachIndex([&](size_t i, size_t j) {
    //!     acc(i, j) *= 2;
    //! });
    //! \endcode
    //!
    template <typename Callback>
    void parallelForEachIndex(Callback func) const;

    //! Returns the linear index of the given 2-D coordinate (pt.x, pt.y).
    [[nodiscard]] size_t index(const Point2UI &pt) const;

    //! Returns the linear index of the given 2-D coordinate (i, j).
    [[nodiscard]] size_t index(size_t i, size_t j) const;

    //! Returns the reference to the i-th element.
    T &operator[](size_t i);

    //! Returns the const reference to the i-th element.
    const T &operator[](size_t i) const;

    //! Returns the reference to the element at (pt.x, pt.y).
    T &operator()(const Point2UI &pt);

    //! Returns the const reference to the element at (pt.x, pt.y).
    const T &operator()(const Point2UI &pt) const;

    //! Returns the reference to the element at (i, j).
    T &operator()(size_t i, size_t j);

    //! Returns the const reference to the element at (i, j).
    const T &operator()(size_t i, size_t j) const;

    //! Copies given array accessor \p other.
    ArrayAccessor &operator=(const ArrayAccessor &other);

    //! Casts type to ConstArrayAccessor.
    operator ConstArrayAccessor<T, 2>() const;

private:
    Size2 _size;
    T *_data;
};

//! Type alias for 2-D array accessor.
template <typename T>
using ArrayAccessor2 = ArrayAccessor<T, 2>;

//!
//! \brief 2-D read-only array accessor class.
//!
//! This class represents 2-D read-only array accessor. Array accessor provides
//! array-like data read/write functions, but does not handle memory management.
//! Thus, it is more like a random access iterator, but with multi-dimension
//! support. Similar to Array2<T, 2>, this class interprets a linear array as a
//! 2-D array using i-major indexing.
//!
//! \see Array2<T, 2>
//!
template <typename T>
class ConstArrayAccessor<T, 2> {
public:
    //! Constructs empty 2-D read-only array accessor.
    ConstArrayAccessor();

    //! Constructs a read-only array accessor that wraps given array.
    //! \param size Size of the 2-D array.
    //! \param data Raw array pointer.
    ConstArrayAccessor(const Size2 &size, const T *data);

    //! Constructs an array accessor that wraps given array.
    //! \param width Width of the 2-D array.
    //! \param height Height of the 2-D array.
    //! \param data Raw array pointer.
    ConstArrayAccessor(size_t width, size_t height, const T *data);

    //! Constructs a read-only array accessor from read/write accessor.
    explicit ConstArrayAccessor(const ArrayAccessor<T, 2> &other);

    //! Copy constructor.
    ConstArrayAccessor(const ConstArrayAccessor &other);

    //! Returns the reference to the i-th element.
    const T &at(size_t i) const;

    //! Returns the const reference to the element at (pt.x, pt.y).
    const T &at(const Point2UI &pt) const;

    //! Returns the const reference to the element at (i, j).
    const T &at(size_t i, size_t j) const;

    //! Returns the begin iterator of the array.
    const T *begin() const;

    //! Returns the end iterator of the array.
    const T *end() const;

    //! Returns the size of the array.
    [[nodiscard]] Size2 size() const;

    //! Returns the width of the array.
    [[nodiscard]] size_t width() const;

    //! Returns the height of the array.
    [[nodiscard]] size_t height() const;

    //! Returns the raw pointer to the array data.
    const T *data() const;

    //!
    //! \brief Iterates the array and invoke given \p func for each index.
    //!
    //! This function iterates the array elements and invoke the callback
    //! function \p func. The callback function takes array's element as its
    //! input. The order of execution will be the same as the nested for-loop
    //! below:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ConstArrayAccessor<int, 2> acc(2, 3, data);
    //! for (size_t j = 0; j < acc.height(); ++j) {
    //!     for (size_t i = 0; i < acc.width(); ++i) {
    //!         func(acc(i, j));
    //!     }
    //! }
    //! \endcode
    //!
    //! Below is the sample usage:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ConstArrayAccessor<int, 2> acc(2, 3, data);
    //! acc.forEach([](int elem) {
    //!     printf("%d\n", elem);
    //! });
    //! \endcode
    //!
    template <typename Callback>
    void forEach(Callback func) const;

    //!
    //! \brief Iterates the array and invoke given \p func for each index.
    //!
    //! This function iterates the array elements and invoke the callback
    //! function \p func. The callback function takes two parameters which are
    //! the (i, j) indices of the array. The order of execution will be the same
    //! as the nested for-loop below:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ConstArrayAccessor<int, 2> acc(2, 3, data);
    //! for (size_t j = 0; j < acc.height(); ++j) {
    //!     for (size_t i = 0; i < acc.width(); ++i) {
    //!         func(i, j);
    //!     }
    //! }
    //! \endcode
    //!
    //! Below is the sample usage:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ConstArrayAccessor<int, 2> acc(2, 3, data);
    //! acc.forEachIndex([&](size_t i, size_t j) {
    //!     acc(i, j) = 4.f * i + 7.f * j + 1.5f;
    //! });
    //! \endcode
    //!
    template <typename Callback>
    void forEachIndex(Callback func) const;

    //!
    //! \brief Iterates the array and invoke given \p func for each index in
    //!     parallel using multi-threading.
    //!
    //! This function iterates the array elements and invoke the callback
    //! function \p func in parallel using multi-threading. The callback
    //! function takes two parameters which are the (i, j) indices of the array.
    //! The order of execution will be non-deterministic since it runs in
    //! parallel. Below is the sample usage:
    //!
    //! \code{.cpp}
    //! int data = {1, 2, 3, 4, 5, 6};
    //! ConstArrayAccessor<int, 2> acc(2, 3, data);
    //! acc.parallelForEachIndex([&](size_t i, size_t j) {
    //!     acc(i, j) *= 2;
    //! });
    //! \endcode
    //!
    template <typename Callback>
    void parallelForEachIndex(Callback func) const;

    //! Returns the linear index of the given 2-D coordinate (pt.x, pt.y).
    [[nodiscard]] size_t index(const Point2UI &pt) const;

    //! Returns the linear index of the given 2-D coordinate (i, j).
    [[nodiscard]] size_t index(size_t i, size_t j) const;

    //! Returns the const reference to the i-th element.
    const T &operator[](size_t i) const;

    //! Returns the const reference to the element at (pt.x, pt.y).
    const T &operator()(const Point2UI &pt) const;

    //! Returns the const reference to the element at (i, j).
    const T &operator()(size_t i, size_t j) const;

private:
    Size2 _size;
    const T *_data;
};

//! Type alias for 2-D const array accessor.
template <typename T>
using ConstArrayAccessor2 = ConstArrayAccessor<T, 2>;

}  // namespace vox

#include "vox.geometry/array_accessor2-inl.h"
