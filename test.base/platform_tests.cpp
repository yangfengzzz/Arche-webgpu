//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cassert>
#include <climits>
#include <cstdint>

#include "gtest/gtest.h"
#include "vox.base/macros.h"
#include "vox.base/string_utils.h"

TEST(StaticAssertion, Platform) {
    static_assert(2 == 2, "Must compile.");
    // static_assert(1 == 2, "Must not compile.);
}

namespace {
// Declares a structure that should have at least 8 bytes aligned.
struct Misc {
    double d;
    char c;
    int i;
};

// Declares an aligned structure in order to test VOX_ALIGN and AlignOf.
struct Aligned {
    alignas(128) char c;
};
}  // namespace

TEST(Alignment, Platform) {
    static_assert(alignof(char) == 1, "Unexpected type size");
    static_assert(alignof(double) == 8, "Unexpected type size");
    static_assert(alignof(Misc) == 8, "Unexpected type size");
    static_assert(alignof(Aligned) == 128, "Unexpected type size");

    Aligned aligned{};
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&aligned) & (128 - 1), 0u);
}

TEST(IntegerAlignment, Platform) {
    {
        short s = 0x1234;
        int aligned_s = vox::align(s, 128);
        EXPECT_TRUE(aligned_s == 0x1280);
        EXPECT_TRUE(vox::isAligned(aligned_s, 128));
    }

    {
        int i = 0x00a01234;
        int aligned_i = vox::align(i, 1024);
        EXPECT_TRUE(aligned_i == 0x00a01400);
        EXPECT_TRUE(vox::isAligned(aligned_i, 1024));
    }
}

TEST(PointerAlignment, Platform) {
    void* p = reinterpret_cast<void*>(0x00a01234);
    void* aligned_p = vox::align(p, 1024);
    EXPECT_TRUE(aligned_p == reinterpret_cast<void*>(0x00a01400));
    EXPECT_TRUE(vox::isAligned(aligned_p, 1024));
}

TEST(TypeSize, Platform) {
    // Checks sizes.
    static_assert(CHAR_BIT == 8, "Unexpected type size");
    static_assert(sizeof(vox::byte) == 1, "Unexpected type size");
    static_assert(sizeof(int8_t) == 1, "Unexpected type size");
    static_assert(sizeof(uint8_t) == 1, "Unexpected type size");
    static_assert(sizeof(int16_t) == 2, "Unexpected type size");
    static_assert(sizeof(uint16_t) == 2, "Unexpected type size");
    static_assert(sizeof(int32_t) == 4, "Unexpected type size");
    static_assert(sizeof(uint32_t) == 4, "Unexpected type size");
    static_assert(sizeof(int64_t) == 8, "Unexpected type size");
    static_assert(sizeof(uint64_t) == 8, "Unexpected type size");
    static_assert(sizeof(intptr_t) == sizeof(int*), "Unexpected type size");
    static_assert(sizeof(uintptr_t) == sizeof(unsigned int*), "Unexpected type size");

    // Checks signs. Right shift maintains a sign bit for signed types, and fills
    // with 0 for unsigned types.
    static_assert((int8_t(-1) >> 1) == -1, "Unexpected type sign");
    static_assert((int16_t(-1) >> 1) == -1, "Unexpected type sign");
    static_assert((int32_t(-1) >> 1) == -1, "Unexpected type sign");
    static_assert((int64_t(-1) >> 1) == -1, "Unexpected type sign");
    static_assert((vox::byte(-1) >> 1) == 0x7f, "Unexpected type sign");
    static_assert((uint8_t(-1) >> 1) == 0x7f, "Unexpected type sign");
    static_assert((uint16_t(-1) >> 1) == 0x7fff, "Unexpected type sign");
    static_assert((uint32_t(-1) >> 1) == 0x7fffffff, "Unexpected type sign");
    static_assert((uint64_t(-1) >> 1) == 0x7fffffffffffffffLL, "Unexpected type sign");

    // Assumes that an "int" is at least 32 bits.
    static_assert(sizeof(int) >= 4, "Unexpected type size");

    // "char" type is used to manipulate bytes. Can be signed or unsigned.
    static_assert(sizeof(char) == 1, "Unexpected type size");
}

TEST(DebudNDebug, Platform) {
    VOX_IF_DEBUG(assert(true));
    VOX_IF_NDEBUG(assert(false));
}

TEST(ArraySize, Platform) {
    int ai[46];
    (void)ai;
    static_assert(VOX_ARRAY_SIZE(ai) == 46, "Unexpected array size");

    char ac[] = "forty six";
    (void)ac;
    static_assert(VOX_ARRAY_SIZE(ac) == 10, "Unexpected array size");
}

TEST(StrMatch, Platform) {
    EXPECT_TRUE(vox::strmatch("a", "a"));
    EXPECT_FALSE(vox::strmatch("a", "b"));
    EXPECT_TRUE(vox::strmatch("a", "a*"));
    EXPECT_FALSE(vox::strmatch("a", "a?"));
    EXPECT_TRUE(vox::strmatch("ab", "a?"));
    EXPECT_TRUE(vox::strmatch("ab", "?b"));
    EXPECT_FALSE(vox::strmatch("ab", "a"));
    EXPECT_TRUE(vox::strmatch("ab", "ab"));
    EXPECT_TRUE(vox::strmatch("", ""));
    EXPECT_TRUE(vox::strmatch("", "*"));
    EXPECT_FALSE(vox::strmatch("", "?"));
    EXPECT_FALSE(vox::strmatch("ab", ""));
    EXPECT_FALSE(vox::strmatch("ab", "?"));
    EXPECT_TRUE(vox::strmatch("ab", "??"));
    EXPECT_TRUE(vox::strmatch("a*b", "a*b"));
    EXPECT_TRUE(vox::strmatch("a*b", "a?b"));
    EXPECT_TRUE(vox::strmatch("ab", "ab*"));
    EXPECT_TRUE(vox::strmatch("ab", "a*"));
    EXPECT_TRUE(vox::strmatch("ab", "*b"));
    EXPECT_TRUE(vox::strmatch("ab", "a*b"));
    EXPECT_TRUE(vox::strmatch("acb", "a*b"));
    EXPECT_FALSE(vox::strmatch("abc", "a*b"));
    EXPECT_TRUE(vox::strmatch("abcdef", "a*c*"));
    EXPECT_TRUE(vox::strmatch("abc.def", "a*c.*"));
    EXPECT_TRUE(vox::strmatch("abc.def", "abc.def"));
    EXPECT_TRUE(vox::strmatch("abc.def", "abc.def***"));
    EXPECT_FALSE(vox::strmatch("abc.def", "abc.def?"));
    EXPECT_TRUE(vox::strmatch("abc.def", "abc?def"));
    EXPECT_TRUE(vox::strmatch("abc.def", "a*c?*"));
    EXPECT_TRUE(vox::strmatch("abc.def", "a*.*"));
    EXPECT_TRUE(vox::strmatch("abc.def", "a*c.*e?"));
    EXPECT_TRUE(vox::strmatch("abc.def", "*"));
    EXPECT_TRUE(vox::strmatch("abc.def", "*.*"));
    EXPECT_TRUE(vox::strmatch("abc.def", "???.???"));
    EXPECT_FALSE(vox::strmatch("abc.def", "??.???"));
    EXPECT_TRUE(vox::strmatch("abc.def", "*??.???"));
    EXPECT_TRUE(vox::strmatch("abc.def", "*??.??*"));
    EXPECT_TRUE(
            vox::strmatch("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                          "aaaaaaaaaaaaaaa",
                          "*a*??????a?????????a???????????????"));
}
