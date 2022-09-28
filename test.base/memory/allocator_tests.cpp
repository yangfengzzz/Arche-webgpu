//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "vox.base/memory/allocator.h"

TEST(Allocate, Memory) {
    void* p = vox::memory::default_allocator()->Allocate(12, 1024);
    EXPECT_TRUE(p != nullptr);
    EXPECT_TRUE(vox::isAligned(p, 1024));

    // Fills allocated memory.
    memset(p, 0, 12);

    vox::memory::default_allocator()->Deallocate(p);
}

TEST(MallocCompliance, Memory) {
    {  // Allocating 0 byte gives a valid pointer.
        void* p = vox::memory::default_allocator()->Allocate(0, 1024);
        EXPECT_TRUE(p != nullptr);
        vox::memory::default_allocator()->Deallocate(p);
    }

    {  // Freeing of a nullptr pointer is valid.
        vox::memory::default_allocator()->Deallocate(nullptr);
    }
}

struct AlignedInts {
    AlignedInts() {
        for (int i = 0; i < array_size; ++i) {
            array[i] = i;
        }
    }

    AlignedInts(int _i0) {
        array[0] = _i0;
        for (int i = 1; i < array_size; ++i) {
            array[i] = i;
        }
    }

    AlignedInts(int _i0, int _i1) {
        array[0] = _i0;
        array[1] = _i1;
        for (int i = 2; i < array_size; ++i) {
            array[i] = i;
        }
    }

    AlignedInts(int _i0, int _i1, int _i2) {
        array[0] = _i0;
        array[1] = _i1;
        array[2] = _i2;
        for (int i = 3; i < array_size; ++i) {
            array[i] = i;
        }
    }

    static const int array_size = 517;
    alignas(64) int array[array_size]{};
};

TEST(NewDelete, Memory) {
    auto* ai0 = vox::New<AlignedInts>();
    ASSERT_TRUE(ai0 != nullptr);
    for (int i = 0; i < AlignedInts::array_size; ++i) {
        EXPECT_EQ(ai0->array[i], i);
    }
    vox::Delete(ai0);

    auto* ai1 = vox::New<AlignedInts>(46);
    ASSERT_TRUE(ai1 != nullptr);
    EXPECT_EQ(ai1->array[0], 46);
    for (int i = 1; i < AlignedInts::array_size; ++i) {
        EXPECT_EQ(ai1->array[i], i);
    }
    vox::Delete(ai1);

    auto* ai2 = vox::New<AlignedInts>(46, 69);
    ASSERT_TRUE(ai2 != nullptr);
    EXPECT_EQ(ai2->array[0], 46);
    EXPECT_EQ(ai2->array[1], 69);
    for (int i = 2; i < AlignedInts::array_size; ++i) {
        EXPECT_EQ(ai2->array[i], i);
    }
    vox::Delete(ai2);

    auto* ai3 = vox::New<AlignedInts>(46, 69, 58);
    ASSERT_TRUE(ai3 != nullptr);
    EXPECT_EQ(ai3->array[0], 46);
    EXPECT_EQ(ai3->array[1], 69);
    EXPECT_EQ(ai3->array[2], 58);
    for (int i = 3; i < AlignedInts::array_size; ++i) {
        EXPECT_EQ(ai3->array[i], i);
    }
    vox::Delete(ai3);
}

class TestAllocator : public vox::memory::Allocator {
public:
    TestAllocator() : hard_coded_address_(&hard_coded_address_) {}

    [[nodiscard]] void* hard_coded_address() const { return hard_coded_address_; }

private:
    void* Allocate(size_t _size, size_t _alignment) override {
        (void)_size;
        (void)_alignment;
        return hard_coded_address_;
    }
    void Deallocate(void* _block) override { (void)_block; }

    void* hard_coded_address_;
};

TEST(AllocatorOverride, Memory) {
    TestAllocator test_allocator;
    vox::memory::Allocator* previous = vox::memory::SetDefaultAllocator(&test_allocator);
    vox::memory::Allocator* current = vox::memory::default_allocator();

    void* alloc = current->Allocate(1, 1);
    EXPECT_EQ(alloc, test_allocator.hard_coded_address());
    current->Deallocate(alloc);

    EXPECT_EQ(vox::memory::SetDefaultAllocator(previous), current);
}
