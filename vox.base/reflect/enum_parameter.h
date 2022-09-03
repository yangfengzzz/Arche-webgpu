//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "vox.base/reflect/parameter.h"

namespace vox::utility {
/** Class of an enum parameter. */
class EnumParameter : public Parameter<int> {
public:
    using Ptr = std::unique_ptr<EnumParameter>;

    struct EnumValue {
        int id{};
        std::string name;
    };

protected:
    std::vector<EnumValue> m_enumValues;

public:
    EnumParameter(const std::string& name, const std::string& label, int* valuePtr)
        : Parameter<int>(name, label, ParameterBase::ENUM, valuePtr) {}

    EnumParameter(const std::string& name, const std::string& label, GetFunc<int> getValue, SetFunc<int> setValue)
        : Parameter<int>(name, label, ParameterBase::ENUM, std::move(getValue), std::move(setValue)) {}

    ~EnumParameter() override = default;

    /** Add an enum value to this parameter type. */
    void addEnumValue(const std::string& name, int& id) {
        EnumValue val;
        val.name = name;
        id = static_cast<int>(m_enumValues.size());
        val.id = id;
        m_enumValues.push_back(val);
    }

    /** Clear all enum values.
     */
    void clearEnumValues() { m_enumValues.clear(); }

    const std::vector<EnumValue>& getEnumValues() { return m_enumValues; }
};
}  // namespace vox::utility