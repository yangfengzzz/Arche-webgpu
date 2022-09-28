//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

// Provides traits for customizing archive serialization properties: version,
// tag... See archive.h for more details.

#include <cstddef>
#include <cstdint>

namespace vox::io {

// Forward declaration of archive types.
class OArchive;
class IArchive;

// Default loading and saving external declaration.
// Those template implementations aim to be specialized at compilation time by
// non-member Load and save functions. For example the specialization of the
// Save() function for a type Type is:
// void Save(OArchive& _archive, const Extrusive* _test, size_t _count) {
// }
// The Load() function receives the version _version of type _Ty at the time the
// archive was saved.
// This uses polymorphism rather than template specialization to avoid
// including the file that contains the template definition.
//
// This default function call member _Ty::Load/Save function.
template <typename Ty>
struct Extern;

// Declares the current (compile time) version of _type.
// This macro must be used inside namespace vox::io.
// Syntax is: VOX_IO_TYPE_VERSION(46, Foo).
#define VOX_IO_TYPE_VERSION(_version, _type)                     \
    static_assert((_version) > 0, "Version number must be > 0"); \
    namespace internal {                                         \
    template <>                                                  \
    struct Version<const _type> {                                \
        enum { kValue = (_version) };                            \
    };                                                           \
    }  // internal

// Declares the current (compile time) version of a template _type.
// This macro must be used inside namespace vox::io.
// VOX_IO_TYPE_VERSION_T1(46, typename _T1, Foo<_T1>).
#define VOX_IO_TYPE_VERSION_T1(_version, _arg0, ...)           \
    static_assert(_version > 0, "Version number must be > 0"); \
    namespace internal {                                       \
    template <_arg0>                                           \
    struct Version<const __VA_ARGS__> {                        \
        enum { kValue = _version };                            \
    };                                                         \
    }  // internal

// Declares the current (compile time) version of a template _type.
// This macro must be used inside namespace vox::io.
// VOX_IO_TYPE_VERSION_T2(46, typename _T1, typename _T2, Foo<_T1, _T2>).
#define VOX_IO_TYPE_VERSION_T2(_version, _arg0, _arg1, ...)    \
    static_assert(_version > 0, "Version number must be > 0"); \
    namespace internal {                                       \
    template <_arg0, _arg1>                                    \
    struct Version<const __VA_ARGS__> {                        \
        enum { kValue = _version };                            \
    };                                                         \
                                                               \
    }  // internal

// Declares the current (compile time) version of a template _type.
// This macro must be used inside namespace vox::io.
// VOX_IO_TYPE_VERSION_T3(
//   46, typename _T1, typename _T2, typename _T3, Foo<_T1, _T2, _T3>).
#define VOX_IO_TYPE_VERSION_T3(_version, _arg0, _arg1, _arg2, ...) \
    static_assert(_version > 0, "Version number must be > 0");     \
    namespace internal {                                           \
    template <_arg0, _arg1, _arg2>                                 \
    struct Version<const __VA_ARGS__> {                            \
        enum { kValue = _version };                                \
    };                                                             \
                                                                   \
    }  // internal

// Declares the current (compile time) version of a template _type.
// This macro must be used inside namespace vox::io.
// VOX_IO_TYPE_VERSION_T4(
//   46, typename _T1, typename _T2, typename _T3, typename _T4,
//   Foo<_T1, _T2, _T3, _T4>).
#define VOX_IO_TYPE_VERSION_T4(_version, _arg0, _arg1, _arg2, _arg3, ...) \
    static_assert(_version > 0, "Version number must be > 0");            \
    namespace internal {                                                  \
    template <_arg0, _arg1, _arg2, _arg3>                                 \
    struct Version<const __VA_ARGS__> {                                   \
        enum { kValue = _version };                                       \
    };                                                                    \
    }  // internal

// Declares that _type is not versionable. Its version number is 0.
// Once a type has been declared not versionable, it cannot be changed without
// braking versioning.
// This macro must be used inside namespace vox::io.
// Syntax is: VOX_IO_TYPE_NOT_VERSIONABLE(Foo).
#define VOX_IO_TYPE_NOT_VERSIONABLE(_type) \
    namespace internal {                   \
    template <>                            \
    struct Version<const _type> {          \
        enum { kValue = 0 };               \
    };                                     \
    }  // internal

// Declares that a template _type is not versionable. Its version number is 0.
// Once a type has been declared not versionable, it cannot be changed without
// braking versioning.
// This macro must be used inside namespace vox::io.
// Syntax is:
// VOX_IO_TYPE_NOT_VERSIONABLE_T1(typename _T1, Foo<_T1>).
#define VOX_IO_TYPE_NOT_VERSIONABLE_T1(_arg0, ...) \
    namespace internal {                           \
    template <_arg0>                               \
    struct Version<const __VA_ARGS__> {            \
        enum { kValue = 0 };                       \
    };                                             \
    }  // internal

// Decline non-versionable template declaration to 2 template arguments.
// Syntax is:
// VOX_IO_TYPE_NOT_VERSIONABLE_T2(typename _T1, typename _T2, Foo<_T1, _T2>).
#define VOX_IO_TYPE_NOT_VERSIONABLE_T2(_arg0, _arg1, ...) \
    namespace internal {                                  \
    template <_arg0, _arg1>                               \
    struct Version<const __VA_ARGS__> {                   \
        enum { kValue = 0 };                              \
    };                                                    \
    }  // internal

// Decline non-versionable template declaration to 3 template arguments.
// Syntax is:
// VOX_IO_TYPE_NOT_VERSIONABLE_T3(
//   typename _T1, typename _T2, typename _T3, Foo<_T1, _T2, _T3>).
#define VOX_IO_TYPE_NOT_VERSIONABLE_T3(_arg0, _arg1, _arg2, ...) \
    namespace internal {                                         \
    template <_arg0, _arg1, _arg2>                               \
    struct Version<const __VA_ARGS__> {                          \
        enum { kValue = 0 };                                     \
    };                                                           \
    }  // internal

// Decline non-versionable template declaration to 4 template arguments.
// Syntax is:
// VOX_IO_TYPE_NOT_VERSIONABLE_T4(
//   typename _T1, typename _T2, typename _T3, typename _T4,
//   Foo<_T1, _T2, _T3, _T4>).
#define VOX_IO_TYPE_NOT_VERSIONABLE_T4(_arg0, _arg1, _arg2, _arg3, ...) \
    namespace internal {                                                \
    template <_arg0, _arg1, _arg2, _arg3>                               \
    struct Version<const __VA_ARGS__> {                                 \
        enum { kValue = 0 };                                            \
    };                                                                  \
    }  // internal

// Declares the tag of a template _type.
// A tag is a c-string that can be used to check the type (through its tag) of
// the next object to be read from an archive. If no tag is defined, then no
// check is performed.
// This macro must be used inside namespace vox::io.
// VOX_IO_TYPE_TAG("Foo", Foo).
#define VOX_IO_TYPE_TAG(_tag, _type)                                      \
    namespace internal {                                                  \
    template <>                                                           \
    struct Tag<const _type> {                                             \
        /* Length includes null terminated character to detect partial */ \
        /* tag mapping.*/                                                 \
        enum { kTagLength = VOX_ARRAY_SIZE(_tag) };                       \
        static const char* Get() { return _tag; }                         \
    };                                                                    \
    }  // internal

namespace internal {
// Definition of version specializable template struct.
// There's no default implementation in order to force user to define it, which
// in turn forces those who want to serialize an object to include the file that
// defines it's version. This helps with detecting issue at compile time.
template <typename Ty>
struct Version;

// Defines default tag value, which is disabled.
template <typename Ty>
struct Tag {
    enum { kTagLength = 0 };
};
}  // namespace internal
}  // namespace vox::io