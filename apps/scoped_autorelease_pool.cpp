//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scoped_autorelease_pool.h"

#include "common/Compiler.h"

namespace utils {
ScopedAutoreleasePool::ScopedAutoreleasePool() : mPool(nullptr) {
    DAWN_UNUSED(mPool);
}

ScopedAutoreleasePool::~ScopedAutoreleasePool() = default;

ScopedAutoreleasePool::ScopedAutoreleasePool(ScopedAutoreleasePool&& rhs) {
}

ScopedAutoreleasePool& ScopedAutoreleasePool::operator=(ScopedAutoreleasePool&& rhs) {
    return *this;
}

}  // namespace utils
