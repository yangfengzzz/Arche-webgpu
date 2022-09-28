//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <gtest/gtest.h>

#include "test.base/gtest_helper.h"
#include "vox.base/span.h"

TEST(Span, Platform) {
    const size_t kSize = 46;
    int i = 20;
    int ai[kSize];
    const size_t array_size = VOX_ARRAY_SIZE(ai);

    vox::span<int> empty;
    EXPECT_TRUE(empty.begin() == nullptr);
    EXPECT_EQ(empty.size(), 0u);
    EXPECT_EQ(empty.size_bytes(), 0u);

    EXPECT_ASSERTION(empty[46], "Index out of range.");

    vox::span<int> single(i);
    EXPECT_TRUE(single.begin() == &i);
    EXPECT_EQ(single.size(), 1u);
    EXPECT_EQ(single.size_bytes(), sizeof(i));

    EXPECT_ASSERTION(single[46], "Index out of range.");

    vox::span<int> cs1(ai, ai + array_size);
    EXPECT_EQ(cs1.begin(), ai);
    EXPECT_EQ(cs1.size(), array_size);
    EXPECT_EQ(cs1.size_bytes(), sizeof(ai));

    // Re-inint
    vox::span<int> reinit;
    reinit = ai;
    EXPECT_EQ(reinit.begin(), ai);
    EXPECT_EQ(reinit.size(), array_size);
    EXPECT_EQ(reinit.size_bytes(), sizeof(ai));

    // Clear
    reinit = {};
    EXPECT_EQ(reinit.size(), 0u);
    EXPECT_EQ(reinit.size_bytes(), 0u);

    cs1[12] = 46;
    EXPECT_EQ(cs1[12], 46);
    EXPECT_ASSERTION(cs1[46], "Index out of range.");

    vox::span<int> cs2(ai, array_size);
    EXPECT_EQ(cs2.begin(), ai);
    EXPECT_EQ(cs2.size(), array_size);
    EXPECT_EQ(cs2.size_bytes(), sizeof(ai));

    vox::span<int> carray(ai);
    EXPECT_EQ(carray.begin(), ai);
    EXPECT_EQ(carray.size(), array_size);
    EXPECT_EQ(carray.size_bytes(), sizeof(ai));

    vox::span<int> copy(cs2);
    EXPECT_EQ(cs2.begin(), copy.begin());
    EXPECT_EQ(cs2.size_bytes(), copy.size_bytes());

    vox::span<const int> const_copy(cs2);
    EXPECT_EQ(cs2.begin(), const_copy.begin());
    EXPECT_EQ(cs2.size_bytes(), const_copy.size_bytes());

    EXPECT_EQ(cs2[12], 46);
    EXPECT_ASSERTION(cs2[46], "Index out of range.");

    // Invalid range
    EXPECT_ASSERTION(vox::span<int>(ai, ai - array_size), "Invalid range.");
}

TEST(SpanAsBytes, Platform) {
    const size_t kSize = 46;
    int ai[kSize];
    {
        vox::span<int> si(ai);
        EXPECT_EQ(si.size(), kSize);

        vox::span<const vox::byte> ab = as_bytes(si);
        EXPECT_EQ(ab.size(), kSize * sizeof(int));

        vox::span<vox::byte> awb = as_writable_bytes(si);
        EXPECT_EQ(awb.size(), kSize * sizeof(int));
    }

    {  // mutable char
        char ac[kSize];
        vox::span<char> sc(ac);
        EXPECT_EQ(sc.size(), kSize);

        vox::span<const vox::byte> ab = as_bytes(sc);
        EXPECT_EQ(ab.size(), sc.size());

        vox::span<vox::byte> awbc = as_writable_bytes(sc);
        EXPECT_EQ(awbc.size(), sc.size());
    }
    {  // const
        vox::span<const int> si(ai);
        EXPECT_EQ(si.size(), kSize);

        vox::span<const vox::byte> ab = as_bytes(si);
        EXPECT_EQ(ab.size(), kSize * sizeof(int));
    }

    // const char
    {
        const char ac[kSize] = {};
        vox::span<const char> sc(ac);
        EXPECT_EQ(sc.size(), kSize);

        vox::span<const vox::byte> ab = as_bytes(sc);
        EXPECT_EQ(ab.size(), sc.size());
    }
}

TEST(SpanFill, Platform) {
    alignas(alignof(int)) vox::byte abuffer[16];
    vox::span<vox::byte> src(abuffer);

    vox::span<int> ispan1 = vox::fill_span<int>(src, 3);
    EXPECT_EQ(ispan1.size(), 3u);
    vox::span<int> ispan2 = vox::fill_span<int>(src, 1);
    EXPECT_EQ(ispan2.size(), 1u);
    EXPECT_TRUE(src.empty());
    EXPECT_ASSERTION(vox::fill_span<int>(src, 1), "Invalid range.");

    // Bad alignment
    src = vox::make_span(abuffer);

    vox::span<char> cspan = vox::fill_span<char>(src, 1);
    EXPECT_EQ(cspan.size(), 1u);
    EXPECT_ASSERTION(vox::fill_span<int>(src, 1), "Invalid alignment.");
}

TEST(SpanRangeLoop, Platform) {
    const size_t kSize = 46;
    size_t ai[kSize];

    // non const
    vox::span<size_t> si = vox::make_span(ai);
    size_t i = 0;
    for (size_t& li : si) {
        li = i++;
    }

    EXPECT_EQ(i, kSize);

    // const
    vox::span<const size_t> sci = si;
    i = 0;
    for (const size_t& li : sci) {
        EXPECT_EQ(i, li);
        i++;
    }
}

TEST(SpanSubSpan, Platform) {
    const size_t kSize = 46;
    size_t ai[kSize];
    for (size_t i = 0; i < kSize; ++i) {
        ai[i] = i;
    }

    {  // empty
        vox::span<size_t> eai;
        vox::span<size_t> seai = eai.subspan(0, 0);
        EXPECT_EQ(seai.size(), 0u);
    }

    {  // subspan
        vox::span<size_t> ncai(ai);

        EXPECT_ASSERTION(ncai.subspan(kSize, 1), " count out of range");
        EXPECT_ASSERTION(ncai.subspan(1, kSize), " count out of range");
        EXPECT_ASSERTION(ncai.subspan(kSize + 1, 0), "Offset out of range");
        EXPECT_ASSERTION(ncai.subspan(0, kSize + 1), "Count out of range");

        EXPECT_EQ(ncai.subspan(0, 0).size(), 0u);
        EXPECT_EQ(ncai.subspan(0, kSize).size(), kSize);
        EXPECT_EQ(ncai.subspan(0, kSize - 10)[0], 0u);
        EXPECT_EQ(ncai.subspan(10, kSize - 10).size(), kSize - 10);
        EXPECT_EQ(ncai.subspan(10, kSize - 10)[0], 10u);
        EXPECT_EQ(ncai.subspan(0, kSize - 10).size(), kSize - 10);
    }

    {  // first - last
        vox::span<size_t> ncai(ai);

        EXPECT_ASSERTION(ncai.first(kSize + 1), "Count out of range");
        EXPECT_EQ(ncai.first(0).size(), 0u);
        EXPECT_EQ(ncai.first(10).size(), 10u);
        EXPECT_EQ(ncai.first(10)[0], 0u);

        EXPECT_ASSERTION(ncai.last(kSize + 1), "Count out of range");
        EXPECT_EQ(ncai.last(0).size(), 0u);
        EXPECT_EQ(ncai.last(10).size(), 10u);
        EXPECT_EQ(ncai.last(10)[0], kSize - 10);
    }
}
