//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/tools/export.h"
#include "vox.animation/offline/tools/import2vox.h"
#include "vox.animation/offline/tools/import2vox_config.h"
#include "vox.base/endianness.h"
#include "vox.base/macros.h"

namespace Json {
class Value;
}

namespace vox {
namespace animation {
class Skeleton;
namespace offline {

class OzzImporter;
VOX_ANIMTOOLS_DLL bool ProcessTracks(OzzImporter& _importer,
                                     const char* _animation_name,
                                     const Skeleton& _skeleton,
                                     const Json::Value& _config,
                                     const vox::Endianness _endianness);

// Property type enum to config string conversions.
struct VOX_ANIMTOOLS_DLL PropertyTypeConfig : JsonEnum<PropertyTypeConfig, OzzImporter::NodeProperty::Type> {
    static EnumNames GetNames();
};
}  // namespace offline
}  // namespace animation
}  // namespace vox