//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <map>

#include "vox.render/event.h"
#include "vox.render/ui/widgets/data_widget.h"

namespace vox::ui {
/**
 * Widget that can display a list of values that the user can select
 */
class ComboBox : public DataWidget<int> {
public:
    explicit ComboBox(int current_choice = 0);

protected:
    void DrawImpl() override;

public:
    std::map<int, std::string> choices_;
    int current_choice_;

public:
    Event<int> value_changed_event_;
};

}  // namespace vox::ui
