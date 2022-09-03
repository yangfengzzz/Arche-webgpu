//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector2.h"
#include "vox.render/ui/widgets/widget.h"

namespace vox::ui {
/**
 * Simple widget that display a progress bar
 */
class ProgressBar : public Widget {
public:
    explicit ProgressBar(float fraction = 0.0f, const Vector2F &size = {0.0f, 0.0f}, std::string overlay = "");

protected:
    void DrawImpl() override;

public:
    float fraction_;
    Vector2F size_;
    std::string overlay_;
};

}  // namespace vox::ui
