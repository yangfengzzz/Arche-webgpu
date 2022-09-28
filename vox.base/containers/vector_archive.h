//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/containers/vector.h"
#include "vox.base/io/archive.h"
#include "vox.base/macros.h"

namespace vox::io {

VOX_IO_TYPE_NOT_VERSIONABLE_T2(class _Ty, class _Allocator, std::vector<_Ty, _Allocator>)

template <class Ty, class Allocator>
struct Extern<std::vector<Ty, Allocator>> {
    inline static void Save(OArchive& _archive, const std::vector<Ty, Allocator>* _values, size_t _count) {
        for (size_t i = 0; i < _count; i++) {
            const std::vector<Ty, Allocator>& vector = _values[i];
            const auto size = static_cast<uint32_t>(vector.size());
            _archive << size;
            if (size > 0) {
                _archive << vox::io::MakeArray(vector.data(), size);
            }
        }
    }
    inline static void Load(IArchive& _archive, std::vector<Ty, Allocator>* _values, size_t _count, uint32_t _version) {
        (void)_version;
        for (size_t i = 0; i < _count; i++) {
            std::vector<Ty, Allocator>& vector = _values[i];
            uint32_t size;
            _archive >> size;
            vector.resize(size);
            if (size > 0) {
                _archive >> vox::io::MakeArray(vector.data(), size);
            }
        }
    }
};
}  // namespace vox::io