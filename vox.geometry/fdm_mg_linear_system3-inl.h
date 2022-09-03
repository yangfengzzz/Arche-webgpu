//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/fdm_mg_linear_system3.h"

namespace vox {

template <typename T>
void FdmMgUtils3::resizeArrayWithCoarsest(const Size3 &coarsestResolution,
                                          size_t numberOfLevels,
                                          std::vector<Array3<T>> *levels) {
    numberOfLevels = std::max(numberOfLevels, kOneSize);

    levels->resize(numberOfLevels);

    // Level 0 is the finest level, thus takes coarsestResolution ^
    // numberOfLevels.
    // Level numberOfLevels - 1 is the coarsest, taking coarsestResolution.
    Size3 res = coarsestResolution;
    for (size_t level = 0; level < numberOfLevels; ++level) {
        (*levels)[numberOfLevels - level - 1].resize(res);
        res.x = res.x << 1;
        res.y = res.y << 1;
        res.z = res.z << 1;
    }
}

template <typename T>
void FdmMgUtils3::resizeArrayWithFinest(const Size3 &finestResolution,
                                        size_t maxNumberOfLevels,
                                        std::vector<Array3<T>> *levels) {
    Size3 res = finestResolution;
    size_t i = 1;
    for (; i < maxNumberOfLevels; ++i) {
        if (res.x % 2 == 0 && res.y % 2 == 0 && res.z % 2 == 0) {
            res.x = res.x >> 1;
            res.y = res.y >> 1;
            res.z = res.z >> 1;
        } else {
            break;
        }
    }
    resizeArrayWithCoarsest(res, i, levels);
}

}  // namespace vox