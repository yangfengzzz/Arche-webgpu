//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "asset_pipeline/export.h"
#include "asset_pipeline/fbx/fbx.h"
#include "asset_pipeline/importer/import2ozz.h"

namespace vox::animation::offline {
struct RawSkeleton;
namespace fbx {
VOX_ASSET_DLL bool ExtractSkeleton(FbxSceneLoader& _loader,
                                   const OzzImporter::NodeType& _types,
                                   RawSkeleton* _skeleton);

}  // namespace fbx
}  // namespace vox::animation::offline