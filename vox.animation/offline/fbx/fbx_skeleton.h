//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/fbx/export.h"
#include "vox.animation/offline/fbx/fbx.h"
#include "vox.animation/offline/tools/import2vox.h"

namespace vox {
namespace animation {
namespace offline {

struct RawSkeleton;

namespace fbx {

VOX_ANIMFBX_DLL bool ExtractSkeleton(FbxSceneLoader& _loader,
                                     const OzzImporter::NodeType& _types,
                                     RawSkeleton* _skeleton);

}  // namespace fbx
}  // namespace offline
}  // namespace animation
}  // namespace vox