//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/matrix_mxn.h"
#include "vox.math/matrix2x2.h"
#include "vox.math/matrix3x3.h"
#include "vox.math/matrix4x4.h"

namespace vox {

//!
//! \brief Singular value decomposition (SVD).
//!
//! This function decompose the input matrix \p a to \p u * \p w * \p v^T.
//!
//! \tparam T Real-value type.
//!
//! \param a The input matrix to decompose.
//! \param u Left-most output matrix.
//! \param w The vector of singular values.
//! \param v Right-most output matrix.
//!
template <typename T>
void svd(const MatrixMxN<T> &a, MatrixMxN<T> &u, VectorN<T> &w, MatrixMxN<T> &v);

//!
//! \brief Singular value decomposition (SVD).
//!
//! This function decompose the input matrix \p a to \p u * \p w * \p v^T.
//!
//! \tparam T Real-value type.
//!
//! \param a The input matrix to decompose.
//! \param u Left-most output matrix.
//! \param w The vector of singular values.
//! \param v Right-most output matrix.
//!
template <typename T, size_t M, size_t N>
void svd(const Matrix<T, M, N> &a, Matrix<T, M, N> &u, Vector<T, N> &w, Matrix<T, N, N> &v);

}  // namespace vox

#include "vox.geometry/svd-inl.h"