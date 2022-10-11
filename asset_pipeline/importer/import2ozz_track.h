//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "asset_pipeline/export.h"
#include "asset_pipeline/importer/import2ozz.h"
#include "asset_pipeline/importer/import2ozz_config.h"
#include "vox.base/endianness.h"
#include "vox.base/macros.h"

namespace Json {
class Value;
}

namespace vox::animation {
class Skeleton;
namespace offline {

class OzzImporter;
VOX_ASSET_DLL bool ProcessTracks(OzzImporter& _importer,
                                 const char* _animation_name,
                                 const Skeleton& _skeleton,
                                 const Json::Value& _config,
                                 vox::Endianness _endianness);

// Property type enum to config string conversions.
struct VOX_ASSET_DLL PropertyTypeConfig : JsonEnum<PropertyTypeConfig, OzzImporter::NodeProperty::Type> {
    static EnumNames GetNames();
};
}  // namespace offline
}  // namespace vox::animation