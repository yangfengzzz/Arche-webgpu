//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/macros.h"

#ifndef UNUSED_VARIABLE
#define UNUSED_VARIABLE(x) ((void)x)
#endif

#ifndef CLONE_W_CUSTOM_DELETER
#define CLONE_W_CUSTOM_DELETER(ClassName) \
    std::shared_ptr<ClassName>(new ClassName(*this), [](ClassName* obj) { delete obj; });
#endif

#ifdef JET_WINDOWS

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <exception>
#include <string>

inline void throwIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        throw std::runtime_error(std::to_string(hr).c_str());
    }
}

#ifndef IF_FAILED_CLEANUP
#define IF_FAILED_CLEANUP(_hr) \
    if (FAILED(_hr)) {         \
        hr = _hr;              \
        goto Cleanup;          \
    }
#endif

#ifndef FAIL_AND_CLEANUP
#define FAIL_AND_CLEANUP(_hr) \
    {                         \
        hr = _hr;             \
        goto Cleanup;         \
    }
#endif

#endif  // JET_WINDOWS
