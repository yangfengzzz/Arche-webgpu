//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/raw_skeleton.h"
#include "vox.base/containers/string_archive.h"
#include "vox.base/containers/vector_archive.h"
#include "vox.math/math_archive.h"

namespace vox::io {

void Extern<animation::offline::RawSkeleton>::Save(OArchive& _archive,
                                                   const animation::offline::RawSkeleton* _skeletons,
                                                   size_t _count) {
    for (size_t i = 0; i < _count; ++i) {
        const animation::offline::RawSkeleton& skeleton = _skeletons[i];
        _archive << skeleton.roots;
    }
}
void Extern<animation::offline::RawSkeleton>::Load(IArchive& _archive,
                                                   animation::offline::RawSkeleton* _skeletons,
                                                   size_t _count,
                                                   uint32_t _version) {
    (void)_version;
    for (size_t i = 0; i < _count; ++i) {
        animation::offline::RawSkeleton& skeleton = _skeletons[i];
        _archive >> skeleton.roots;
    }
}

// RawSkeleton::Joint's version can be declared locally as it will be saved from
// this cpp file only.
VOX_IO_TYPE_VERSION(1, animation::offline::RawSkeleton::Joint)

template <>
struct Extern<animation::offline::RawSkeleton::Joint> {
    static void Save(OArchive& _archive, const animation::offline::RawSkeleton::Joint* _joints, size_t _count) {
        for (size_t i = 0; i < _count; ++i) {
            const animation::offline::RawSkeleton::Joint& joint = _joints[i];
            _archive << joint.name;
            _archive << joint.transform;
            _archive << joint.children;
        }
    }
    static void Load(IArchive& _archive,
                     animation::offline::RawSkeleton::Joint* _joints,
                     size_t _count,
                     uint32_t _version) {
        (void)_version;
        for (size_t i = 0; i < _count; ++i) {
            animation::offline::RawSkeleton::Joint& joint = _joints[i];
            _archive >> joint.name;
            _archive >> joint.transform;
            _archive >> joint.children;
        }
    }
};
}  // namespace vox::io
