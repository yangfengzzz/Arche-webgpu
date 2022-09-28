//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/containers/array.h"
#include "vox.base/io/archive.h"

namespace vox::io {

VOX_IO_TYPE_NOT_VERSIONABLE_T2(class _Ty, size_t _N, std::array<_Ty, _N>)

template <class Ty, size_t N>
struct Extern<std::array<Ty, N>> {
    inline static void Save(OArchive& _archive, const std::array<Ty, N>* _values, size_t _count) {
        if (void(0), N != 0) {
            for (size_t i = 0; i < _count; i++) {
                const std::array<Ty, N>& array = _values[i];
                _archive << vox::io::MakeArray(array.data(), N);
            }
        }
    }
    inline static void Load(IArchive& _archive, std::array<Ty, N>* _values, size_t _count, uint32_t _version) {
        (void)_version;
        if (void(0), N != 0) {
            for (size_t i = 0; i < _count; i++) {
                std::array<Ty, N>& array = _values[i];
                _archive >> vox::io::MakeArray(array.data(), N);
            }
        }
    }
};
}  // namespace vox::io
