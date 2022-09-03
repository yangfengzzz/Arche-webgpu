//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <gtest/gtest.h>

#include "test_parameter_object.h"

using namespace vox::utility;

TEST(ParamterTest, Constructor) {
    TestParameterObject tc;
    EXPECT_EQ(tc.numParameters(), 8);

    auto boolParam = static_cast<BoolParameter*>(tc.getParameter(TestParameterObject::MY_BOOL_PARAMETER));
    EXPECT_EQ(boolParam->getValue(), true);
    EXPECT_EQ(tc.getValue<bool>(TestParameterObject::MY_BOOL_PARAMETER), true);
    EXPECT_EQ(tc.getType(TestParameterObject::MY_BOOL_PARAMETER), ParameterBase::BOOL);

    auto intParam = static_cast<IntParameter*>(tc.getParameter(TestParameterObject::MY_INT_PARAMETER));
    EXPECT_EQ(intParam->getValue(), 3);
    EXPECT_EQ(tc.getValue<int>(TestParameterObject::MY_INT_PARAMETER), 3);
    EXPECT_EQ(tc.getType(TestParameterObject::MY_INT_PARAMETER), ParameterBase::INT32);
    // check max
    tc.setValue<int>(TestParameterObject::MY_INT_PARAMETER, 5);
    EXPECT_EQ(tc.getValue<int>(TestParameterObject::MY_INT_PARAMETER), 4);
    // check min
    tc.setValue<int>(TestParameterObject::MY_INT_PARAMETER, -2);
    EXPECT_EQ(tc.getValue<int>(TestParameterObject::MY_INT_PARAMETER), 2);

    auto doubleParam = static_cast<DoubleParameter*>(tc.getParameter(TestParameterObject::MY_DOUBLE_PARAMETER));
    EXPECT_EQ(doubleParam->getValue(), 3.123);
    EXPECT_EQ(tc.getValue<double>(TestParameterObject::MY_DOUBLE_PARAMETER), 3.123);
    tc.setValue<double>(TestParameterObject::MY_DOUBLE_PARAMETER, 4.85);
    EXPECT_EQ(tc.getValue<double>(TestParameterObject::MY_DOUBLE_PARAMETER), 4.85);

    EXPECT_EQ(doubleParam->getMinValue(), 0.1);
    EXPECT_EQ(doubleParam->getMaxValue(), 0.9);
    EXPECT_EQ(tc.getType(TestParameterObject::MY_DOUBLE_PARAMETER), ParameterBase::DOUBLE);

    auto enumParam = static_cast<EnumParameter*>(tc.getParameter(TestParameterObject::MY_ENUM_PARAMETER));
    EXPECT_EQ(enumParam->getValue(), TestParameterObject::MY_ENUM_VALUE2);
    EXPECT_EQ(tc.getValue<int>(TestParameterObject::MY_ENUM_PARAMETER), TestParameterObject::MY_ENUM_VALUE2);
    const std::vector<EnumParameter::EnumValue>& enumValues = enumParam->getEnumValues();
    EXPECT_EQ(enumValues[0].name, "enum1");
    EXPECT_EQ(enumValues[1].name, "enum2");
    EXPECT_EQ(tc.getGroup(TestParameterObject::MY_ENUM_PARAMETER), "EnumGroup");
    EXPECT_EQ(tc.getType(TestParameterObject::MY_ENUM_PARAMETER), ParameterBase::ENUM);

    auto enumParam2 = static_cast<EnumParameter*>(tc.getParameter(TestParameterObject::MY_ENUM_PARAMETER2));
    EXPECT_EQ(enumParam2->getValue(), TestParameterObject::MY_ENUM2_VALUE1);
    EXPECT_EQ(tc.getValue<int>(TestParameterObject::MY_ENUM_PARAMETER2), TestParameterObject::MY_ENUM2_VALUE1);
    const std::vector<EnumParameter::EnumValue>& enumValues2 = enumParam2->getEnumValues();
    EXPECT_EQ(enumValues2[0].name, "enum3");
    EXPECT_EQ(enumValues2[1].name, "enum4");
    EXPECT_EQ(tc.getGroup(TestParameterObject::MY_ENUM_PARAMETER2), "ParameterGroup");
    EXPECT_EQ(tc.getType(TestParameterObject::MY_ENUM_PARAMETER2), ParameterBase::ENUM);
    EXPECT_EQ(tc.getReadOnly(TestParameterObject::MY_ENUM_PARAMETER2), true);
    tc.setValue<int>(TestParameterObject::MY_ENUM_PARAMETER2, TestParameterObject::MY_ENUM2_VALUE2);
    EXPECT_EQ(tc.getValue<int>(TestParameterObject::MY_ENUM_PARAMETER2), TestParameterObject::MY_ENUM2_VALUE1);

    EXPECT_EQ(tc.getValue<std::string>(TestParameterObject::MY_STRING_PARAMETER), "test string");
    EXPECT_EQ(tc.getType(TestParameterObject::MY_STRING_PARAMETER), ParameterBase::STRING);

    auto vec3Param = static_cast<DoubleVectorParameter*>(tc.getParameter(TestParameterObject::MY_VEC3_PARAMETER));
    double* vecValue;
    vecValue = vec3Param->getValue();
    EXPECT_EQ(vecValue[0], 1.1);
    EXPECT_EQ(vecValue[1], 2.2);
    EXPECT_EQ(vecValue[2], -3.5);
    EXPECT_EQ(vecValue, tc.getVecValue<double>(TestParameterObject::MY_VEC3_PARAMETER));
    double newVec[3] = {2, 3, 4.5};
    tc.setVecValue<double>(TestParameterObject::MY_VEC3_PARAMETER, newVec);
    vecValue = vec3Param->getValue();
    EXPECT_EQ(vecValue[0], 2.0);
    EXPECT_EQ(vecValue[1], 3);
    EXPECT_EQ(vecValue[2], 4.5);
    EXPECT_EQ(tc.getType(TestParameterObject::MY_VEC3_PARAMETER), ParameterBase::VEC_DOUBLE);

    tc.setCounter(0);
    auto funcParam = static_cast<FunctionParameter*>(tc.getParameter(TestParameterObject::MY_FUNCTION_PARAMETER));
    funcParam->callFunction();
    EXPECT_EQ(tc.getCounter(), 1);
    tc.callFunction(TestParameterObject::MY_FUNCTION_PARAMETER);
    EXPECT_EQ(tc.getCounter(), 2);
    EXPECT_EQ(tc.getType(TestParameterObject::MY_FUNCTION_PARAMETER), ParameterBase::FUNCTION);
}
