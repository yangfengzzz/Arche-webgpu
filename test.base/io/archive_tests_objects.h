//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef VOX_TEST_BASE_IO_ARCHIVE_TESTS_OBJECTS_H_
#define VOX_TEST_BASE_IO_ARCHIVE_TESTS_OBJECTS_H_

#include "vox.base/io/archive_traits.h"
#include "vox.base/macros.h"

namespace vox::io {
class OArchive;
class IArchive;
}  // namespace vox::io

struct Intrusive {
    explicit Intrusive(int32_t _i = 12) : i(_i) {}
    void Save(vox::io::OArchive& _archive) const;
    void Load(vox::io::IArchive& _archive, uint32_t _version);
    int32_t i;
};

struct Extrusive {
    uint64_t i;
};

namespace vox::io {
// Give Intrusive type a version.
VOX_IO_TYPE_VERSION(46, Intrusive)

// Extrusive is not versionable.
VOX_IO_TYPE_NOT_VERSIONABLE(Extrusive)

// Specializes Extrusive type external Save and Load functions.
template <>
struct Extern<Extrusive> {
    static void Save(OArchive& _archive, const Extrusive* _test, size_t _count);
    static void Load(IArchive& _archive, Extrusive* _test, size_t _count, uint32_t _version);
};
}  // namespace vox::io

class Tagged1 {
public:
    void Save(vox::io::OArchive& _archive) const;
    void Load(vox::io::IArchive& _archive, uint32_t _version);
};

class Tagged2 {
public:
    void Save(vox::io::OArchive& _archive) const;
    void Load(vox::io::IArchive& _archive, uint32_t _version);
};

namespace vox::io {
VOX_IO_TYPE_NOT_VERSIONABLE(Tagged1)
VOX_IO_TYPE_TAG("tagged1", Tagged1)
VOX_IO_TYPE_NOT_VERSIONABLE(Tagged2)
VOX_IO_TYPE_TAG("tagged2", Tagged2)
}  // namespace vox::io
#endif  // VOX_TEST_BASE_IO_ARCHIVE_TESTS_OBJECTS_H_
