//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.navigation/nav_file.h"

namespace vox::nav {
FileIO::FileIO() : m_fp(nullptr), m_mode(-1) {}

FileIO::~FileIO() {
    if (m_fp) fclose(m_fp);
}

bool FileIO::openForWrite(const char* path) {
    if (m_fp) return false;
    m_fp = fopen(path, "wb");
    if (!m_fp) return false;
    m_mode = 1;
    return true;
}

bool FileIO::openForRead(const char* path) {
    if (m_fp) return false;
    m_fp = fopen(path, "rb");
    if (!m_fp) return false;
    m_mode = 2;
    return true;
}

bool FileIO::isWriting() const { return m_mode == 1; }

bool FileIO::isReading() const { return m_mode == 2; }

bool FileIO::write(const void* ptr, const size_t size) {
    if (!m_fp || m_mode != 1) return false;
    fwrite(ptr, size, 1, m_fp);
    return true;
}

bool FileIO::read(void* ptr, const size_t size) {
    if (!m_fp || m_mode != 2) return false;
    size_t readLen = fread(ptr, size, 1, m_fp);
    return readLen == 1;
}
}  // namespace vox::nav