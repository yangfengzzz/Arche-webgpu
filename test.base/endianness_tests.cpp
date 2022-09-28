//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <gtest/gtest.h>

#include "vox.base/endianness.h"

TEST(NativeEndianness, Endianness) {
// Uses pre-defined macro to check know endianness.
// Endianness detection does not rely on this as this is not standard, but it
// helps with testing.

// x86 and x86-64 Little endian processors.
#if defined(i386) || defined(__i386__) || /*GNU C*/         \
        defined(__X86__) ||               /*MinGW32*/       \
        defined(__x86_64__) ||            /*Intel C/C++*/   \
        defined(_M_IX86) ||               /*Visual Studio*/ \
        defined(_M_X64)                   /*Visual Studio*/
    EXPECT_EQ(vox::getNativeEndianness(), vox::kLittleEndian);
#endif

// PowerPc Big endian processors.
#if defined(__POWERPC__) || defined(__ppc__) || defined(__powerpc__) || /*GNU C*/ \
        defined(__IA64__) || defined(__ia64__) || defined(_M_PPC)       /*Visual Studio*/
    EXPECT_EQ(vox::getNativeEndianness(), vox::kBigEndian);
#endif

// Itanium Big endian processors.
#if defined(_IA64) ||       /*GNU C*/         \
        defined(_M_IA64) || /*Visual Studio*/ \
        defined(_M_IA64)    /*Intel C/C++*/
    EXPECT_EQ(vox::getNativeEndianness(), vox::kBigEndian);
#endif
}

TEST(Swap, Endianness) {
    {  // 1 byte swapping.
        uint8_t uo = 0x46;
        EXPECT_EQ(vox::endianSwap(uo), 0x46);
    }
    {  // 1 byte array swapping.
        uint8_t uo[] = {0x46, 0x58};
        vox::endianSwap(uo, VOX_ARRAY_SIZE(uo));
        EXPECT_EQ(uo[0], 0x46);
        EXPECT_EQ(uo[1], 0x58);
    }
    {  // 2 bytes swapping.
        uint16_t uo = 0x4699;
        EXPECT_EQ(vox::endianSwap(uo), 0x9946);
    }
    {  // 2 bytes array swapping.
        uint16_t uo[] = {0x4699, 0x5814};
        vox::endianSwap(uo, VOX_ARRAY_SIZE(uo));
        EXPECT_EQ(uo[0], 0x9946);
        EXPECT_EQ(uo[1], 0x1458);
    }
    {  // 4 bytes swapping.
        uint32_t uo = 0x46992715;
        EXPECT_EQ(vox::endianSwap(uo), 0x15279946u);
    }
    {  // 2 bytes array swapping.
        uint32_t uo[] = {0x46992715, 0x58142611};
        vox::endianSwap(uo, VOX_ARRAY_SIZE(uo));
        EXPECT_EQ(uo[0], 0x15279946u);
        EXPECT_EQ(uo[1], 0x11261458u);
    }
    {  // 8 bytes swapping.
        uint64_t uo = 0x4699271511190417ull;
        EXPECT_EQ(vox::endianSwap(uo), 0x1704191115279946ull);
    }
    {  // 8 bytes array swapping.
        uint64_t uo[] = {0x4699271511190417ull, 0x5814264669080735ull};
        vox::endianSwap(uo, VOX_ARRAY_SIZE(uo));
        EXPECT_EQ(uo[0], 0x1704191115279946ull);
        EXPECT_EQ(uo[1], 0x3507086946261458ull);
    }
}
