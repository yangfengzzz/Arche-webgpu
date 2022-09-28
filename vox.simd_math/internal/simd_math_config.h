//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/macros.h"

// Avoid SIMD instruction detection if reference (aka scalar) implementation is
// forced.
#if !defined(VOX_BUILD_SIMD_REF)

// Try to match a SSE2+ version.
#if defined(__AVX2__) || defined(VOX_SIMD_AVX2)
#include <immintrin.h>
#define VOX_SIMD_AVX2
#define VOX_SIMD_AVX  // avx is available if avx2 is.
#endif

#if defined(__FMA__) || defined(VOX_SIMD_FMA)
#include <immintrin.h>
#define VOX_SIMD_FMA
#endif

#if defined(__AVX__) || defined(VOX_SIMD_AVX)
#include <immintrin.h>
#define VOX_SIMD_AVX
#define VOX_SIMD_SSE4_2  // SSE4.2 is available if avx is.
#endif

#if defined(__SSE4_2__) || defined(VOX_SIMD_SSE4_2)
#include <nmmintrin.h>
#define VOX_SIMD_SSE4_2
#define VOX_SIMD_SSE4_1  // SSE4.1 is available if SSE4.2 is.
#endif

#if defined(__SSE4_1__) || defined(VOX_SIMD_SSE4_1)
#include <smmintrin.h>
#define VOX_SIMD_SSE4_1
#define VOX_SIMD_SSSE3  // SSSE3 is available if SSE4.1 is.
#endif

#if defined(__SSSE3__) || defined(VOX_SIMD_SSSE3)
#include <tmmintrin.h>
#define VOX_SIMD_SSSE3
#define VOX_SIMD_SSE3  // SSE3 is available if SSSE3 is.
#endif

#if defined(__SSE3__) || defined(VOX_SIMD_SSE3)
#include <pmmintrin.h>
#define VOX_SIMD_SSE3
#define VOX_SIMD_SSE2  // SSE2 is available if SSE3 is.
#endif

// x64/amd64 have SSE2 instructions
// _M_IX86_FP is 2 if /arch:SSE2, /arch:AVX or /arch:AVX2 was used.
#if defined(__SSE2__) || defined(_M_AMD64) || defined(_M_X64) || (_M_IX86_FP >= 2) || defined(VOX_SIMD_SSE2)
#include <emmintrin.h>
#define VOX_SIMD_SSE2
#define VOX_SIMD_SSEx  // VOX_SIMD_SSEx is the generic flag for SSE support
#endif

// End of SIMD instruction detection
#endif  // !VOX_BUILD_SIMD_REF

// SEE* intrinsics available
#if defined(VOX_SIMD_SSEx)

namespace vox::simd_math {

// Vector of four floating point values.
typedef __m128 SimdFloat4;

// Argument type for Float4.
typedef const __m128 _SimdFloat4;

// Vector of four integer values.
typedef __m128i SimdInt4;

// Argument type for Int4.
typedef const __m128i _SimdInt4;
}  // namespace vox::simd_math

#else  // No builtin simd available

// No simd instruction set detected, switch back to reference implementation.
// VOX_SIMD_REF is the generic flag for SIMD reference implementation.
#define VOX_SIMD_REF

// Declares reference simd float and integer vectors outside of vox::simd_math, in
// order to match non-reference implementation details.

// Vector of four floating point values.
struct SimdFloat4Def {
    alignas(16) float x;
    float y;
    float z;
    float w;
};

// Vector of four integer values.
struct SimdInt4Def {
    alignas(16) int x;
    int y;
    int z;
    int w;
};

namespace vox {
namespace simd_math {

// Vector of four floating point values.
typedef SimdFloat4Def SimdFloat4;

// Argument type for SimdFloat4
typedef const SimdFloat4& _SimdFloat4;

// Vector of four integer values.
typedef SimdInt4Def SimdInt4;

// Argument type for SimdInt4.
typedef const SimdInt4& _SimdInt4;

}  // namespace simd_math
}  // namespace vox
#endif  // VOX_SIMD_x

// Native SIMD operator already exist on some compilers, so they have to be
// disabled from vox implementation
#if !defined(VOX_SIMD_REF) && (defined(__GNUC__) || defined(__llvm__))
#define VOX_DISABLE_SSE_NATIVE_OPERATORS
#endif