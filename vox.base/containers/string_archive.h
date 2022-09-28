//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/containers/string.h"
#include "vox.base/io/archive_traits.h"
#include "vox.base/macros.h"

namespace vox::io {

VOX_IO_TYPE_NOT_VERSIONABLE(vox::string)

template <>
struct VOX_BASE_DLL Extern<vox::string> {
    static void Save(OArchive& _archive, const vox::string* _values, size_t _count);
    static void Load(IArchive& _archive, vox::string* _values, size_t _count, uint32_t _version);
};
}  // namespace vox
