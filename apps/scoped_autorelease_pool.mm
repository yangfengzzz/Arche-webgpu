//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scoped_autorelease_pool.h"

#import <Foundation/Foundation.h>

namespace utils {

ScopedAutoreleasePool::ScopedAutoreleasePool() : mPool([[NSAutoreleasePool alloc] init]) {
}

ScopedAutoreleasePool::~ScopedAutoreleasePool() {
    if (mPool != nullptr) {
        [static_cast<NSAutoreleasePool*>(mPool) release];
        mPool = nullptr;
    }
}

ScopedAutoreleasePool::ScopedAutoreleasePool(ScopedAutoreleasePool&& rhs) {
    mPool = rhs.mPool;
    rhs.mPool = nullptr;
}

ScopedAutoreleasePool& ScopedAutoreleasePool::operator=(ScopedAutoreleasePool&& rhs) {
    if (&rhs != this) {
        mPool = rhs.mPool;
        rhs.mPool = nullptr;
    }
    return *this;
}

}  // namespace utils
