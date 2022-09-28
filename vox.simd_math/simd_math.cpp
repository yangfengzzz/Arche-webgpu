//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.simd_math/simd_math.h"

namespace vox {
namespace math {

// Select compile time name of the simd implementation
#if defined(VOX_SIMD_AVX2) && defined(VOX_SIMD_FMA)
#define _VOX_SIMD_IMPLEMENTATION "AVX2-FMA"
#elif defined(VOX_SIMD_AVX2)
#define _VOX_SIMD_IMPLEMENTATION "AVX2"
#elif defined(VOX_SIMD_AVX)
#define _VOX_SIMD_IMPLEMENTATION "AVX"
#elif defined(VOX_SIMD_SSE4_2)
#define _VOX_SIMD_IMPLEMENTATION "SSE4.2"
#elif defined(VOX_SIMD_SSE4_1)
#define _VOX_SIMD_IMPLEMENTATION "SSE4.1"
#elif defined(VOX_SIMD_SSSE3)
#define _VOX_SIMD_IMPLEMENTATION "SSSE3"
#elif defined(VOX_SIMD_SSE3)
#define _VOX_SIMD_IMPLEMENTATION "SSE3"
#elif defined(VOX_SIMD_SSEx)
#define _VOX_SIMD_IMPLEMENTATION "SSE2"
#elif defined(VOX_SIMD_REF)
#define _VOX_SIMD_IMPLEMENTATION "Reference"
#else
// Not defined
#endif

#pragma message("Ozz libraries were built with " _VOX_SIMD_IMPLEMENTATION " SIMD math implementation")

const char* SimdImplementationName() { return _VOX_SIMD_IMPLEMENTATION; }
}  // namespace math
}  // namespace vox
