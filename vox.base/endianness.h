//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

// Declares endianness modes and functions to swap data from a mode to another.

#include <cstddef>

#include "vox.base/macros.h"

namespace vox {

// Declares supported endianness.
enum Endianness {
    kBigEndian,
    kLittleEndian,
};

// Get the native endianness of the targeted processor.
// This function does not rely on a pre-processor definition as no standard
// definition exists. It is rather implemented as a portable runtime function.
inline Endianness getNativeEndianness() {
    const union {
        uint16_t s;
        uint8_t c[2];
    } u = {1};  // Initializes u.s -> then read u.c.
    return Endianness(u.c[0]);
}

// Declare the endian swapper struct that is aimed to be specialized (template
// meaning) for every type sizes.
// The swapper provides two functions:
// - void Swap(_Ty* _ty, size_t _count) swaps the array _ty of _count
// elements in-place.
// - _Ty Swap(_Ty _ty) returns a swapped copy of _ty.
// It can be used directly if _Ty is known or through EndianSwap function.
// The default value of template attribute _size enables automatic
// specialization selection.
template <typename Ty, size_t _size = sizeof(Ty)>
struct EndianSwapper;

// Internal macro used to swap two bytes.
#define VOX_BYTE_SWAP(_a, _b)        \
    do {                             \
        const vox::byte temp = (_a); \
        (_a) = (_b);                 \
        (_b) = temp;                 \
    } while (0)

// EndianSwapper specialization for 1 byte types.
template <typename Ty>
struct EndianSwapper<Ty, 1> {
    VOX_INLINE static void swap(Ty*, size_t) {}
    VOX_INLINE static Ty swap(Ty _ty) { return _ty; }
};

// EndianSwapper specialization for 2 bytes types.
template <typename Ty>
struct EndianSwapper<Ty, 2> {
    VOX_INLINE static void swap(Ty* _ty, size_t _count) {
        byte* alias = reinterpret_cast<byte*>(_ty);
        for (size_t i = 0; i < _count * 2; i += 2) {
            VOX_BYTE_SWAP(alias[i + 0], alias[i + 1]);
        }
    }
    VOX_INLINE static Ty swap(Ty _ty) {  // Pass by copy to swap _ty in-place.
        byte* alias = reinterpret_cast<byte*>(&_ty);
        VOX_BYTE_SWAP(alias[0], alias[1]);
        return _ty;
    }
};

// EndianSwapper specialization for 4 bytes types.
template <typename Ty>
struct EndianSwapper<Ty, 4> {
    VOX_INLINE static void swap(Ty* _ty, size_t _count) {
        byte* alias = reinterpret_cast<byte*>(_ty);
        for (size_t i = 0; i < _count * 4; i += 4) {
            VOX_BYTE_SWAP(alias[i + 0], alias[i + 3]);
            VOX_BYTE_SWAP(alias[i + 1], alias[i + 2]);
        }
    }
    VOX_INLINE static Ty swap(Ty _ty) {  // Pass by copy to swap _ty in-place.
        byte* alias = reinterpret_cast<byte*>(&_ty);
        VOX_BYTE_SWAP(alias[0], alias[3]);
        VOX_BYTE_SWAP(alias[1], alias[2]);
        return _ty;
    }
};

// EndianSwapper specialization for 8 bytes types.
template <typename Ty>
struct EndianSwapper<Ty, 8> {
    VOX_INLINE static void swap(Ty* _ty, size_t _count) {
        byte* alias = reinterpret_cast<byte*>(_ty);
        for (size_t i = 0; i < _count * 8; i += 8) {
            VOX_BYTE_SWAP(alias[i + 0], alias[i + 7]);
            VOX_BYTE_SWAP(alias[i + 1], alias[i + 6]);
            VOX_BYTE_SWAP(alias[i + 2], alias[i + 5]);
            VOX_BYTE_SWAP(alias[i + 3], alias[i + 4]);
        }
    }
    VOX_INLINE static Ty swap(Ty _ty) {  // Pass by copy to swap _ty in-place.
        byte* alias = reinterpret_cast<byte*>(&_ty);
        VOX_BYTE_SWAP(alias[0], alias[7]);
        VOX_BYTE_SWAP(alias[1], alias[6]);
        VOX_BYTE_SWAP(alias[2], alias[5]);
        VOX_BYTE_SWAP(alias[3], alias[4]);
        return _ty;
    }
};

// VOX_BYTE_SWAP is not useful anymore.
#undef VOX_BYTE_SWAP

// Helper function that swaps _count elements of the array _ty in place.
template <typename Ty>
VOX_INLINE void endianSwap(Ty* _ty, size_t _count) {
    EndianSwapper<Ty>::swap(_ty, _count);
}

// Helper function that swaps _ty in place.
template <typename Ty>
VOX_INLINE Ty endianSwap(Ty _ty) {
    return EndianSwapper<Ty>::swap(_ty);
}
}  // namespace vox
