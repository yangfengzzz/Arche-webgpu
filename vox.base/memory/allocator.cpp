//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/memory/allocator.h"

#include <atomic>
#include <cassert>
#include <cstdlib>

namespace vox::memory {

namespace {
struct Header {
    void* unaligned;
    size_t size;
};
}  // namespace

// Implements the basic heap allocator.
// Will trace allocation count and assert in case of a memory leak.
class HeapAllocator : public Allocator {
public:
    HeapAllocator() { allocation_count_.store(0); }
    ~HeapAllocator() override { assert(allocation_count_.load() == 0 && "Memory leak detected"); }

protected:
    void* Allocate(size_t _size, size_t _alignment) override {
        // Allocates enough memory to store the header + required alignment space.
        const size_t to_allocate = _size + sizeof(Header) + _alignment - 1;
        char* unaligned = reinterpret_cast<char*>(malloc(to_allocate));
        if (!unaligned) {
            return nullptr;
        }
        char* aligned = vox::align(unaligned + sizeof(Header), _alignment);
        assert(aligned + _size <= unaligned + to_allocate);  // Don't overrun.
        // Set the header
        auto* header = reinterpret_cast<Header*>(aligned - sizeof(Header));
        assert(reinterpret_cast<char*>(header) >= unaligned);
        header->unaligned = unaligned;
        header->size = _size;
        // Allocation's succeeded.
        ++allocation_count_;
        return aligned;
    }

    void Deallocate(void* _block) override {
        if (_block) {
            auto* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(_block) - sizeof(Header));
            free(header->unaligned);
            // Deallocation completed.
            --allocation_count_;
        }
    }

private:
    // Internal allocation count used to track memory leaks.
    // Should equal 0 at destruction time.
    std::atomic_int allocation_count_{};
};

namespace {
// Instantiates the default heap allocator->
HeapAllocator g_heap_allocator;

// Instantiates the default heap allocator pointer.
Allocator* g_default_allocator = &g_heap_allocator;
}  // namespace

// Implements default allocator accessor.
Allocator* default_allocator() { return g_default_allocator; }

// Implements default allocator setter.
Allocator* SetDefaultAllocator(Allocator* _allocator) {
    Allocator* previous = g_default_allocator;
    g_default_allocator = _allocator;
    return previous;
}
}  // namespace vox::memory
