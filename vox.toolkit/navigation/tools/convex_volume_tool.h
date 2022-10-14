//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.toolkit/navigation/navigation_manager.h"

namespace vox::nav {
class ConvexVolumeTool : public NavigationTool {
public:
    static constexpr int MAX_PTS = 12;

    void init(NavigationManager* sample) override;
    void reset() override;
    void handleClick(const float* s, const float* p, bool shift) override;
    void handleRender() override;

private:
    NavigationManager* m_sample;
    PolyAreas m_areaType = PolyAreas::POLY_AREA_GRASS;
    float m_polyOffset = 0.0f;
    float m_boxHeight = 6.0f;
    float m_boxDescent = 1.0f;

    float m_pts[MAX_PTS * 3];
    int m_npts = 0;
    int m_hull[MAX_PTS];
    int m_nhull = 0;
};
}  // namespace vox::nav