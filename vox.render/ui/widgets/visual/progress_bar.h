//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef progress_bar_hpp
#define progress_bar_hpp

#include "ui/widgets/widget.h"
#include "vector2.h"

namespace vox {
namespace ui {
/**
 * Simple widget that display a progress bar
 */
class ProgressBar : public Widget {
public:
    /**
     * Constructor
     * @param p_fraction p_fraction
     * @param p_size p_size
     * @param p_overlay p_overlay
     */
    ProgressBar(float p_fraction = 0.0f, const Vector2F &p_size = {0.0f, 0.0f},
                const std::string &p_overlay = "");
    
protected:
    void _draw_Impl() override;
    
public:
    float fraction;
    Vector2F size;
    std::string overlay;
};

}
}
#endif /* progress_bar_hpp */
