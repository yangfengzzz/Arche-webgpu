//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <nlohmann/json.hpp>

namespace vox {
class VObject {
public:
    virtual ~VObject() = default;

    /**
     * Called when the serialization is asked
     */
    virtual void onSerialize(nlohmann::json &data) = 0;

    /**
     * Called when the deserialization is asked
     */
    virtual void onDeserialize(nlohmann::json &data) = 0;
};

}  // namespace vox