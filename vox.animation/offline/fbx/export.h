//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#if defined(_MSC_VER) && defined(VOX_USE_DYNAMIC_LINKING)

#ifdef VOX_BUILD_ANIMATIONFBX_LIB
// Import/Export for dynamic linking while building vox
#define VOX_ANIMFBX_DLL __declspec(dllexport)
#else
#define VOX_ANIMFBX_DLL __declspec(dllimport)
#endif
#else  // defined(_MSC_VER) && defined(VOX_USE_DYNAMIC_LINKING)
// Static or non msvc linking
#define VOX_ANIMFBX_DLL
#endif  // defined(_MSC_VER) && defined(VOX_USE_DYNAMIC_LINKING)