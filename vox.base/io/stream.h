//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

// Provides Stream interface used to read/write a memory buffer or a file with
// Crt fread/fwrite/fseek/ftell like functions.

#include <cstddef>

#include "vox.base/macros.h"

namespace vox::io {

// Declares a stream access interface that conforms with CRT FILE API.
// This interface should be used to remap io operations.
class VOX_BASE_DLL Stream {
public:
    // Tests whether a file is opened.
    [[nodiscard]] virtual bool opened() const = 0;

    // Reads _size bytes of data to _buffer from the stream. _buffer must be big
    // enough to store _size bytes. The position indicator of the stream is
    // advanced by the total amount of bytes reads.
    // Returns the number of bytes actually read, which may be less than _size.
    virtual size_t Read(void* _buffer, size_t _size) = 0;

    // Writes _size bytes of data from _buffer to the stream. The position
    // indicator of the stream is advanced by the total number of bytes written.
    // Returns the number of bytes actually written, which may be less than _size.
    virtual size_t Write(const void* _buffer, size_t _size) = 0;

    // Declares seeking origin enumeration.
    enum Origin {
        kCurrent,  // Current position of the stream pointer.
        kEnd,      // End of stream.
        kSet,      // Beginning of stream.
    };
    // Sets the position indicator associated with the stream to a new position
    // defined by adding _offset to a reference position specified by _origin.
    // Returns a zero value if successful, otherwise returns a non-zero value.
    virtual int Seek(int _offset, Origin _origin) = 0;

    // Returns the current value of the position indicator of the stream.
    // Returns -1 if an error occurs.
    [[nodiscard]] virtual int Tell() const = 0;

    // Returns the current size of the stream.
    [[nodiscard]] virtual size_t Size() const = 0;

protected:
    Stream() = default;

    // Required virtual destructor.
    virtual ~Stream() = default;

private:
    Stream(const Stream&);
    void operator=(const Stream&);
};

// Implements Stream of type File.
class VOX_BASE_DLL File : public Stream {
public:
    // Test if a file at path _filename exists.
    // Note that this function is costly. If you aim to open the file right after,
    // then open it and use File::opened() to test if it's actually existing.
    static bool Exist(const char* _filename);

    // Open a file at path _filename with mode * _mode, in conformance with fopen
    // specifications.
    // Use opened() function to test opening result.
    File(const char* _filename, const char* _mode);

    // Gives _file ownership to the FileStream, which will be in charge of closing
    // it. _file must be nullptr or a valid std::FILE pointer.
    explicit File(void* _file);

    // Close the file if it is opened.
    ~File() override;

    // Close the file if it is opened.
    void Close();

    // See Stream::opened for details.
    [[nodiscard]] bool opened() const override;

    // See Stream::Read for details.
    size_t Read(void* _buffer, size_t _size) override;

    // See Stream::Write for details.
    size_t Write(const void* _buffer, size_t _size) override;

    // See Stream::Seek for details.
    int Seek(int _offset, Origin _origin) override;

    // See Stream::Tell for details.
    [[nodiscard]] int Tell() const override;

    // See Stream::Tell for details.
    [[nodiscard]] size_t Size() const override;

private:
    // The CRT file pointer.
    void* file_;
};

// Implements an in-memory Stream. Allows to use a memory buffer as a Stream.
// The opening mode is equivalent to fopen w+b (binary read/write).
class VOX_BASE_DLL MemoryStream : public Stream {
public:
    // Construct an empty memory stream opened in w+b mode.
    MemoryStream();

    // Closes the stream and deallocates memory buffer.
    ~MemoryStream() override;

    // See Stream::opened for details.
    [[nodiscard]] bool opened() const override;

    // See Stream::Read for details.
    size_t Read(void* _buffer, size_t _size) override;

    // See Stream::Write for details.
    size_t Write(const void* _buffer, size_t _size) override;

    // See Stream::Seek for details.
    int Seek(int _offset, Origin _origin) override;

    // See Stream::Tell for details.
    [[nodiscard]] int Tell() const override;

    // See Stream::Tell for details.
    [[nodiscard]] size_t Size() const override;

private:
    // Resizes buffers size to _size bytes. If _size is less than the actual
    // buffer size, then it remains unchanged.
    // Returns true if the buffer can contains _size bytes.
    bool Resize(size_t _size);

    // Size of the buffer increment.
    static const size_t kBufferSizeIncrement;

    // Maximum stream size.
    static const size_t kMaxSize;

    // Buffer of data.
    byte* buffer_;

    // The size of the buffer, which is greater or equal to the size of the data
    // it contains (end_).
    size_t alloc_size_;

    // The effective size of the data in the buffer.
    int end_;

    // The cursor position in the buffer of data.
    int tell_;
};
}  // namespace vox::io
