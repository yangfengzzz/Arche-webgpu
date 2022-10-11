//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <json/forwards.h>

#include "asset_pipeline/export.h"
#include "vox.base/macros.h"

namespace vox::animation::offline {

// Get the sanitized (all members are set, with the right types) configuration.
VOX_ASSET_DLL bool ProcessConfiguration(Json::Value* _config);

// Internal function used to compare enum names.
VOX_ASSET_DLL bool CompareName(const char* _a, const char* _b);

// Struct allowing inheriting class to provide enum names.
template <typename Type, typename Enum>
struct JsonEnum {
    // Struct allowing inheriting class to provide enum names.
    struct EnumNames {
        size_t count;
        const char** names;
    };

    static bool GetEnumFromName(const char* _name, Enum* _enum) {
        const EnumNames enums = Type::GetNames();
        for (size_t i = 0; i < enums.count; ++i) {
            if (CompareName(enums.names[i], _name)) {
                *_enum = static_cast<Enum>(i);
                return true;
            }
        }
        return false;
    }

    static const char* GetEnumName(Enum _enum) {
        const EnumNames enums = Type::GetNames();
        assert(static_cast<size_t>(_enum) < enums.count);
        return enums.names[_enum];
    }

    static bool IsValidEnumName(const char* _name) {
        const EnumNames enums = Type::GetNames();
        bool valid = false;
        for (size_t i = 0; !valid && i < enums.count; ++i) {
            valid = CompareName(enums.names[i], _name);
        }
        return valid;
    }
};

}  // namespace vox::animation::offline