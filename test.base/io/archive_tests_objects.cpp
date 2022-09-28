//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "test.base/io/archive_tests_objects.h"

#include "gtest/gtest.h"
#include "vox.base/io/archive.h"

void Intrusive::Save(vox::io::OArchive& _archive) const { _archive << i; }

void Intrusive::Load(vox::io::IArchive& _archive, uint32_t _version) {
    EXPECT_EQ(_version, 46u);
    _archive >> i;
}

namespace vox::io {

// Specializes Extrusive type external Save and Load functions.
void Extern<Extrusive>::Save(OArchive& _archive, const Extrusive* _test, size_t _count) {
    _archive << vox::io::MakeArray(&_test->i, _count);
}
void Extern<Extrusive>::Load(IArchive& _archive, Extrusive* _test, size_t _count, uint32_t _version) {
    EXPECT_EQ(_version, 0u);
    _archive >> vox::io::MakeArray(&_test->i, _count);
}
}  // namespace vox::io

void Tagged1::Save(vox::io::OArchive& /*_archive*/) const {}
void Tagged1::Load(vox::io::IArchive& /*_archive*/, uint32_t /*_version*/) {}
void Tagged2::Save(vox::io::OArchive& /*_archive*/) const {}
void Tagged2::Load(vox::io::IArchive& /*_archive*/, uint32_t /*_version*/) {}
