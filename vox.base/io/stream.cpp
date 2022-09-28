//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/io/stream.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>

#include "vox.base/memory/allocator.h"

namespace vox::io {

// Starts File implementation.

bool File::Exist(const char* _filename) {
    FILE* file = std::fopen(_filename, "r");
    if (file) {
        std::fclose(file);
        return true;
    }
    return false;
}

File::File(const char* _filename, const char* _mode) : file_(std::fopen(_filename, _mode)) {}

File::File(void* _file) : file_(_file) {}

File::~File() { Close(); }

void File::Close() {
    if (file_) {
        auto* file = reinterpret_cast<std::FILE*>(file_);
        std::fclose(file);
        file_ = nullptr;
    }
}

bool File::opened() const { return file_ != nullptr; }

size_t File::Read(void* _buffer, size_t _size) {
    auto* file = reinterpret_cast<std::FILE*>(file_);
    return std::fread(_buffer, 1, _size, file);
}

size_t File::Write(const void* _buffer, size_t _size) {
    auto* file = reinterpret_cast<std::FILE*>(file_);
    return std::fwrite(_buffer, 1, _size, file);
}

int File::Seek(int _offset, Origin _origin) {
    int origins[] = {SEEK_CUR, SEEK_END, SEEK_SET};
    if (_origin >= static_cast<int>(VOX_ARRAY_SIZE(origins))) {
        return -1;
    }
    auto* file = reinterpret_cast<std::FILE*>(file_);
    return std::fseek(file, _offset, origins[_origin]);
}

int File::Tell() const {
    auto* file = reinterpret_cast<std::FILE*>(file_);
    const long current = std::ftell(file);
    return static_cast<int>(current);
}

size_t File::Size() const {
    auto* file = reinterpret_cast<std::FILE*>(file_);

    const long current = std::ftell(file);
    assert(current >= 0);
    int seek = std::fseek(file, 0, SEEK_END);
    assert(seek == 0);
    (void)seek;
    const long end = std::ftell(file);
    assert(end >= 0);
    seek = std::fseek(file, current, SEEK_SET);
    assert(seek == 0);

    return static_cast<size_t>(end);
}

// Starts MemoryStream implementation.
const size_t MemoryStream::kBufferSizeIncrement = 16 << 10;
const size_t MemoryStream::kMaxSize = std::numeric_limits<int>::max();

MemoryStream::MemoryStream() : buffer_(nullptr), alloc_size_(0), end_(0), tell_(0) {}

MemoryStream::~MemoryStream() {
    vox::memory::default_allocator()->Deallocate(buffer_);
    buffer_ = nullptr;
}

bool MemoryStream::opened() const { return true; }

size_t MemoryStream::Read(void* _buffer, size_t _size) {
    // A read cannot set file position beyond the end of the file.
    // A read cannot exceed the maximum Stream size.
    if (tell_ > end_ || _size > kMaxSize) {
        return 0;
    }

    const int read_size = std::min(end_ - tell_, static_cast<int>(_size));
    std::memcpy(_buffer, buffer_ + tell_, read_size);
    tell_ += read_size;
    return read_size;
}

size_t MemoryStream::Write(const void* _buffer, size_t _size) {
    if (_size > kMaxSize || tell_ > static_cast<int>(kMaxSize - _size)) {
        // A write cannot exceed the maximum Stream size.
        return 0;
    }
    if (tell_ > end_) {
        // The fseek() function shall allow the file-position indicator to be set
        // beyond the end of existing data in the file. If data is later written at
        // this point, subsequent reads of data in the gap shall return bytes with
        // the value 0 until data is actually written into the gap.
        if (!Resize(tell_)) {
            return 0;
        }
        // Fills the gap with 0's.
        const size_t gap = tell_ - end_;
        std::memset(buffer_ + end_, 0, gap);
        end_ = tell_;
    }

    const int size = static_cast<int>(_size);
    const int tell_end = tell_ + size;
    if (Resize(tell_end)) {
        end_ = std::max(tell_end, end_);
        std::memcpy(buffer_ + tell_, _buffer, _size);
        tell_ += size;
        return _size;
    }
    return 0;
}

int MemoryStream::Seek(int _offset, Origin _origin) {
    int origin;
    switch (_origin) {
        case kCurrent:
            origin = tell_;
            break;
        case kEnd:
            origin = end_;
            break;
        case kSet:
            origin = 0;
            break;
        default:
            return -1;
    }

    // Exit if seeking before file begin or beyond max file size.
    if (origin < -_offset || (_offset > 0 && origin > static_cast<int>(kMaxSize) - _offset)) {
        return -1;
    }

    // So tell_ is moved but end_ pointer is not moved until something is later
    // written.
    tell_ = origin + _offset;
    return 0;
}

int MemoryStream::Tell() const { return tell_; }

size_t MemoryStream::Size() const { return static_cast<size_t>(end_); }

bool MemoryStream::Resize(size_t _size) {
    if (_size > alloc_size_) {
        // Resize to the next multiple of kBufferSizeIncrement, requires
        // kBufferSizeIncrement to be a power of 2.
        static_assert((MemoryStream::kBufferSizeIncrement & (kBufferSizeIncrement - 1)) == 0,
                      "kBufferSizeIncrement must be a power of 2");
        const size_t new_size = vox::align(_size, kBufferSizeIncrement);
        byte* new_buffer = reinterpret_cast<byte*>(vox::memory::default_allocator()->Allocate(new_size, 16));
        if (buffer_ != nullptr) {
            std::memcpy(new_buffer, buffer_, alloc_size_);
        }
        vox::memory::default_allocator()->Deallocate(buffer_);
        buffer_ = new_buffer;
        alloc_size_ = new_size;
    }
    return _size == 0 || buffer_ != nullptr;
}
}  // namespace vox::io
