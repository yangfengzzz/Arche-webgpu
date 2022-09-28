//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/io/archive.h"

#include <cassert>

namespace vox::io {

// OArchive implementation.

OArchive::OArchive(Stream* _stream, Endianness _endianness)
    : stream_(_stream), endian_swap_(_endianness != getNativeEndianness()) {
    assert(stream_ && stream_->opened() && "_stream argument must point a valid opened stream.");
    // Save as a single byte as it does not need to be swapped.
    auto endianness = static_cast<uint8_t>(_endianness);
    *this << endianness;
}

// IArchive implementation.

IArchive::IArchive(Stream* _stream) : stream_(_stream), endian_swap_(false) {
    assert(stream_ && stream_->opened() && "_stream argument must point a valid opened stream.");
    // Endianness was saved as a single byte, as it does not need to be swapped.
    uint8_t endianness;
    *this >> endianness;
    endian_swap_ = endianness != getNativeEndianness();
}
}  // namespace vox
