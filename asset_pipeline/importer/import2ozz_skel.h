//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "asset_pipeline/export.h"
#include "vox.base/endianness.h"
#include "vox.base/macros.h"

namespace Json {
class Value;
}

namespace vox::animation::offline {

class OzzImporter;

VOX_ASSET_DLL bool ImportSkeleton(const Json::Value& _config,
                                  OzzImporter* _importer,
                                  vox::Endianness _endianness);

}  // namespace vox