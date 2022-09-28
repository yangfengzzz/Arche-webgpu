//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define VOX_WINDOWS
#elif defined(__APPLE__)
#define VOX_APPLE
#ifndef VOX_IOS
#define VOX_MACOSX
#endif
#elif defined(linux) || defined(__linux__)
#define VOX_LINUX
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define VOX_DEBUG_MODE

#include <cassert>

#define VOX_ASSERT(x) assert(x)
#else
#define VOX_ASSERT(x)
#endif

#ifdef __cplusplus
#include <stdexcept>
#define VOX_THROW_INVALID_ARG_IF(expression)      \
    if (expression) {                             \
        throw std::invalid_argument(#expression); \
    }
#define VOX_THROW_INVALID_ARG_WITH_MESSAGE_IF(expression, message) \
    if (expression) {                                              \
        throw std::invalid_argument(message);                      \
    }
#endif

#if defined(VOX_WINDOWS) && defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#define VOX_DIAG_STR(s) #s
#define VOX_DIAG_JOINSTR(x, y) VOX_DIAG_STR(x##y)
#ifdef _MSC_VER
#define VOX_DIAG_DO_PRAGMA(x) __pragma(#x)
#define VOX_DIAG_PRAGMA(compiler, x) VOX_DIAG_DO_PRAGMA(warning(x))
#else
#define VOX_DIAG_DO_PRAGMA(x) _Pragma(#x)
#define VOX_DIAG_PRAGMA(compiler, x) VOX_DIAG_DO_PRAGMA(compiler diagnostic x)
#endif
#if defined(__clang__)
#define VOX_DISABLE_CLANG_WARNING(clang_option) \
    VOX_DIAG_PRAGMA(clang, push)                \
    VOX_DIAG_PRAGMA(clang, ignored VOX_DIAG_JOINSTR(-W, clang_option))
#define VOX_ENABLE_CLANG_WARNING(clang_option) VOX_DIAG_PRAGMA(clang, pop)
#define VOX_DISABLE_MSVC_WARNING(gcc_option)
#define VOX_ENABLE_MSVC_WARNING(gcc_option)
#define VOX_DISABLE_GCC_WARNING(gcc_option)
#define VOX_ENABLE_GCC_WARNING(gcc_option)
#elif defined(_MSC_VER)
#define VOX_DISABLE_CLANG_WARNING(gcc_option)
#define VOX_ENABLE_CLANG_WARNING(gcc_option)
#define VOX_DISABLE_MSVC_WARNING(msvc_errorcode) \
    VOX_DIAG_PRAGMA(msvc, push)                  \
    VOX_DIAG_DO_PRAGMA(warning(disable :##msvc_errorcode))
#define VOX_ENABLE_MSVC_WARNING(msvc_errorcode) VOX_DIAG_PRAGMA(msvc, pop)
#define VOX_DISABLE_GCC_WARNING(gcc_option)
#define VOX_ENABLE_GCC_WARNING(gcc_option)
#elif defined(__GNUC__)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#define VOX_DISABLE_CLANG_WARNING(gcc_option)
#define VOX_ENABLE_CLANG_WARNING(gcc_option)
#define VOX_DISABLE_MSVC_WARNING(gcc_option)
#define VOX_ENABLE_MSVC_WARNING(gcc_option)
#define VOX_DISABLE_GCC_WARNING(gcc_option) \
    VOX_DIAG_PRAGMA(GCC, push)              \
    VOX_DIAG_PRAGMA(GCC, ignored VOX_DIAG_JOINSTR(-W, gcc_option))
#define VOX_ENABLE_GCC_WARNING(gcc_option) VOX_DIAG_PRAGMA(GCC, pop)
#else
#define VOX_DISABLE_CLANG_WARNING(gcc_option)
#define VOX_ENABLE_CLANG_WARNING(gcc_option)
#define VOX_DISABLE_MSVC_WARNING(gcc_option)
#define VOX_ENABLE_MSVC_WARNING(gcc_option)
#define VOX_DISABLE_GCC_WARNING(gcc_option) VOX_DIAG_PRAGMA(GCC, ignored VOX_DIAG_JOINSTR(-W, gcc_option))
#define VOX_ENABLE_GCC_WARNING(gcc_option) VOX_DIAG_PRAGMA(GCC, warning VOX_DIAG_JOINSTR(-W, gcc_option))
#endif
#endif

// Ensures compiler supports c++11 language standards, to help user understand
// compilation error in case it's not supported.
// Unfortunately MSVC doesn't update __cplusplus, so test compiler version
// instead.
#if !((__cplusplus >= 201103L) || (_MSC_VER >= 1900))
#error "Arche-cpp requires c++11 language standards."
#endif  // __cplusplus

#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER) && defined(VOX_USE_DYNAMIC_LINKING)

#ifdef VOX_BUILD_BASE_LIB
// Import/Export for dynamic linking while building vox
#define VOX_BASE_DLL __declspec(dllexport)
#else
#define VOX_BASE_DLL __declspec(dllimport)
#endif
#else  // defined(_MSC_VER) && defined(VOX_USE_DYNAMIC_LINKING)
// Static or non msvc linking
#define VOX_BASE_DLL
#endif  // defined(_MSC_VER) && defined(VOX_USE_DYNAMIC_LINKING)

namespace vox {
// Defines a byte type, unsigned so right shift  doesn't propagate sign bit.
typedef uint8_t byte;

// Finds the number of elements of a statically allocated array.
#define VOX_ARRAY_SIZE(_array) (sizeof(_array) / sizeof((_array)[0]))

// Instructs the compiler to try to inline a function, regardless cost/benefit
// compiler analysis.
// Syntax is: "VOX_INLINE void function();"
#if defined(_MSC_VER)
#define VOX_INLINE __forceinline
#else
#define VOX_INLINE inline __attribute__((always_inline))
#endif

// Tells the compiler to never inline a function.
// Syntax is: "VOX_NO_INLINE void function();"
#if defined(_MSC_VER)
#define VOX_NOINLINE __declspec(noinline)
#else
#define VOX_NOINLINE __attribute__((noinline))
#endif

// Tells the compiler that the memory addressed by the restrict-qualified
// pointer is not aliased, aka no other pointer will access that same memory.
// Syntax is: void function(int* VOX_RESTRICT _p);"
#define VOX_RESTRICT __restrict

// Defines macro to help with DEBUG/NDEBUG syntax.
#if defined(NDEBUG)
#define VOX_IF_DEBUG(...)
#define VOX_IF_NDEBUG(...) __VA_ARGS__
#else  // NDEBUG
#define VOX_IF_DEBUG(...) __VA_ARGS__
#define VOX_IF_NDEBUG(...)
#endif  // NDEBUG

// Tests whether _block is aligned to _alignment boundary.
template <typename Ty>
VOX_INLINE bool isAligned(Ty _value, size_t _alignment) {
    return (_value & (_alignment - 1)) == 0;
}
template <typename Ty>
VOX_INLINE bool isAligned(Ty* _address, size_t _alignment) {
    return (reinterpret_cast<uintptr_t>(_address) & (_alignment - 1)) == 0;
}

// Aligns _block address to the first greater address that is aligned to
// _alignment boundaries.
template <typename Ty>
VOX_INLINE Ty align(Ty _value, size_t _alignment) {
    return static_cast<Ty>(_value + (_alignment - 1)) & (0 - _alignment);
}
template <typename Ty>
VOX_INLINE Ty* align(Ty* _address, size_t _alignment) {
    return reinterpret_cast<Ty*>((reinterpret_cast<uintptr_t>(_address) + (_alignment - 1)) & (0 - _alignment));
}

// Offset a pointer from a given number of bytes.
template <typename Ty>
Ty* pointerStride(Ty* _ty, size_t _stride) {
    return reinterpret_cast<Ty*>(reinterpret_cast<uintptr_t>(_ty) + _stride);
}
}  // namespace vox