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
namespace offline {

class OzzImporter;
VOX_ANIMTOOLS_DLL bool ImportAnimations(const Json::Value& _config,
                                        OzzImporter* _importer,
                                        const vox::Endianness _endianness);

// Additive reference enum to config string conversions.
struct AdditiveReferenceEnum {
    enum Value { kAnimation, kSkeleton };
};
struct VOX_ANIMTOOLS_DLL AdditiveReference : JsonEnum<AdditiveReference, AdditiveReferenceEnum::Value> {
    static EnumNames GetNames();
};
}  // namespace offline
}  // namespace animation
}  // namespace vox