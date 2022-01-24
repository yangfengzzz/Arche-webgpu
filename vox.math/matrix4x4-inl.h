//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_DETAIL_MATRIX4X4_INL_H_
#define INCLUDE_VOX_DETAIL_MATRIX4X4_INL_H_

#include "math_utils.h"
#include <algorithm>
#include <cstring>
#include <utility>

namespace vox {

// MARK: - CTOR/DTOR
template<typename T>
Matrix<T, 4, 4>::Matrix() {
    set(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

template<typename T>
Matrix<T, 4, 4>::Matrix(T s) {
    set(s);
}

template<typename T>
Matrix<T, 4, 4>::Matrix(T m00, T m10, T m20,
                        T m01, T m11, T m21,
                        T m02, T m12, T m22) {
    set(m00, m10, m20,
        m01, m11, m21,
        m02, m12, m22);
}

template<typename T>
Matrix<T, 4, 4>::Matrix(T m00, T m10, T m20, T m30,
                        T m01, T m11, T m21, T m31,
                        T m02, T m12, T m22, T m32,
                        T m03, T m13, T m23, T m33) {
    set(m00, m10, m20, m30,
        m01, m11, m21, m31,
        m02, m12, m22, m32,
        m03, m13, m23, m33);
}

template<typename T>
template<typename U>
Matrix<T, 4, 4>::Matrix(const std::initializer_list<std::initializer_list<U>> &lst) {
    set(lst);
}

template<typename T>
Matrix<T, 4, 4>::Matrix(const Matrix<T, 3, 3> &m33) {
    set(m33);
}

template<typename T>
Matrix<T, 4, 4>::Matrix(const Matrix &m) {
    set(m);
}

template<typename T>
Matrix<T, 4, 4>::Matrix(const T *arr) {
    set(arr);
}


// MARK: - Basic setters
template<typename T>
void Matrix<T, 4, 4>::set(T s) {
    _elements[0] = _elements[4] = _elements[8] = _elements[12] =
    _elements[1] = _elements[5] = _elements[9] = _elements[13] =
    _elements[2] = _elements[6] = _elements[10] = _elements[14] =
    _elements[3] = _elements[7] = _elements[11] = _elements[15] = s;
}

template<typename T>
void Matrix<T, 4, 4>::set(T m00, T m10, T m20,
                          T m01, T m11, T m21,
                          T m02, T m12, T m22) {
    _elements[0] = m00;
    _elements[1] = m10;
    _elements[2] = m20;
    _elements[3] = 0;
    _elements[4] = m01;
    _elements[5] = m11;
    _elements[6] = m21;
    _elements[7] = 0;
    _elements[8] = m02;
    _elements[9] = m12;
    _elements[10] = m22;
    _elements[11] = 0;
    _elements[12] = 0;
    _elements[13] = 0;
    _elements[14] = 0;
    _elements[15] = 1;
}

template<typename T>
void Matrix<T, 4, 4>::set(T m00, T m10, T m20, T m30,
                          T m01, T m11, T m21, T m31,
                          T m02, T m12, T m22, T m32,
                          T m03, T m13, T m23, T m33) {
    _elements[0] = m00;
    _elements[1] = m10;
    _elements[2] = m20;
    _elements[3] = m30;
    _elements[4] = m01;
    _elements[5] = m11;
    _elements[6] = m21;
    _elements[7] = m31;
    _elements[8] = m02;
    _elements[9] = m12;
    _elements[10] = m22;
    _elements[11] = m32;
    _elements[12] = m03;
    _elements[13] = m13;
    _elements[14] = m23;
    _elements[15] = m33;
}

template<typename T>
template<typename U>
void Matrix<T, 4, 4>::set(const std::initializer_list<std::initializer_list<U>> &lst) {
    size_t width = lst.size();
    size_t height = (width > 0) ? lst.begin()->size() : 0;
    VOX_ASSERT(width == 4);
    VOX_ASSERT(height == 4);
    
    auto colIter = lst.begin();
    for (size_t i = 0; i < width; ++i) {
        VOX_ASSERT(height == colIter->size());
        auto rowIter = colIter->begin();
        for (size_t j = 0; j < height; ++j) {
            (*this)(j, i) = static_cast<T>(*rowIter);
            ++rowIter;
        }
        ++colIter;
    }
}

template<typename T>
void Matrix<T, 4, 4>::set(const Matrix<T, 3, 3> &m33) {
    set(m33[0], m33[1], m33[2], 0,
        m33[3], m33[4], m33[5], 0,
        m33[6], m33[7], m33[8], 0,
        0, 0, 0, 1);
}

template<typename T>
void Matrix<T, 4, 4>::set(const Matrix &m) {
    _elements = m._elements;
}

template<typename T>
void Matrix<T, 4, 4>::set(const T *arr) {
    memcpy(_elements.data(), arr, sizeof(T) * 16);
}

template<typename T>
void Matrix<T, 4, 4>::setDiagonal(T s) {
    _elements[0] = _elements[5] = _elements[10] = _elements[15] = s;
}

template<typename T>
void Matrix<T, 4, 4>::setOffDiagonal(T s) {
    _elements[1] = _elements[2] = _elements[3] = _elements[4]
    = _elements[6] = _elements[7] = _elements[8] = _elements[9]
    = _elements[11] = _elements[12] = _elements[13] = _elements[14] = s;
}

template<typename T>
void Matrix<T, 4, 4>::setRow(size_t i, const Vector<T, 4> &row) {
    _elements[i] = row.x;
    _elements[i + 4] = row.y;
    _elements[i + 8] = row.z;
    _elements[i + 12] = row.w;
}

template<typename T>
void Matrix<T, 4, 4>::setColumn(size_t j, const Vector<T, 4> &col) {
    _elements[4 * j] = col.x;
    _elements[4 * j + 1] = col.y;
    _elements[4 * j + 2] = col.z;
    _elements[4 * j + 3] = col.w;
}


// MARK: - Basic getters
template<typename T>
bool Matrix<T, 4, 4>::isSimilar(const Matrix &m, double tol) const {
    return
    std::fabs(_elements[0] - m._elements[0]) < tol
    && std::fabs(_elements[1] - m._elements[1]) < tol
    && std::fabs(_elements[2] - m._elements[2]) < tol
    && std::fabs(_elements[3] - m._elements[3]) < tol
    && std::fabs(_elements[4] - m._elements[4]) < tol
    && std::fabs(_elements[5] - m._elements[5]) < tol
    && std::fabs(_elements[6] - m._elements[6]) < tol
    && std::fabs(_elements[7] - m._elements[7]) < tol
    && std::fabs(_elements[8] - m._elements[8]) < tol
    && std::fabs(_elements[9] - m._elements[9]) < tol
    && std::fabs(_elements[10] - m._elements[10]) < tol
    && std::fabs(_elements[11] - m._elements[11]) < tol
    && std::fabs(_elements[12] - m._elements[12]) < tol
    && std::fabs(_elements[13] - m._elements[13]) < tol
    && std::fabs(_elements[14] - m._elements[14]) < tol
    && std::fabs(_elements[15] - m._elements[15]) < tol;
}

template<typename T>
bool Matrix<T, 4, 4>::isSquare() const {
    return true;
}

template<typename T>
size_t Matrix<T, 4, 4>::rows() const {
    return 4;
}

template<typename T>
size_t Matrix<T, 4, 4>::cols() const {
    return 4;
}

template<typename T>
T *Matrix<T, 4, 4>::data() {
    return _elements.data();
}

template<typename T>
const T *Matrix<T, 4, 4>::data() const {
    return _elements.data();
}

template<typename T>
Matrix<T, 3, 3> Matrix<T, 4, 4>::matrix3() const {
    return Matrix<T, 3, 3>(_elements[0], _elements[1], _elements[2],
                           _elements[4], _elements[5], _elements[6],
                           _elements[8], _elements[9], _elements[10]);
}

template<typename T>
Matrix<T, 3, 3> Matrix<T, 4, 4>::normalMatrix() const {
    T a11 = _elements[0],
    a12 = _elements[1],
    a13 = _elements[2],
    a14 = _elements[3];
    T a21 = _elements[4],
    a22 = _elements[5],
    a23 = _elements[6],
    a24 = _elements[7];
    T a31 = _elements[8],
    a32 = _elements[9],
    a33 = _elements[10],
    a34 = _elements[11];
    T a41 = _elements[12],
    a42 = _elements[13],
    a43 = _elements[14],
    a44 = _elements[15];
    
    T b00 = a11 * a22 - a12 * a21;
    T b01 = a11 * a23 - a13 * a21;
    T b02 = a11 * a24 - a14 * a21;
    T b03 = a12 * a23 - a13 * a22;
    T b04 = a12 * a24 - a14 * a22;
    T b05 = a13 * a24 - a14 * a23;
    T b06 = a31 * a42 - a32 * a41;
    T b07 = a31 * a43 - a33 * a41;
    T b08 = a31 * a44 - a34 * a41;
    T b09 = a32 * a43 - a33 * a42;
    T b10 = a32 * a44 - a34 * a42;
    T b11 = a33 * a44 - a34 * a43;
    
    T det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    if (!det) {
        return Matrix<T, 3, 3>();
    }
    det = 1.0 / det;
    
    return Matrix<T, 3, 3>((a22 * b11 - a23 * b10 + a24 * b09) * det,
                           (a23 * b08 - a21 * b11 - a24 * b07) * det,
                           (a21 * b10 - a22 * b08 + a24 * b06) * det,
                           
                           (a13 * b10 - a12 * b11 - a14 * b09) * det,
                           (a11 * b11 - a13 * b08 + a14 * b07) * det,
                           (a12 * b08 - a11 * b10 - a14 * b06) * det,
                           
                           (a42 * b05 - a43 * b04 + a44 * b03) * det,
                           (a43 * b02 - a41 * b05 - a44 * b01) * det,
                           (a41 * b04 - a42 * b02 + a44 * b00) * det);
}

// MARK: - Binary operator methods - new instance = this instance (+) input
template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::add(T s) const {
    return Matrix(_elements[0] + s,
                  _elements[1] + s,
                  _elements[2] + s,
                  _elements[3] + s,
                  _elements[4] + s,
                  _elements[5] + s,
                  _elements[6] + s,
                  _elements[7] + s,
                  _elements[8] + s,
                  _elements[9] + s,
                  _elements[10] + s,
                  _elements[11] + s,
                  _elements[12] + s,
                  _elements[13] + s,
                  _elements[14] + s,
                  _elements[15] + s);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::add(const Matrix &m) const {
    return Matrix(_elements[0] + m._elements[0],
                  _elements[1] + m._elements[1],
                  _elements[2] + m._elements[2],
                  _elements[3] + m._elements[3],
                  _elements[4] + m._elements[4],
                  _elements[5] + m._elements[5],
                  _elements[6] + m._elements[6],
                  _elements[7] + m._elements[7],
                  _elements[8] + m._elements[8],
                  _elements[9] + m._elements[9],
                  _elements[10] + m._elements[10],
                  _elements[11] + m._elements[11],
                  _elements[12] + m._elements[12],
                  _elements[13] + m._elements[13],
                  _elements[14] + m._elements[14],
                  _elements[15] + m._elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::sub(T s) const {
    return Matrix(_elements[0] - s, _elements[1] - s, _elements[2] - s, _elements[3] - s,
                  _elements[4] - s, _elements[5] - s, _elements[6] - s, _elements[7] - s,
                  _elements[8] - s, _elements[9] - s, _elements[10] - s, _elements[11] - s,
                  _elements[12] - s, _elements[13] - s, _elements[14] - s, _elements[15] - s);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::sub(const Matrix &m) const {
    return Matrix(_elements[0] - m._elements[0],
                  _elements[1] - m._elements[1],
                  _elements[2] - m._elements[2],
                  _elements[3] - m._elements[3],
                  _elements[4] - m._elements[4],
                  _elements[5] - m._elements[5],
                  _elements[6] - m._elements[6],
                  _elements[7] - m._elements[7],
                  _elements[8] - m._elements[8],
                  _elements[9] - m._elements[9],
                  _elements[10] - m._elements[10],
                  _elements[11] - m._elements[11],
                  _elements[12] - m._elements[12],
                  _elements[13] - m._elements[13],
                  _elements[14] - m._elements[14],
                  _elements[15] - m._elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::mul(T s) const {
    return Matrix(_elements[0] * s, _elements[1] * s, _elements[2] * s, _elements[3] * s,
                  _elements[4] * s, _elements[5] * s, _elements[6] * s, _elements[7] * s,
                  _elements[8] * s, _elements[9] * s, _elements[10] * s, _elements[11] * s,
                  _elements[12] * s, _elements[13] * s, _elements[14] * s, _elements[15] * s);
}

template<typename T>
Vector<T, 4> Matrix<T, 4, 4>::mul(const Vector<T, 4> &v) const {
    return Vector<T, 4>(_elements[0] * v.x + _elements[4] * v.y + _elements[8] * v.z + _elements[12] * v.w,
                        _elements[1] * v.x + _elements[5] * v.y + _elements[9] * v.z + _elements[13] * v.w,
                        _elements[2] * v.x + _elements[6] * v.y + _elements[10] * v.z + _elements[14] * v.w,
                        _elements[3] * v.x + _elements[7] * v.y + _elements[11] * v.z + _elements[15] * v.w);
}

template<typename T>
Vector<T, 3> Matrix<T, 4, 4>::mul(const Vector<T, 3> &v) const {
    return Vector<T, 3>(_elements[0] * v.x + _elements[4] * v.y + _elements[8] * v.z,
                        _elements[1] * v.x + _elements[5] * v.y + _elements[9] * v.z,
                        _elements[2] * v.x + _elements[6] * v.y + _elements[10] * v.z);
}

template<typename T>
Point3 <T> Matrix<T, 4, 4>::mul(const Point3 <T> &v) const {
    T w = _elements[3] * v.x + _elements[7] * v.y + _elements[11] * v.z + _elements[15];
    w = (T) 1 / w;
    
    return Point3<T>((_elements[0] * v.x + _elements[4] * v.y + _elements[8] * v.z + _elements[12]) * w,
                     (_elements[1] * v.x + _elements[5] * v.y + _elements[9] * v.z + _elements[13]) * w,
                     (_elements[2] * v.x + _elements[6] * v.y + _elements[10] * v.z + _elements[14]) * w);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::mul(const Matrix &m) const {
    return Matrix(_elements[0] * m._elements[0] + _elements[4] * m._elements[1] + _elements[8] * m._elements[2] + _elements[12] * m._elements[3],
                  _elements[1] * m._elements[0] + _elements[5] * m._elements[1] + _elements[9] * m._elements[2] + _elements[13] * m._elements[3],
                  _elements[2] * m._elements[0] + _elements[6] * m._elements[1] + _elements[10] * m._elements[2] + _elements[14] * m._elements[3],
                  _elements[3] * m._elements[0] + _elements[7] * m._elements[1] + _elements[11] * m._elements[2] + _elements[15] * m._elements[3],
                  
                  _elements[0] * m._elements[4] + _elements[4] * m._elements[5] + _elements[8] * m._elements[6] + _elements[12] * m._elements[7],
                  _elements[1] * m._elements[4] + _elements[5] * m._elements[5] + _elements[9] * m._elements[6] + _elements[13] * m._elements[7],
                  _elements[2] * m._elements[4] + _elements[6] * m._elements[5] + _elements[10] * m._elements[6] + _elements[14] * m._elements[7],
                  _elements[3] * m._elements[4] + _elements[7] * m._elements[5] + _elements[11] * m._elements[6] + _elements[15] * m._elements[7],
                  
                  _elements[0] * m._elements[8] + _elements[4] * m._elements[9] + _elements[8] * m._elements[10] + _elements[12] * m._elements[11],
                  _elements[1] * m._elements[8] + _elements[5] * m._elements[9] + _elements[9] * m._elements[10] + _elements[13] * m._elements[11],
                  _elements[2] * m._elements[8] + _elements[6] * m._elements[9] + _elements[10] * m._elements[10] + _elements[14] * m._elements[11],
                  _elements[3] * m._elements[8] + _elements[7] * m._elements[9] + _elements[11] * m._elements[10] + _elements[15] * m._elements[11],
                  
                  _elements[0] * m._elements[12] + _elements[4] * m._elements[13] + _elements[8] * m._elements[14] + _elements[12] * m._elements[15],
                  _elements[1] * m._elements[12] + _elements[5] * m._elements[13] + _elements[9] * m._elements[14] + _elements[13] * m._elements[15],
                  _elements[2] * m._elements[12] + _elements[6] * m._elements[13] + _elements[10] * m._elements[14] + _elements[14] * m._elements[15],
                  _elements[3] * m._elements[12] + _elements[7] * m._elements[13] + _elements[11] * m._elements[14] + _elements[15] * m._elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::div(T s) const {
    return Matrix(_elements[0] / s, _elements[1] / s, _elements[2] / s, _elements[3] / s,
                  _elements[4] / s, _elements[5] / s, _elements[6] / s, _elements[7] / s,
                  _elements[8] / s, _elements[9] / s, _elements[10] / s, _elements[11] / s,
                  _elements[12] / s, _elements[13] / s, _elements[14] / s, _elements[15] / s);
}


// MARK: - Binary operator methods - new instance = input (+) this instance
template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::radd(T s) const {
    return Matrix(s + _elements[0], s + _elements[1], s + _elements[2], s + _elements[3],
                  s + _elements[4], s + _elements[5], s + _elements[6], s + _elements[7],
                  s + _elements[8], s + _elements[9], s + _elements[10], s + _elements[11],
                  s + _elements[12], s + _elements[13], s + _elements[14], s + _elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::radd(const Matrix &m) const {
    return Matrix(m._elements[0] + _elements[0], m._elements[1] + _elements[1], m._elements[2] + _elements[2], m._elements[3] + _elements[3],
                  m._elements[4] + _elements[4], m._elements[5] + _elements[5], m._elements[6] + _elements[6], m._elements[7] + _elements[7],
                  m._elements[8] + _elements[8], m._elements[9] + _elements[9], m._elements[10] + _elements[10], m._elements[11] + _elements[11],
                  m._elements[12] + _elements[12], m._elements[13] + _elements[13], m._elements[14] + _elements[14], m._elements[15] + _elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rsub(T s) const {
    return Matrix(s - _elements[0], s - _elements[1], s - _elements[2], s - _elements[3],
                  s - _elements[4], s - _elements[5], s - _elements[6], s - _elements[7],
                  s - _elements[8], s - _elements[9], s - _elements[10], s - _elements[11],
                  s - _elements[12], s - _elements[13], s - _elements[14], s - _elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rsub(const Matrix &m) const {
    return Matrix(m._elements[0] - _elements[0], m._elements[1] - _elements[1], m._elements[2] - _elements[2], m._elements[3] - _elements[3],
                  m._elements[4] - _elements[4], m._elements[5] - _elements[5], m._elements[6] - _elements[6], m._elements[7] - _elements[7],
                  m._elements[8] - _elements[8], m._elements[9] - _elements[9], m._elements[10] - _elements[10], m._elements[11] - _elements[11],
                  m._elements[12] - _elements[12], m._elements[13] - _elements[13], m._elements[14] - _elements[14], m._elements[15] - _elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rmul(T s) const {
    return Matrix(s * _elements[0], s * _elements[1], s * _elements[2], s * _elements[3],
                  s * _elements[4], s * _elements[5], s * _elements[6], s * _elements[7],
                  s * _elements[8], s * _elements[9], s * _elements[10], s * _elements[11],
                  s * _elements[12], s * _elements[13], s * _elements[14], s * _elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rmul(const Matrix &m) const {
    return m.mul(*this);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rdiv(T s) const {
    return Matrix(s / _elements[0], s / _elements[1], s / _elements[2], s / _elements[3],
                  s / _elements[4], s / _elements[5], s / _elements[6], s / _elements[7],
                  s / _elements[8], s / _elements[9], s / _elements[10], s / _elements[11],
                  s / _elements[12], s / _elements[13], s / _elements[14], s / _elements[15]);
}

// MARK: - Augmented operator methods - this instance (+)= input
template<typename T>
void Matrix<T, 4, 4>::iadd(T s) {
    _elements[0] += s;
    _elements[1] += s;
    _elements[2] += s;
    _elements[3] += s;
    _elements[4] += s;
    _elements[5] += s;
    _elements[6] += s;
    _elements[7] += s;
    _elements[8] += s;
    _elements[9] += s;
    _elements[10] += s;
    _elements[11] += s;
    _elements[12] += s;
    _elements[13] += s;
    _elements[14] += s;
    _elements[15] += s;
}

template<typename T>
void Matrix<T, 4, 4>::iadd(const Matrix &m) {
    _elements[0] += m._elements[0];
    _elements[1] += m._elements[1];
    _elements[2] += m._elements[2];
    _elements[3] += m._elements[3];
    _elements[4] += m._elements[4];
    _elements[5] += m._elements[5];
    _elements[6] += m._elements[6];
    _elements[7] += m._elements[7];
    _elements[8] += m._elements[8];
    _elements[9] += m._elements[9];
    _elements[10] += m._elements[10];
    _elements[11] += m._elements[11];
    _elements[12] += m._elements[12];
    _elements[13] += m._elements[13];
    _elements[14] += m._elements[14];
    _elements[15] += m._elements[15];
}

template<typename T>
void Matrix<T, 4, 4>::isub(T s) {
    _elements[0] -= s;
    _elements[1] -= s;
    _elements[2] -= s;
    _elements[3] -= s;
    _elements[4] -= s;
    _elements[5] -= s;
    _elements[6] -= s;
    _elements[7] -= s;
    _elements[8] -= s;
    _elements[9] -= s;
    _elements[10] -= s;
    _elements[11] -= s;
    _elements[12] -= s;
    _elements[13] -= s;
    _elements[14] -= s;
    _elements[15] -= s;
}

template<typename T>
void Matrix<T, 4, 4>::isub(const Matrix &m) {
    _elements[0] -= m._elements[0];
    _elements[1] -= m._elements[1];
    _elements[2] -= m._elements[2];
    _elements[3] -= m._elements[3];
    _elements[4] -= m._elements[4];
    _elements[5] -= m._elements[5];
    _elements[6] -= m._elements[6];
    _elements[7] -= m._elements[7];
    _elements[8] -= m._elements[8];
    _elements[9] -= m._elements[9];
    _elements[10] -= m._elements[10];
    _elements[11] -= m._elements[11];
    _elements[12] -= m._elements[12];
    _elements[13] -= m._elements[13];
    _elements[14] -= m._elements[14];
    _elements[15] -= m._elements[15];
}

template<typename T>
void Matrix<T, 4, 4>::imul(T s) {
    _elements[0] *= s;
    _elements[1] *= s;
    _elements[2] *= s;
    _elements[3] *= s;
    _elements[4] *= s;
    _elements[5] *= s;
    _elements[6] *= s;
    _elements[7] *= s;
    _elements[8] *= s;
    _elements[9] *= s;
    _elements[10] *= s;
    _elements[11] *= s;
    _elements[12] *= s;
    _elements[13] *= s;
    _elements[14] *= s;
    _elements[15] *= s;
}

template<typename T>
void Matrix<T, 4, 4>::imul(const Matrix &m) {
    set(mul(m));
}

template<typename T>
void Matrix<T, 4, 4>::idiv(T s) {
    _elements[0] /= s;
    _elements[1] /= s;
    _elements[2] /= s;
    _elements[3] /= s;
    _elements[4] /= s;
    _elements[5] /= s;
    _elements[6] /= s;
    _elements[7] /= s;
    _elements[8] /= s;
    _elements[9] /= s;
    _elements[10] /= s;
    _elements[11] /= s;
    _elements[12] /= s;
    _elements[13] /= s;
    _elements[14] /= s;
    _elements[15] /= s;
}


// MARK: - Modifiers
template<typename T>
void Matrix<T, 4, 4>::transpose() {
    std::swap(_elements[1], _elements[4]);
    std::swap(_elements[2], _elements[8]);
    std::swap(_elements[3], _elements[12]);
    std::swap(_elements[6], _elements[9]);
    std::swap(_elements[7], _elements[13]);
    std::swap(_elements[11], _elements[14]);
}

template<typename T>
void Matrix<T, 4, 4>::invert() {
    T d = determinant();
    Matrix m;
    m._elements[0] = _elements[5] * _elements[10] * _elements[15]
    + _elements[6] * _elements[11] * _elements[13]
    + _elements[7] * _elements[9] * _elements[14]
    - _elements[5] * _elements[11] * _elements[14]
    - _elements[6] * _elements[9] * _elements[15]
    - _elements[7] * _elements[10] * _elements[13];
    m._elements[1] = _elements[1] * _elements[11] * _elements[14]
    + _elements[2] * _elements[9] * _elements[15]
    + _elements[3] * _elements[10] * _elements[13]
    - _elements[1] * _elements[10] * _elements[15]
    - _elements[2] * _elements[11] * _elements[13]
    - _elements[3] * _elements[9] * _elements[14];
    m._elements[2] = _elements[1] * _elements[6] * _elements[15]
    + _elements[2] * _elements[7] * _elements[13]
    + _elements[3] * _elements[5] * _elements[14]
    - _elements[1] * _elements[7] * _elements[14]
    - _elements[2] * _elements[5] * _elements[15]
    - _elements[3] * _elements[6] * _elements[13];
    m._elements[3] = _elements[1] * _elements[7] * _elements[10]
    + _elements[2] * _elements[5] * _elements[11]
    + _elements[3] * _elements[6] * _elements[9]
    - _elements[1] * _elements[6] * _elements[11]
    - _elements[2] * _elements[7] * _elements[9]
    - _elements[3] * _elements[5] * _elements[10];
    m._elements[4] = _elements[4] * _elements[11] * _elements[14]
    + _elements[6] * _elements[8] * _elements[15]
    + _elements[7] * _elements[10] * _elements[12]
    - _elements[4] * _elements[10] * _elements[15]
    - _elements[6] * _elements[11] * _elements[12]
    - _elements[7] * _elements[8] * _elements[14];
    m._elements[5] = _elements[0] * _elements[10] * _elements[15]
    + _elements[2] * _elements[11] * _elements[12]
    + _elements[3] * _elements[8] * _elements[14]
    - _elements[0] * _elements[11] * _elements[14]
    - _elements[2] * _elements[8] * _elements[15]
    - _elements[3] * _elements[10] * _elements[12];
    m._elements[6] = _elements[0] * _elements[7] * _elements[14]
    + _elements[2] * _elements[4] * _elements[15]
    + _elements[3] * _elements[6] * _elements[12]
    - _elements[0] * _elements[6] * _elements[15]
    - _elements[2] * _elements[7] * _elements[12]
    - _elements[3] * _elements[4] * _elements[14];
    m._elements[7] = _elements[0] * _elements[6] * _elements[11]
    + _elements[2] * _elements[7] * _elements[8]
    + _elements[3] * _elements[4] * _elements[10]
    - _elements[0] * _elements[7] * _elements[10]
    - _elements[2] * _elements[4] * _elements[11]
    - _elements[3] * _elements[6] * _elements[8];
    m._elements[8] = _elements[4] * _elements[9] * _elements[15]
    + _elements[5] * _elements[11] * _elements[12]
    + _elements[7] * _elements[8] * _elements[13]
    - _elements[4] * _elements[11] * _elements[13]
    - _elements[5] * _elements[8] * _elements[15]
    - _elements[7] * _elements[9] * _elements[12];
    m._elements[9] = _elements[0] * _elements[11] * _elements[13]
    + _elements[1] * _elements[8] * _elements[15]
    + _elements[3] * _elements[9] * _elements[12]
    - _elements[0] * _elements[9] * _elements[15]
    - _elements[1] * _elements[11] * _elements[12]
    - _elements[3] * _elements[8] * _elements[13];
    m._elements[10] = _elements[0] * _elements[5] * _elements[15]
    + _elements[1] * _elements[7] * _elements[12]
    + _elements[3] * _elements[4] * _elements[13]
    - _elements[0] * _elements[7] * _elements[13]
    - _elements[1] * _elements[4] * _elements[15]
    - _elements[3] * _elements[5] * _elements[12];
    m._elements[11] = _elements[0] * _elements[7] * _elements[9]
    + _elements[1] * _elements[4] * _elements[11]
    + _elements[3] * _elements[5] * _elements[8]
    - _elements[0] * _elements[5] * _elements[11]
    - _elements[1] * _elements[7] * _elements[8]
    - _elements[3] * _elements[4] * _elements[9];
    m._elements[12] = _elements[4] * _elements[10] * _elements[13]
    + _elements[5] * _elements[8] * _elements[14]
    + _elements[6] * _elements[9] * _elements[12]
    - _elements[4] * _elements[9] * _elements[14]
    - _elements[5] * _elements[10] * _elements[12]
    - _elements[6] * _elements[8] * _elements[13];
    m._elements[13] = _elements[0] * _elements[9] * _elements[14]
    + _elements[1] * _elements[10] * _elements[12]
    + _elements[2] * _elements[8] * _elements[13]
    - _elements[0] * _elements[10] * _elements[13]
    - _elements[1] * _elements[8] * _elements[14]
    - _elements[2] * _elements[9] * _elements[12];
    m._elements[14] = _elements[0] * _elements[6] * _elements[13]
    + _elements[1] * _elements[4] * _elements[14]
    + _elements[2] * _elements[5] * _elements[12]
    - _elements[0] * _elements[5] * _elements[14]
    - _elements[1] * _elements[6] * _elements[12]
    - _elements[2] * _elements[4] * _elements[13];
    m._elements[15] = _elements[0] * _elements[5] * _elements[10]
    + _elements[1] * _elements[6] * _elements[8]
    + _elements[2] * _elements[4] * _elements[9]
    - _elements[0] * _elements[6] * _elements[9]
    - _elements[1] * _elements[4] * _elements[10]
    - _elements[2] * _elements[5] * _elements[8];
    m.idiv(d);
    
    set(m);
}


// MARK: - Complex getters
template<typename T>
T Matrix<T, 4, 4>::sum() const {
    T s = 0;
    for (int i = 0; i < 16; ++i) {
        s += _elements[i];
    }
    return s;
}

template<typename T>
T Matrix<T, 4, 4>::avg() const {
    return sum() / 16;
}

template<typename T>
T Matrix<T, 4, 4>::min() const {
    return minn(data(), 16);
}

template<typename T>
T Matrix<T, 4, 4>::max() const {
    return maxn(data(), 16);
}

template<typename T>
T Matrix<T, 4, 4>::absmin() const {
    return absminn(data(), 16);
}

template<typename T>
T Matrix<T, 4, 4>::absmax() const {
    return absmaxn(data(), 16);
}

template<typename T>
T Matrix<T, 4, 4>::trace() const {
    return _elements[0] + _elements[5] + _elements[10] + _elements[15];
}

template<typename T>
T Matrix<T, 4, 4>::determinant() const {
    return
    _elements[0] * _elements[5] * _elements[10] * _elements[15]
    + _elements[0] * _elements[6] * _elements[11] * _elements[13]
    + _elements[0] * _elements[7] * _elements[9] * _elements[14]
    + _elements[1] * _elements[4] * _elements[11] * _elements[14]
    + _elements[1] * _elements[6] * _elements[8] * _elements[15]
    + _elements[1] * _elements[7] * _elements[10] * _elements[12]
    + _elements[2] * _elements[4] * _elements[9] * _elements[15]
    + _elements[2] * _elements[5] * _elements[11] * _elements[12]
    + _elements[2] * _elements[7] * _elements[8] * _elements[13]
    + _elements[3] * _elements[4] * _elements[10] * _elements[13]
    + _elements[3] * _elements[5] * _elements[8] * _elements[14]
    + _elements[3] * _elements[6] * _elements[9] * _elements[12]
    - _elements[0] * _elements[5] * _elements[11] * _elements[14]
    - _elements[0] * _elements[6] * _elements[9] * _elements[15]
    - _elements[0] * _elements[7] * _elements[10] * _elements[13]
    - _elements[1] * _elements[4] * _elements[10] * _elements[15]
    - _elements[1] * _elements[6] * _elements[11] * _elements[12]
    - _elements[1] * _elements[7] * _elements[8] * _elements[14]
    - _elements[2] * _elements[4] * _elements[11] * _elements[13]
    - _elements[2] * _elements[5] * _elements[8] * _elements[15]
    - _elements[2] * _elements[7] * _elements[9] * _elements[12]
    - _elements[3] * _elements[4] * _elements[9] * _elements[14]
    - _elements[3] * _elements[5] * _elements[10] * _elements[12]
    - _elements[3] * _elements[6] * _elements[8] * _elements[13];
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::diagonal() const {
    return Matrix(_elements[0], 0, 0, 0,
                  0, _elements[5], 0, 0,
                  0, 0, _elements[10], 0,
                  0, 0, 0, _elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::offDiagonal() const {
    return Matrix(0, _elements[1], _elements[2], _elements[3],
                  _elements[4], 0, _elements[6], _elements[7],
                  _elements[8], _elements[9], 0, _elements[11],
                  _elements[12], _elements[13], _elements[14], 0);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::strictLowerTri() const {
    return Matrix(0, _elements[1], _elements[2], _elements[3],
                  0, 0, _elements[6], _elements[7],
                  0, 0, 0, _elements[11],
                  0, 0, 0, 0);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::strictUpperTri() const {
    return Matrix(0, 0, 0, 0,
                  _elements[4], 0, 0, 0,
                  _elements[8], _elements[9], 0, 0,
                  _elements[12], _elements[13], _elements[14], 0);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::lowerTri() const {
    return Matrix(_elements[0], _elements[1], _elements[2], _elements[3],
                  0, _elements[5], _elements[6], _elements[7],
                  0, 0, _elements[10], _elements[11],
                  0, 0, 0, _elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::upperTri() const {
    return Matrix(_elements[0], 0, 0, 0,
                  _elements[4], _elements[5], 0, 0,
                  _elements[8], _elements[9], _elements[10], 0,
                  _elements[12], _elements[13], _elements[14], _elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::transposed() const {
    return Matrix(_elements[0], _elements[4], _elements[8], _elements[12],
                  _elements[1], _elements[5], _elements[9], _elements[13],
                  _elements[2], _elements[6], _elements[10], _elements[14],
                  _elements[3], _elements[7], _elements[11], _elements[15]);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::inverse() const {
    Matrix m(*this);
    m.invert();
    return m;
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rotateAxisAngle(const Vector3 <T> &axis, T r) const {
    T x = axis.x;
    T y = axis.y;
    T z = axis.z;
    T len = std::sqrt(x * x + y * y + z * z);
    
    if (std::abs(len) < std::numeric_limits<T>::epsilon()) {
        return Matrix<T, 4, 4>::makeIdentity();
    }
    
    len = 1 / len;
    x *= len;
    y *= len;
    z *= len;
    
    T s = std::sin(r);
    T c = std::cos(r);
    T t = 1 - c;
    
    T a11 = _elements[0],
    a12 = _elements[1],
    a13 = _elements[2],
    a14 = _elements[3];
    T a21 = _elements[4],
    a22 = _elements[5],
    a23 = _elements[6],
    a24 = _elements[7];
    T a31 = _elements[8],
    a32 = _elements[9],
    a33 = _elements[10],
    a34 = _elements[11];
    
    // Construct the elements of the rotation matrix
    T b11 = x * x * t + c;
    T b12 = y * x * t + z * s;
    T b13 = z * x * t - y * s;
    T b21 = x * y * t - z * s;
    T b22 = y * y * t + c;
    T b23 = z * y * t + x * s;
    T b31 = x * z * t + y * s;
    T b32 = y * z * t - x * s;
    T b33 = z * z * t + c;
    
    // Perform rotation-specific matrix multiplication
    return Matrix<T, 4, 4>(a11 * b11 + a21 * b12 + a31 * b13,
                           a12 * b11 + a22 * b12 + a32 * b13,
                           a13 * b11 + a23 * b12 + a33 * b13,
                           a14 * b11 + a24 * b12 + a34 * b13,
                           
                           a11 * b21 + a21 * b22 + a31 * b23,
                           a12 * b21 + a22 * b22 + a32 * b23,
                           a13 * b21 + a23 * b22 + a33 * b23,
                           a14 * b21 + a24 * b22 + a34 * b23,
                           
                           a11 * b31 + a21 * b32 + a31 * b33,
                           a12 * b31 + a22 * b32 + a32 * b33,
                           a13 * b31 + a23 * b32 + a33 * b33,
                           a14 * b31 + a24 * b32 + a34 * b33,
                           
                           _elements[12],
                           _elements[13],
                           _elements[14],
                           _elements[15]);
}

template<typename T>
void Matrix<T, 4, 4>::rotateAxisAngle(const Vector3 <T> &axis, T r) {
    T x = axis.x;
    T y = axis.y;
    T z = axis.z;
    T len = std::sqrt(x * x + y * y + z * z);
    
    if (std::abs(len) < std::numeric_limits<T>::epsilon()) {
        return;
    }
    
    len = 1 / len;
    x *= len;
    y *= len;
    z *= len;
    
    T s = std::sin(r);
    T c = std::cos(r);
    T t = 1 - c;
    
    T a11 = _elements[0],
    a12 = _elements[1],
    a13 = _elements[2],
    a14 = _elements[3];
    T a21 = _elements[4],
    a22 = _elements[5],
    a23 = _elements[6],
    a24 = _elements[7];
    T a31 = _elements[8],
    a32 = _elements[9],
    a33 = _elements[10],
    a34 = _elements[11];
    
    // Construct the elements of the rotation matrix
    T b11 = x * x * t + c;
    T b12 = y * x * t + z * s;
    T b13 = z * x * t - y * s;
    T b21 = x * y * t - z * s;
    T b22 = y * y * t + c;
    T b23 = z * y * t + x * s;
    T b31 = x * z * t + y * s;
    T b32 = y * z * t - x * s;
    T b33 = z * z * t + c;
    
    // Perform rotation-specific matrix multiplication
    _elements[0] = a11 * b11 + a21 * b12 + a31 * b13;
    _elements[1] = a12 * b11 + a22 * b12 + a32 * b13;
    _elements[2] = a13 * b11 + a23 * b12 + a33 * b13;
    _elements[3] = a14 * b11 + a24 * b12 + a34 * b13;
    
    _elements[4] = a11 * b21 + a21 * b22 + a31 * b23;
    _elements[5] = a12 * b21 + a22 * b22 + a32 * b23;
    _elements[6] = a13 * b21 + a23 * b22 + a33 * b23;
    _elements[7] = a14 * b21 + a24 * b22 + a34 * b23;
    
    _elements[8] = a11 * b31 + a21 * b32 + a31 * b33;
    _elements[9] = a12 * b31 + a22 * b32 + a32 * b33;
    _elements[10] = a13 * b31 + a23 * b32 + a33 * b33;
    _elements[11] = a14 * b31 + a24 * b32 + a34 * b33;
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::scale(const Vector3 <T> &s) const {
    T x = s.x;
    T y = s.y;
    T z = s.z;
    
    return Matrix<T, 4, 4>(_elements[0] * x,
                           _elements[1] * x,
                           _elements[2] * x,
                           _elements[3] * x,
                           
                           _elements[4] * y,
                           _elements[5] * y,
                           _elements[6] * y,
                           _elements[7] * y,
                           
                           _elements[8] * z,
                           _elements[9] * z,
                           _elements[10] * z,
                           _elements[11] * z,
                           
                           _elements[12],
                           _elements[13],
                           _elements[14],
                           _elements[15]);
    
}

template<typename T>
void Matrix<T, 4, 4>::scale(const Vector3 <T> &s) {
    T x = s.x;
    T y = s.y;
    T z = s.z;
    
    _elements[0] *= x;
    _elements[1] *= x;
    _elements[2] *= x;
    _elements[3] *= x;
    
    _elements[4] *= y;
    _elements[5] *= y;
    _elements[6] *= y;
    _elements[7] *= y;
    
    _elements[8] *= z;
    _elements[9] *= z;
    _elements[10] *= z;
    _elements[11] *= z;
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::translate(const Vector3 <T> &v) const {
    T x = v.x;
    T y = v.y;
    T z = v.z;
    
    T a11 = _elements[0],
    a12 = _elements[1],
    a13 = _elements[2],
    a14 = _elements[3];
    T a21 = _elements[4],
    a22 = _elements[5],
    a23 = _elements[6],
    a24 = _elements[7];
    T a31 = _elements[8],
    a32 = _elements[9],
    a33 = _elements[10],
    a34 = _elements[11];
    
    return Matrix<T, 4, 4>(a11,
                           a12,
                           a13,
                           a14,
                           
                           a21,
                           a22,
                           a23,
                           a24,
                           
                           a31,
                           a32,
                           a33,
                           a34,
                           
                           a11 * x + a21 * y + a31 * z + _elements[12],
                           a12 * x + a22 * y + a32 * z + _elements[13],
                           a13 * x + a23 * y + a33 * z + _elements[14],
                           a14 * x + a24 * y + a34 * z + _elements[15]);
}

template<typename T>
void Matrix<T, 4, 4>::translate(const Vector3 <T> &v) {
    T x = v.x;
    T y = v.y;
    T z = v.z;
    
    _elements[12] = _elements[0] * x + _elements[4] * y + _elements[8] * z + _elements[12];
    _elements[13] = _elements[1] * x + _elements[5] * y + _elements[9] * z + _elements[13];
    _elements[14] = _elements[2] * x + _elements[6] * y + _elements[10] * z + _elements[14];
    _elements[15] = _elements[3] * x + _elements[7] * y + _elements[11] * z + _elements[15];
}

template<typename T>
template<typename U>
Matrix<U, 4, 4> Matrix<T, 4, 4>::castTo() const {
    return Matrix<U, 4, 4>(static_cast<U>(_elements[0]),
                           static_cast<U>(_elements[1]),
                           static_cast<U>(_elements[2]),
                           static_cast<U>(_elements[3]),
                           static_cast<U>(_elements[4]),
                           static_cast<U>(_elements[5]),
                           static_cast<U>(_elements[6]),
                           static_cast<U>(_elements[7]),
                           static_cast<U>(_elements[8]),
                           static_cast<U>(_elements[9]),
                           static_cast<U>(_elements[10]),
                           static_cast<U>(_elements[11]),
                           static_cast<U>(_elements[12]),
                           static_cast<U>(_elements[13]),
                           static_cast<U>(_elements[14]),
                           static_cast<U>(_elements[15]));
}


// MARK: - Setter operators
template<typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator=(const Matrix &m) {
    set(m);
    return *this;
}

template<typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator+=(T s) {
    iadd(s);
    return *this;
}

template<typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator+=(const Matrix &m) {
    iadd(m);
    return *this;
}

template<typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator-=(T s) {
    isub(s);
    return *this;
}

template<typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator-=(const Matrix &m) {
    isub(m);
    return *this;
}

template<typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator*=(T s) {
    imul(s);
    return *this;
}

template<typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator*=(const Matrix &m) {
    imul(m);
    return *this;
}

template<typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator/=(T s) {
    idiv(s);
    return *this;
}

template<typename T>
bool Matrix<T, 4, 4>::operator==(const Matrix &m) const {
    return _elements[0] == m._elements[0] && _elements[1] == m._elements[1] && _elements[2] == m._elements[2] &&
    _elements[3] == m._elements[3] && _elements[4] == m._elements[4] && _elements[5] == m._elements[5] &&
    _elements[6] == m._elements[6] && _elements[7] == m._elements[7] && _elements[8] == m._elements[8] &&
    _elements[9] == m._elements[9] && _elements[10] == m._elements[10] && _elements[11] == m._elements[11] &&
    _elements[12] == m._elements[12] && _elements[13] == m._elements[13] && _elements[14] == m._elements[14] &&
    _elements[15] == m._elements[15];
}

template<typename T>
bool Matrix<T, 4, 4>::operator!=(const Matrix &m) const {
    return _elements[0] != m._elements[0] || _elements[1] != m._elements[1] || _elements[2] != m._elements[2] ||
    _elements[3] != m._elements[3] || _elements[4] != m._elements[4] || _elements[5] != m._elements[5] ||
    _elements[6] != m._elements[6] || _elements[7] != m._elements[7] || _elements[8] != m._elements[8] ||
    _elements[9] != m._elements[9] || _elements[10] != m._elements[10] || _elements[11] != m._elements[11] ||
    _elements[12] != m._elements[12] || _elements[13] != m._elements[13] || _elements[14] != m._elements[14] ||
    _elements[15] != m._elements[15];
}


// MARK: - Getter operators
template<typename T>
T &Matrix<T, 4, 4>::operator[](size_t i) {
    return _elements[i];
}

template<typename T>
const T &Matrix<T, 4, 4>::operator[](size_t i) const {
    return _elements[i];
}

template<typename T>
T &Matrix<T, 4, 4>::operator()(size_t i, size_t j) {
    return _elements[i + 4 * j];
}

template<typename T>
const T &Matrix<T, 4, 4>::operator()(size_t i, size_t j) const {
    return _elements[i + 4 * j];
}


// MARK: - Helpers
template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::makeZero() {
    return Matrix(0, 0, 0, 0,
                  0, 0, 0, 0,
                  0, 0, 0, 0,
                  0, 0, 0, 0);
}

template<typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::makeIdentity() {
    return Matrix(1, 0, 0, 0,
                  0, 1, 0, 0,
                  0, 0, 1, 0,
                  0, 0, 0, 1);
}


// MARK: - Operator overloadings
template<typename T>
Matrix<T, 4, 4> operator-(const Matrix<T, 4, 4> &a) {
    return a.mul(-1);
}

template<typename T>
Matrix<T, 4, 4> operator+(const Matrix<T, 4, 4> &a, const Matrix<T, 4, 4> &b) {
    return a.add(b);
}

template<typename T>
Matrix<T, 4, 4> operator+(const Matrix<T, 4, 4> &a, T b) {
    return a.add(b);
}

template<typename T>
Matrix<T, 4, 4> operator+(T a, const Matrix<T, 4, 4> &b) {
    return b.radd(a);
}

template<typename T>
Matrix<T, 4, 4> operator-(const Matrix<T, 4, 4> &a, const Matrix<T, 4, 4> &b) {
    return a.sub(b);
}

template<typename T>
Matrix<T, 4, 4> operator-(const Matrix<T, 4, 4> &a, T b) {
    return a.sub(b);
}

template<typename T>
Matrix<T, 4, 4> operator-(T a, const Matrix<T, 4, 4> &b) {
    return b.rsub(a);
}

template<typename T>
Matrix<T, 4, 4> operator*(const Matrix<T, 4, 4> &a, T b) {
    return a.mul(b);
}

template<typename T>
Matrix<T, 4, 4> operator*(T a, const Matrix<T, 4, 4> &b) {
    return b.rmul(a);
}

template<typename T>
Point3 <T> operator*(const Matrix<T, 4, 4> &a, const Point3 <T> &b) {
    return a.mul(b);
}

template<typename T>
Vector<T, 3> operator*(const Matrix<T, 4, 4> &a, const Vector<T, 3> &b) {
    return a.mul(b);
}

template<typename T>
Vector<T, 4> operator*(const Matrix<T, 4, 4> &a, const Vector<T, 4> &b) {
    return a.mul(b);
}

template<typename T>
Matrix<T, 4, 4> operator*(const Matrix<T, 4, 4> &a, const Matrix<T, 4, 4> &b) {
    return a.mul(b);
}

template<typename T>
Matrix<T, 4, 4> operator/(const Matrix<T, 4, 4> &a, T b) {
    return a.div(b);
}

template<typename T>
Matrix<T, 4, 4> operator/(T a, const Matrix<T, 4, 4> &b) {
    return b.rdiv(a);
}

}  // namespace vox

#endif  // INCLUDE_VOX_DETAIL_MATRIX4X4_INL_H_
