//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/data_widget.h"

namespace vox::ui {
/**
 * Simple widget to display text on a panel
 */
class Text : public DataWidget<std::string> {
public:
    explicit Text(std::string content = "");

protected:
    void DrawImpl() override;

public:
    std::string content_;
};

}  // namespace vox::ui
