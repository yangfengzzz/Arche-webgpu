//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cstdint>
#include <limits>

#include "gtest/gtest.h"
#include "vox.base/io/stream.h"

void TestStream(vox::io::Stream* _stream) {
    ASSERT_TRUE(_stream->opened());
    EXPECT_EQ(_stream->Size(), 0u);
    EXPECT_EQ(_stream->Seek(0, vox::io::Stream::kSet), 0);
    EXPECT_EQ(_stream->Tell(), 0);
    typedef int Type;
    const Type to_write = 46;
    EXPECT_EQ(_stream->Write(&to_write, sizeof(Type)), sizeof(Type));
    EXPECT_EQ(_stream->Tell(), static_cast<int>(sizeof(Type)));
    EXPECT_EQ(_stream->Seek(0, vox::io::Stream::kSet), 0);
    EXPECT_EQ(_stream->Tell(), 0);
    EXPECT_EQ(_stream->Size(), sizeof(Type));
    Type to_read = 0;
    EXPECT_EQ(_stream->Read(&to_read, sizeof(Type)), sizeof(Type));
    EXPECT_EQ(to_read, to_write);
    EXPECT_EQ(_stream->Tell(), static_cast<int>(sizeof(Type)));
    EXPECT_EQ(_stream->Size(), sizeof(Type));
}

void TestSeek(vox::io::Stream* _stream) {
    ASSERT_TRUE(_stream->opened());
    EXPECT_EQ(_stream->Seek(0, vox::io::Stream::kSet), 0);
    EXPECT_EQ(_stream->Tell(), 0);
    EXPECT_EQ(_stream->Size(), 0u);

    // Seeking before file's begin.
    EXPECT_NE(_stream->Seek(-1, vox::io::Stream::kSet), 0);
    EXPECT_EQ(_stream->Tell(), 0);
    EXPECT_NE(_stream->Seek(-1, vox::io::Stream::kCurrent), 0);
    EXPECT_EQ(_stream->Tell(), 0);
    EXPECT_NE(_stream->Seek(-1, vox::io::Stream::kEnd), 0);
    EXPECT_EQ(_stream->Tell(), 0);
    EXPECT_EQ(_stream->Size(), 0u);

    // Bad seek argument.
    EXPECT_EQ(_stream->Seek(46, vox::io::Stream::Origin(27)), -1);
    EXPECT_EQ(_stream->Tell(), 0);
    EXPECT_EQ(_stream->Size(), 0u);

    typedef int Type;
    const Type to_write = 46;
    EXPECT_EQ(_stream->Write(&to_write, sizeof(Type)), sizeof(Type));
    EXPECT_EQ(_stream->Tell(), static_cast<int>(sizeof(Type)));
    EXPECT_EQ(_stream->Size(), sizeof(Type));

    const int64_t kEnd = 465827;
    // Force file length to kEnd but do not write to the stream.
    EXPECT_EQ(_stream->Seek(kEnd - _stream->Tell(), vox::io::Stream::kCurrent), 0);
    EXPECT_EQ(_stream->Tell(), kEnd);
    EXPECT_EQ(_stream->Size(), sizeof(Type));

    Type to_read = 0;
    EXPECT_EQ(_stream->Seek(0, vox::io::Stream::kSet), 0);
    EXPECT_EQ(_stream->Size(), sizeof(Type));
    EXPECT_EQ(_stream->Read(&to_read, sizeof(Type)), sizeof(Type));
    EXPECT_EQ(to_read, to_write);
    EXPECT_EQ(_stream->Tell(), static_cast<int>(sizeof(Type)));
    EXPECT_EQ(_stream->Read(&to_read, sizeof(Type)), 0u);
    EXPECT_EQ(_stream->Tell(), static_cast<int>(sizeof(Type)));
    EXPECT_EQ(_stream->Size(), sizeof(Type));

    // Force file length to kEnd + sizeof(Type) and write to the stream.
    EXPECT_EQ(_stream->Seek(kEnd - _stream->Tell() - static_cast<int>(sizeof(Type)), vox::io::Stream::kCurrent), 0);
    EXPECT_EQ(_stream->Tell(), kEnd - static_cast<int>(sizeof(Type)));
    EXPECT_EQ(_stream->Write(&to_write, sizeof(Type)), sizeof(Type));
    EXPECT_EQ(_stream->Tell(), kEnd);
    EXPECT_EQ(_stream->Seek(-static_cast<int>(sizeof(Type)), vox::io::Stream::kEnd), 0);
    EXPECT_EQ(_stream->Tell(), kEnd - static_cast<int>(sizeof(Type)));
    EXPECT_EQ(_stream->Read(&to_read, sizeof(Type)), sizeof(Type));
    EXPECT_EQ(to_read, to_write);
    EXPECT_EQ(_stream->Tell(), kEnd);
    EXPECT_EQ(_stream->Seek(-static_cast<int>(sizeof(Type)) * 2, vox::io::Stream::kEnd), 0);
    EXPECT_EQ(_stream->Read(&to_read, sizeof(Type)), sizeof(Type));
    EXPECT_EQ(to_read, 0);
    EXPECT_EQ(_stream->Tell(), kEnd - static_cast<int>(sizeof(Type)));
    // Rewind from kEnd.
    EXPECT_EQ(_stream->Seek(-kEnd, vox::io::Stream::kEnd), 0);
    EXPECT_EQ(_stream->Tell(), 0);
    EXPECT_EQ(_stream->Seek(kEnd, vox::io::Stream::kSet), 0);
    EXPECT_EQ(_stream->Tell(), kEnd);

    // Read at the end of the file.
    EXPECT_EQ(_stream->Read(&to_read, sizeof(Type)), 0u);
    EXPECT_EQ(_stream->Tell(), kEnd);

    // Read after a seek beyond the end of the file.
    EXPECT_EQ(_stream->Seek(4, vox::io::Stream::kCurrent), 0);
    EXPECT_EQ(_stream->Tell(), kEnd + 4);
    EXPECT_EQ(_stream->Read(&to_read, sizeof(Type)), 0u);
    EXPECT_EQ(_stream->Tell(), kEnd + 4);
    EXPECT_EQ(_stream->Size(), static_cast<size_t>(kEnd));
}

void TestTooBigStream(vox::io::Stream* _stream) {
    const int max_size = std::numeric_limits<int>::max();
    ASSERT_TRUE(_stream->opened());
    EXPECT_EQ(_stream->Seek(0, vox::io::Stream::kSet), 0);
    EXPECT_EQ(_stream->Tell(), 0);

    // Seeks outside of Stream valid range.
    EXPECT_EQ(_stream->Seek(max_size, vox::io::Stream::kCurrent), 0);
    EXPECT_EQ(_stream->Tell(), max_size);
    EXPECT_EQ(_stream->Size(), 0u);
    EXPECT_NE(_stream->Seek(max_size, vox::io::Stream::kCurrent), 0);
    EXPECT_EQ(_stream->Tell(), max_size);
    EXPECT_EQ(_stream->Size(), 0u);

    // Writes/Reads outside of Stream valid range.
    EXPECT_EQ(_stream->Seek(1, vox::io::Stream::kSet), 0);
    EXPECT_EQ(_stream->Tell(), 1);
    char c;
    EXPECT_EQ(_stream->Write(&c, max_size), 0u);
    EXPECT_EQ(_stream->Read(&c, max_size), 0u);
    EXPECT_EQ(_stream->Size(), 0u);
}

TEST(File, Stream) {
    {
        vox::io::File file(nullptr);
        EXPECT_FALSE(file.opened());
    }
    { EXPECT_FALSE(vox::io::File::Exist("unexisting.file")); }
    {
        vox::io::File file("test.bin", "w+t");
        EXPECT_TRUE(file.opened());
        TestSeek(&file);
    }
    { EXPECT_TRUE(vox::io::File::Exist("test.bin")); }
}

TEST(MemoryStream, Stream) {
    {
        vox::io::MemoryStream stream;
        TestStream(&stream);
    }
    {
        vox::io::MemoryStream stream;
        TestSeek(&stream);
    }
    {
        vox::io::MemoryStream stream;
        TestTooBigStream(&stream);
    }
}
