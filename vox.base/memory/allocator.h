//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstddef>
#include <new>
#include <utility>

#include "vox.base/macros.h"

namespace vox {
namespace memory {

// Forwards declare Allocator class.
class Allocator;

// Defines the default allocator accessor.
VOX_BASE_DLL Allocator* default_allocator();

// Set the default allocator, used for all dynamic allocation inside vox.
// Returns current memory allocator, such that in can be restored if needed.
VOX_BASE_DLL Allocator* SetDefaultAllocator(Allocator* _allocator);

// Defines an abstract allocator class.
// Implements helper methods to allocate/deallocate POD typed objects instead of
// raw memory.
// Implements New and Delete function to allocate C++ objects, as a replacement
// of new and delete operators.
class VOX_BASE_DLL Allocator {
public:
    // Default virtual destructor.
    virtual ~Allocator() = default;

    // Next functions are the pure virtual functions that must be implemented by
    // allocator concrete classes.

    // Allocates _size bytes on the specified _alignment boundaries.
    // Allocate function conforms with standard malloc function specifications.
    virtual void* Allocate(size_t _size, size_t _alignment) = 0;

    // Frees a block that was allocated with Allocate or Reallocate.
    // Argument _block can be nullptr.
    // Deallocate function conforms with standard free function specifications.
    virtual void Deallocate(void* _block) = 0;
};
}  // namespace memory

// vox replacement for c++ operator new with, used to allocate with a
// vox::memory::Allocator. Delete must be used to deallocate such object.
// It can be used for constructor with no argument:
// Type* object = New<Type>();
// or any number of argument:
// Type* object = New<Type>(1,2,3,4);
template <typename Ty, typename... Args>
Ty* New(Args&&... _args) {
    void* alloc = memory::default_allocator()->Allocate(sizeof(Ty), alignof(Ty));
    return new (alloc) Ty(std::forward<Args>(_args)...);
}

template <typename Ty>
void Delete(Ty* _object) {
    if (_object) {
        // Prevents from false "unreferenced parameter" warning when _Ty has no
        // explicit destructor.
        (void)_object;
        _object->~Ty();
        memory::default_allocator()->Deallocate(_object);
    }
}

}  // namespace vox