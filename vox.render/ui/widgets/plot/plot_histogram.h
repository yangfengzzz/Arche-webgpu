//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef plot_histogram_hpp
#define plot_histogram_hpp

#include "plot.h"

namespace vox {
namespace ui {
/**
 * Plot displayed as an histogram
 */
class PlotHistogram : public Plot {
public:
    /**
     * Constructor
     * @param p_data p_data
     * @param p_minScale p_minScale
     * @param p_maxScale p_maxScale
     * @param p_size p_size
     * @param p_overlay p_overlay
     * @param p_label p_label
     */
    PlotHistogram(const std::vector<float> &p_data = std::vector<float>(),
                  float p_minScale = std::numeric_limits<float>::min(),
                  float p_maxScale = std::numeric_limits<float>::max(),
                  const Vector2F &p_size = {0.0f, 0.0f},
                  const std::string &p_overlay = "",
                  const std::string &p_label = "");
    
protected:
    void _draw_Impl() override;
};

}
}
#endif /* plot_histogram_hpp */
