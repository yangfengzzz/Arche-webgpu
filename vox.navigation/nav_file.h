//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdio>
//
#include <Recast.h>
#include <RecastDump.h>

namespace vox::nav {
class FileIO : public duFileIO {
    FILE* m_fp;
    int m_mode;

public:
    FileIO();
    ~FileIO() override;
    bool openForWrite(const char* path);
    bool openForRead(const char* path);
    [[nodiscard]] bool isWriting() const override;
    [[nodiscard]] bool isReading() const override;
    bool write(const void* ptr, size_t size) override;
    bool read(void* ptr, size_t size) override;

private:
    // Explicitly disabled copy constructor and copy assignment operator.
    FileIO(const FileIO&);
    FileIO& operator=(const FileIO&);
};
}  // namespace vox::nav