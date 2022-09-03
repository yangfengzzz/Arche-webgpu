//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.base/reflect/parameter.h"

namespace vox::utility {
/** Class of a function parameter. This parameter type contains a callback function.
 */
class FunctionParameter : public ParameterBase {
public:
    using CallbackFunc = std::function<void()>;

protected:
    CallbackFunc m_function;

public:
    /** Constructor */
    FunctionParameter(const std::string& name, const std::string& label, CallbackFunc function)
        : ParameterBase(name, label, ParameterBase::FUNCTION), m_function(std::move(function)) {}

    void callFunction() {
        if (m_function != nullptr) m_function();
    }

    ~FunctionParameter() override = default;

    void setReadOnly(const bool val) override {}
};
}  // namespace vox::utility