//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "voxel.h"
#include <string>

namespace vox {
namespace geo {
VoxelType getVoxelType(const char *str) {
    for (int j = 0; j < (int)VoxelType::Max; ++j) {
        if (std::strcmp(VoxelTypeStr[j], str) != 0) {
            continue;
        }
        return (VoxelType)j;
    }
    return VoxelType::Max;
}

void Voxel::setFlags(uint8_t flags) {
    _flags = flags;
    // max 3 bits
    assert(flags <= 7);
}


}
}
