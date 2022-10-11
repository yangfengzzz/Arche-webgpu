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

namespace vox::animation::offline {

class OzzImporter;
VOX_ASSET_DLL bool ImportAnimations(const Json::Value& _config,
                                    OzzImporter* _importer,
                                    vox::Endianness _endianness);

// Additive reference enum to config string conversions.
struct AdditiveReferenceEnum {
    enum Value { kAnimation, kSkeleton };
};
struct VOX_ASSET_DLL AdditiveReference : JsonEnum<AdditiveReference, AdditiveReferenceEnum::Value> {
    static EnumNames GetNames();
};
}  // namespace vox::animation::offline