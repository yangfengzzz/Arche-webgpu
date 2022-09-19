//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdlib>

namespace vox {
/**
 * Number of cascades to use for directional light shadows.
 */
enum ShadowCascadesMode : uint32_t {
    /** No cascades */
    NoCascades = 1,
    /** Two cascades */
    TwoCascades = 2,
    /** Four cascades */
    FourCascades = 4
};

}  // namespace vox