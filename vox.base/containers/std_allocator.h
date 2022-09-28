//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <new>

#include "vox.base/memory/allocator.h"

namespace vox {
// Define a STL allocator compliant allocator->
template <typename Ty>
class StdAllocator {
public:
    typedef Ty value_type;                      // Element type.
    typedef value_type* pointer;                // Pointer to element.
    typedef value_type& reference;              // Reference to element.
    typedef const value_type* const_pointer;    // Constant pointer to element.
    typedef const value_type& const_reference;  // Constant reference to element.
    typedef size_t size_type;                   // Quantities of elements.
    typedef ptrdiff_t difference_type;          // Difference between two pointers.

    StdAllocator() noexcept = default;
    StdAllocator(const StdAllocator&) noexcept = default;

    template <class Other>
    StdAllocator(const StdAllocator<Other>&) noexcept {}

    template <class Other>
    struct rebind {
        typedef StdAllocator<Other> other;
    };

    pointer address(reference _ref) const noexcept { return &_ref; }
    const_pointer address(const_reference _ref) const noexcept { return &_ref; }

    template <class Other, class... Args>
    void construct(Other* _ptr, Args&&... _args) {
        ::new (static_cast<void*>(_ptr)) Other(std::forward<Args>(_args)...);
    }

    template <class Other>
    void destroy(Other* _ptr) {
        (void)_ptr;
        _ptr->~Other();
    }

    // Allocates array of _Count elements.
    pointer allocate(size_t _count) noexcept {
        // Makes sure to a use c like allocator, to avoid duplicated constructor
        // calls.
        return reinterpret_cast<pointer>(
                memory::default_allocator()->Allocate(sizeof(value_type) * _count, alignof(value_type)));
    }

    // Deallocates object at _Ptr, ignores size.
    void deallocate(pointer _ptr, size_type) noexcept { memory::default_allocator()->Deallocate(_ptr); }

    [[nodiscard]] size_type max_size() const noexcept { return (~size_type(0)) / sizeof(value_type); }
};

// Tests for allocator equality (always true).
template <class Ty, class Other>
inline bool operator==(const StdAllocator<Ty>&, const StdAllocator<Other>&) noexcept {
    return true;
}

// Tests for allocator inequality (always false).
template <class Ty, class Other>
inline bool operator!=(const StdAllocator<Ty>&, const StdAllocator<Other>&) noexcept {
    return false;
}
}  // namespace vox
