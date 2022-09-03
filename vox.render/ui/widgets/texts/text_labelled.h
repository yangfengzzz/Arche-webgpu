//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/texts/text.h"

namespace vox::ui {
/**
 * Simple widget to display a labelled text on a panel
 */
class TextLabelled : public Text {
public:
    explicit TextLabelled(const std::string &content = "", std::string label = "");

protected:
    void DrawImpl() override;

public:
    std::string label_;
};

}  // namespace vox::ui
