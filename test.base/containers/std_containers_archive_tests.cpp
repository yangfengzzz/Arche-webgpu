//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <algorithm>

#include "gtest/gtest.h"
#include "vox.base/containers/array_archive.h"
#include "vox.base/containers/string_archive.h"
#include "vox.base/containers/vector_archive.h"
#include "vox.base/io/archive.h"

TEST(string, Archive) {
  for (int e = 0; e < 2; ++e) {
    vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;

    vox::io::MemoryStream stream;
    ASSERT_TRUE(stream.opened());

    // Writes.
    vox::io::OArchive o(&stream, endianess);
    vox::string empty_o;
    o << empty_o;

    vox::string small_o("Forty-six");
    o << small_o;

    vox::string big_o(
        "Forty-six is a Wedderburn-Etherington number, an "
        "enneagonal number and a centered triangular number. It is the sum of "
        "the totient function for the first twelve integers. 46 is the largest "
        "even integer that can't be expressed as a sum of two abundant numbers."
        "46 is the 16th semiprime. 46 is the third semiprime with a semiprime"
        "aliquot sum. The aliquot sequence of 46 is (46,26,16,15,9,4,3,1,0)."
        "Since it is possible to find sequences of 46 consecutive integers "
        "such that each inner member shares a factor with either the first or "
        "the last member, 46 is an Erdos-Woods number.");
    o << big_o;

    // Rewrite for the string reuse test.
    vox::string reuse_o("Forty-six");
    o << reuse_o;

    // Reads.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    vox::string empty_i;
    i >> empty_i;
    EXPECT_STREQ(empty_o.c_str(), empty_i.c_str());

    vox::string small_i;
    i >> small_i;
    EXPECT_STREQ(small_o.c_str(), small_i.c_str());

    vox::string big_i;
    i >> big_i;
    EXPECT_STREQ(big_o.c_str(), big_i.c_str());

    vox::string reuse_i("already used string");
    i >> reuse_i;
    EXPECT_STREQ(reuse_o.c_str(), reuse_i.c_str());
  }
}

TEST(Vector, Archive) {
  for (int e = 0; e < 2; ++e) {
    vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;

    vox::io::MemoryStream stream;
    ASSERT_TRUE(stream.opened());

    // Writes.
    vox::io::OArchive o(&stream, endianess);
    vox::vector<int> empty_o;
    o << empty_o;

    vox::vector<int> small_o(5);
    std::generate(small_o.begin(), small_o.end(), std::rand);
    o << small_o;

    vox::vector<int> big_o(1263);
    std::generate(big_o.begin(), big_o.end(), std::rand);
    o << big_o;

    // Rewrite for the Vector reuse test.
    vox::vector<int> reuse_o(46);
    std::generate(reuse_o.begin(), reuse_o.end(), std::rand);
    o << reuse_o;

    // Reads.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    vox::vector<int> empty_i;
    i >> empty_i;
    EXPECT_EQ(empty_i.size(), 0u);

    vox::vector<int> small_i;
    i >> small_i;
    EXPECT_EQ(small_o.size(), small_i.size());
    for (size_t j = 0; j < small_i.size(); ++j) {
      EXPECT_EQ(small_o[j], small_i[j]);
    }

    vox::vector<int> big_i;
    i >> big_i;
    EXPECT_EQ(big_o.size(), big_i.size());
    for (size_t j = 0; j < big_i.size(); ++j) {
      EXPECT_EQ(big_o[j], big_i[j]);
    }

    vox::vector<int> reuse_i(3);
    std::generate(reuse_i.begin(), reuse_i.end(), std::rand);
    i >> reuse_i;
    EXPECT_EQ(reuse_o.size(), reuse_i.size());
    for (size_t j = 0; j < reuse_i.size(); ++j) {
      EXPECT_EQ(reuse_o[j], reuse_i[j]);
    }
  }
}

TEST(Array, Archive) {
  for (int e = 0; e < 2; ++e) {
    vox::Endianness endianess = e == 0 ? vox::kBigEndian : vox::kLittleEndian;

    vox::io::MemoryStream stream;
    ASSERT_TRUE(stream.opened());

    // Writes.
    vox::io::OArchive o(&stream, endianess);
    vox::array<int, 0> empty_o;
    o << empty_o;

    vox::array<int, 5> array_o{{0, 1, 2, 3, 4}};
    o << array_o;

    // Rewrite for the Vector reuse test.
    vox::array<int, 5> reuse_o{{5, 6, 7, 8, 9}};
    o << reuse_o;

    // Reads.
    stream.Seek(0, vox::io::Stream::kSet);
    vox::io::IArchive i(&stream);

    vox::array<int, 0> empty_i;
    i >> empty_i;
    EXPECT_EQ(empty_i.size(), 0u);

    vox::array<int, 5> array_i;
    i >> array_i;
    EXPECT_EQ(array_o.size(), array_i.size());
    for (size_t j = 0; j < array_i.size(); ++j) {
      EXPECT_EQ(array_o[j], array_i[j]);
    }

    i >> array_i;
    EXPECT_EQ(reuse_o.size(), array_i.size());
    for (size_t j = 0; j < array_i.size(); ++j) {
      EXPECT_EQ(reuse_o[j], array_i[j]);
    }
  }
}
