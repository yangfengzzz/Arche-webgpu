//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/io/archive.h"

#include <cstdint>
#include <cstring>

#include "gtest/gtest.h"

#include "test.base/gtest_helper.h"

#include "archive_tests_objects.h"

TEST(Error, Archive) {
  {  // Invalid nullptr stream.
    EXPECT_ASSERTION(
        void(vox::io::OArchive(nullptr, vox::getNativeEndianness())),
        "valid opened stream");
    EXPECT_ASSERTION(void(vox::io::IArchive(nullptr)), "valid opened stream");
  }
  {  // Invalid not opened streams.
    vox::io::File stream("root_that_does_not_exist:/file.vox", "r");
    EXPECT_ASSERTION(
        void(vox::io::OArchive(&stream, vox::getNativeEndianness())),
        "valid opened stream");
    EXPECT_ASSERTION(void(vox::io::IArchive(&stream)), "valid opened stream");
  }
}

TEST(Primitives, Archive) {
  for (int e = 0; e < 2; ++e) {
    vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;

    vox::io::MemoryStream stream;
    ASSERT_TRUE(stream.opened());

    // Write primitive types.
    vox::io::OArchive o(&stream, endianess);
    const int8_t i8o = 46;
    o << i8o;
    const uint8_t ui8o = 46;
    o << ui8o;
    const int16_t i16o = 46;
    o << i16o;
    const uint16_t ui16o = 46;
    o << ui16o;
    const int32_t i32o = 46;
    o << i32o;
    const uint32_t ui32o = 46;
    o << ui32o;
    const int64_t i64o = 46;
    o << i64o;
    const uint64_t ui64o = 46;
    o << ui64o;
    const bool bo = true;
    o << bo;
    const float fo = 46.f;
    o << fo;

    // Read primitive types.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);
    int8_t i8i;
    i >> i8i;
    EXPECT_EQ(i8i, i8o);
    uint8_t ui8i;
    i >> ui8i;
    EXPECT_EQ(ui8i, ui8o);
    int16_t i16i;
    i >> i16i;
    EXPECT_EQ(i16i, i16o);
    uint16_t ui16i;
    i >> ui16i;
    EXPECT_EQ(ui16i, ui16o);
    int32_t i32i;
    i >> i32i;
    EXPECT_EQ(i32i, i32o);
    uint32_t ui32i;
    i >> ui32i;
    EXPECT_EQ(ui32i, ui32o);
    int64_t i64i;
    i >> i64i;
    EXPECT_EQ(i64i, i64o);
    uint64_t ui64i;
    i >> ui64i;
    EXPECT_EQ(ui64i, ui64o);
    bool bi;
    i >> bi;
    EXPECT_EQ(bi, bo);
    float fi;
    i >> fi;
    EXPECT_EQ(fi, fo);
  }
}

TEST(PrimitiveArrays, Archive) {
  for (int e = 0; e < 2; ++e) {
    vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;

    vox::io::MemoryStream stream;
    ASSERT_TRUE(stream.opened());

    // Write primitive types.
    vox::io::OArchive o(&stream, endianess);
    const int8_t i8o[] = {46, 26, 14, 58, 99, 27};
    o << vox::io::MakeArray(i8o);
    const uint8_t ui8o[] = {46, 26, 14, 58, 99, 27};
    o << vox::io::MakeArray(ui8o);
    const int16_t i16o[] = {46, 26, 14, 58, 99, 27};
    o << vox::io::MakeArray(i16o);
    const uint16_t ui16o[] = {46, 26, 14, 58, 99, 27};
    o << vox::io::MakeArray(ui16o);
    const int32_t i32o[] = {46, 26, 14, 58, 99, 27};
    o << vox::io::MakeArray(i32o);
    const uint32_t ui32o[] = {46, 26, 14, 58, 99, 27};
    o << vox::io::MakeArray(ui32o);
    const int64_t i64o[] = {46, 26, 14, 58, 99, 27};
    o << vox::io::MakeArray(i64o);
    const uint64_t ui64o[] = {46, 26, 14, 58, 99, 27};
    o << vox::io::MakeArray(ui64o);
    const bool bo[] = {true, false, true};
    o << vox::io::MakeArray(bo);
    const float fo[] = {46.f, 26.f, 14.f, 58.f, 99.f, 27.f};
    o << vox::io::MakeArray(fo);
    const uint32_t* po_null = nullptr;
    o << vox::io::MakeArray(po_null, 0);
    const vox::span<const float> rfo(fo);
    o << vox::io::MakeArray(rfo);

    // Read primitive types.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);
    int8_t i8i[VOX_ARRAY_SIZE(i8o)];
    i >> vox::io::MakeArray(i8i);
    EXPECT_EQ(std::memcmp(i8i, i8o, sizeof(i8o)), 0);
    uint8_t ui8i[VOX_ARRAY_SIZE(ui8o)];
    i >> vox::io::MakeArray(ui8i);
    EXPECT_EQ(std::memcmp(ui8i, ui8o, sizeof(ui8o)), 0);
    int16_t i16i[VOX_ARRAY_SIZE(i16o)];
    i >> vox::io::MakeArray(i16i);
    EXPECT_EQ(std::memcmp(i16i, i16o, sizeof(i16o)), 0);
    uint16_t ui16i[VOX_ARRAY_SIZE(ui16o)];
    i >> vox::io::MakeArray(ui16i);
    EXPECT_EQ(std::memcmp(ui16i, ui16o, sizeof(ui16o)), 0);
    int32_t i32i[VOX_ARRAY_SIZE(i32o)];
    i >> vox::io::MakeArray(i32i);
    EXPECT_EQ(std::memcmp(i32i, i32o, sizeof(i32o)), 0);
    uint32_t ui32i[VOX_ARRAY_SIZE(ui32o)];
    i >> vox::io::MakeArray(ui32i);
    EXPECT_EQ(std::memcmp(ui32i, ui32o, sizeof(ui32o)), 0);
    int64_t i64i[VOX_ARRAY_SIZE(i64o)];
    i >> vox::io::MakeArray(i64i);
    EXPECT_EQ(std::memcmp(i64i, i64o, sizeof(i64o)), 0);
    uint64_t ui64i[VOX_ARRAY_SIZE(ui64o)];
    i >> vox::io::MakeArray(ui64i);
    EXPECT_EQ(std::memcmp(ui64i, ui64o, sizeof(ui64o)), 0);
    bool bi[VOX_ARRAY_SIZE(bo)];
    i >> vox::io::MakeArray(bi);
    EXPECT_EQ(std::memcmp(bi, bo, sizeof(bo)), 0);
    float fi[VOX_ARRAY_SIZE(fo)];
    i >> vox::io::MakeArray(fi);
    EXPECT_EQ(std::memcmp(fi, fo, sizeof(fo)), 0);
    uint32_t* pi_null = nullptr;
    i >> vox::io::MakeArray(pi_null, 0);
    float fi2[VOX_ARRAY_SIZE(fo)];
    vox::span<float> rfi(fi2);
    i >> vox::io::MakeArray(rfi);
    EXPECT_EQ(std::memcmp(rfi.data(), fo, sizeof(fo)), 0);
  }
}

TEST(Class, Archive) {
  for (int e = 0; e < 2; ++e) {
    vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;

    vox::io::MemoryStream stream;
    ASSERT_TRUE(stream.opened());

    // Write classes.
    vox::io::OArchive o(&stream, endianess);
    const Intrusive oi(46);
    o << oi;
    const Intrusive oi_mutable(46);
    o << oi_mutable;
    const Extrusive oe = {58};
    o << oe;
    const Extrusive oe_mutable = {58};
    o << oe_mutable;

    // Read classes.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);
    Intrusive ii;
    i >> ii;
    EXPECT_EQ(ii.i, oi.i);
    Intrusive ii_mutable;
    i >> ii_mutable;
    EXPECT_EQ(ii_mutable.i, oi_mutable.i);
    Extrusive ie;
    i >> ie;
    EXPECT_EQ(ie.i, oe.i);
    Extrusive ie_mutable;
    i >> ie_mutable;
    EXPECT_EQ(ie_mutable.i, oe_mutable.i);
  }
}

TEST(ClassArrays, Archive) {
  for (int e = 0; e < 2; ++e) {
    vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;

    vox::io::MemoryStream stream;
    ASSERT_TRUE(stream.opened());

    // Write classes.
    vox::io::OArchive o(&stream, endianess);
    const Intrusive oi[12];
    o << vox::io::MakeArray(oi);
    const Extrusive oe[] = {{46}, {58}, {14}, {26}, {99}};
    o << vox::io::MakeArray(oe);

    // Read classes.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);
    Intrusive ii[VOX_ARRAY_SIZE(oi)];
    i >> vox::io::MakeArray(ii);
    EXPECT_EQ(std::memcmp(oi, ii, sizeof(oi)), 0);
    Extrusive ie[VOX_ARRAY_SIZE(oe)];
    i >> vox::io::MakeArray(ie);
    EXPECT_EQ(std::memcmp(oe, ie, sizeof(oe)), 0);
  }
}

TEST(Tag, Archive) {
  vox::io::MemoryStream stream;
  ASSERT_TRUE(stream.opened());

  // Writes to archive.
  vox::io::OArchive o(&stream, vox::getNativeEndianness());
  Tagged1 ot;
  o << ot;

  // Reads from archive.
  stream.Seek(0, vox::io::Stream::kSet);
  vox::io::IArchive i(&stream);

  // Tests and reads a wrong object (different tag).
  VOX_IF_DEBUG(Tagged2 it2);
  EXPECT_FALSE(i.TestTag<Tagged2>());
  EXPECT_ASSERTION(i >> it2, "Type tag does not match archive content.");

  // Reads the right object (different tag).
  Tagged1 it1;
  EXPECT_TRUE(i.TestTag<Tagged1>());
  EXPECT_NO_FATAL_FAILURE(i >> it1);
}

TEST(TagEOF, Archive) {
  vox::io::MemoryStream stream;
  ASSERT_TRUE(stream.opened());

  // Writes to archive n elements.
  const int n_writes = 10;
  vox::io::OArchive o(&stream, vox::getNativeEndianness());
  for (int i = 0; i < n_writes; ++i) {
    Tagged1 ot;
    o << ot;
  }

  // Reads from archive.
  stream.Seek(0, vox::io::Stream::kSet);
  vox::io::IArchive i(&stream);

  EXPECT_FALSE(i.TestTag<Tagged2>());

  // Tests and reads all objects.
  int n_read = 0;
  while (i.TestTag<Tagged1>()) {
    Tagged1 it;
    i >> it;
    ++n_read;
  }
  EXPECT_EQ(n_read, n_writes);

  EXPECT_FALSE(i.TestTag<Tagged2>());
}
