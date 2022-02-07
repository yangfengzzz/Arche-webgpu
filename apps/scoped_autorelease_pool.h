//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef UTILS_SCOPEDAUTORELEASEPOOL_H_
#define UTILS_SCOPEDAUTORELEASEPOOL_H_

#include <cstddef>

namespace utils {

/**
 * ScopedAutoreleasePool is a scoped class which initializes an NSAutoreleasePool on
 * creation, and drains it on destruction. On non-Apple platforms, ScopedAutoreleasePool
 * is a no-op.
 *
 * An autoreleasepool is needed when using protocol objects in Objective-C because Cocoa
 * expects a pool to always be available in each thread. If a pool is not available, then
 * autoreleased objects will never be released and will leak.
 *
 * In long-running blocks of code or loops, it is important to periodically create and drain
 * autorelease pools so that memory is recycled. In Dawn's tests, we have an autoreleasepool
 * per-test. In graphics applications it's advised to create an autoreleasepool around the
 * frame loop. Ex.)
 *   void frame() {
 *     // Any protocol objects will be reclaimed when this object falls out of scope.
 *     utils::ScopedAutoreleasePool pool;
 *
 *     // do rendering ...
 *   }
 */
class [[nodiscard]] ScopedAutoreleasePool {
public:
    ScopedAutoreleasePool();
    ~ScopedAutoreleasePool();
    
    ScopedAutoreleasePool(const ScopedAutoreleasePool&) = delete;
    ScopedAutoreleasePool& operator=(const ScopedAutoreleasePool&) = delete;
    
    ScopedAutoreleasePool(ScopedAutoreleasePool&&);
    ScopedAutoreleasePool& operator=(ScopedAutoreleasePool&&);
    
private:
    void* mPool = nullptr;
};

}  // namespace utils

#endif  // UTILS_SCOPEDAUTORELEASEPOOL_H_
