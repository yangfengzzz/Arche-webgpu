//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/reflect/parameter_object.h"

namespace vox::utility {
class TestParameterObject : public ParameterObject {
protected:
    bool m_boolValue{};
    int m_intValue{};
    double m_doubleValue{};
    int m_enumValue{};
    int m_enumValue2{};
    std::string m_strValue;
    double m_vec3Value[3]{};
    int m_counter;

public:
    static int MY_BOOL_PARAMETER;
    static int MY_INT_PARAMETER;
    static int MY_DOUBLE_PARAMETER;
    static int MY_ENUM_PARAMETER;
    static int MY_ENUM_PARAMETER2;
    static int MY_STRING_PARAMETER;
    static int MY_VEC3_PARAMETER;
    static int MY_FUNCTION_PARAMETER;

    static int MY_ENUM_VALUE1;
    static int MY_ENUM_VALUE2;
    static int MY_ENUM2_VALUE1;
    static int MY_ENUM2_VALUE2;

    TestParameterObject() {
        initParameters();
        m_counter = 0;
    };
    ~TestParameterObject() override = default;

    void initParameters() override;

    [[nodiscard]] double getDoubleValue() const { return m_doubleValue; }
    void setDoubleValue(double val) { m_doubleValue = val; }

    [[nodiscard]] double getIntValue() const { return m_intValue; }
    void setIntValue(int val) { m_intValue = val; }

    [[nodiscard]] int getEnumValue2() const { return m_enumValue2; }

    [[nodiscard]] int getCounter() const { return m_counter; }
    void setCounter(int val) { m_counter = val; }
};
}  // namespace vox::utility