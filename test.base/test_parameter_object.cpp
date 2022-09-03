//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "test_parameter_object.h"

#include "vox.base/reflect/enum_parameter.h"

using namespace vox::utility;

int TestParameterObject::MY_BOOL_PARAMETER = -1;
int TestParameterObject::MY_INT_PARAMETER = -1;
int TestParameterObject::MY_DOUBLE_PARAMETER = -1;
int TestParameterObject::MY_ENUM_PARAMETER = -1;
int TestParameterObject::MY_ENUM_PARAMETER2 = -1;
int TestParameterObject::MY_STRING_PARAMETER = -1;
int TestParameterObject::MY_VEC3_PARAMETER = -1;
int TestParameterObject::MY_FUNCTION_PARAMETER = -1;

int TestParameterObject::MY_ENUM_VALUE1 = -1;
int TestParameterObject::MY_ENUM_VALUE2 = -1;
int TestParameterObject::MY_ENUM2_VALUE1 = -1;
int TestParameterObject::MY_ENUM2_VALUE2 = -1;

void TestParameterObject::initParameters() {
    ParameterObject::initParameters();

    m_boolValue = true;
    MY_BOOL_PARAMETER = createBoolParameter("boolParam", "Bool param", &m_boolValue);
    setGroup(MY_BOOL_PARAMETER, "ParameterGroup");
    setDescription(MY_BOOL_PARAMETER, "Help text");

    m_intValue = 3;
    MY_INT_PARAMETER = createNumericParameter("intParam", "Int param", &m_intValue);
    setGroup(MY_INT_PARAMETER, "ParameterGroup");
    setDescription(MY_INT_PARAMETER, "Help text");
    auto* intParam = static_cast<IntParameter*>(getParameter(MY_INT_PARAMETER));
    intParam->setMinValue(2);
    intParam->setMaxValue(4);

    setDoubleValue(3.123);
    ParameterBase::GetFunc<double> getFct = std::bind(&TestParameterObject::getDoubleValue, this);
    ParameterBase::SetFunc<double> setFct =
            std::bind(&TestParameterObject::setDoubleValue, this, std::placeholders::_1);
    MY_DOUBLE_PARAMETER = createNumericParameter<double>("doubleParam", "Double param", getFct, setFct);
    setGroup(MY_DOUBLE_PARAMETER, "ParameterGroup");
    setDescription(MY_DOUBLE_PARAMETER, "Help text");
    auto* doubleParam = static_cast<DoubleParameter*>(getParameter(MY_DOUBLE_PARAMETER));
    doubleParam->setMinValue(0.1);
    doubleParam->setMaxValue(0.9);

    m_enumValue = 1;
    MY_ENUM_PARAMETER = createEnumParameter("enumParam", "Enum param", &m_enumValue);
    auto* enumParam = static_cast<EnumParameter*>(getParameter(MY_ENUM_PARAMETER));
    enumParam->addEnumValue("enum1", MY_ENUM_VALUE1);
    enumParam->addEnumValue("enum2", MY_ENUM_VALUE2);
    enumParam->setGroup("EnumGroup");
    enumParam->setDescription("This is an enum.");

    m_enumValue2 = 0;
    ParameterBase::GetFunc<int> getFct2 = std::bind(&TestParameterObject::getEnumValue2, this);
    MY_ENUM_PARAMETER2 = createEnumParameter("enumParam2", "Enum param 2", getFct2);
    auto* enumParam2 = static_cast<EnumParameter*>(getParameter(MY_ENUM_PARAMETER2));
    enumParam2->addEnumValue("enum3", MY_ENUM2_VALUE1);
    enumParam2->addEnumValue("enum4", MY_ENUM2_VALUE2);
    enumParam2->setGroup("EnumGroup");
    setGroup(MY_ENUM_PARAMETER2, "ParameterGroup");
    setDescription(MY_ENUM_PARAMETER2, "Help text");
    setReadOnly(MY_ENUM_PARAMETER2,
                false);  // Has no influence. Since there is no set function, this must be read-only.

    m_strValue = "test string";
    MY_STRING_PARAMETER = createStringParameter("strParam", "String param", &m_strValue);
    setGroup(MY_STRING_PARAMETER, "ParameterGroup");
    setDescription(MY_STRING_PARAMETER, "Help text");

    m_vec3Value[0] = 1.1;
    m_vec3Value[1] = 2.2;
    m_vec3Value[2] = -3.5;
    MY_VEC3_PARAMETER = createVectorParameter("vec3Param", "Vector3 param", 3, &m_vec3Value[0]);
    setGroup(MY_VEC3_PARAMETER, "ParameterGroup");
    setDescription(MY_VEC3_PARAMETER, "Help text");

    MY_FUNCTION_PARAMETER =
            createFunctionParameter("funcParam", "Function param", [&]() { setCounter(getCounter() + 1); });
    setGroup(MY_FUNCTION_PARAMETER, "ParameterGroup");
    setDescription(MY_FUNCTION_PARAMETER, "Help text");
}
