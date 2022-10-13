//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.toolkit/navigation/base_tool.h"
#include "vox.toolkit/navigation/input_geom.h"

namespace vox::nav {
class ConvexVolumeTool : public BaseTool {
public:
    static constexpr int MAX_PTS = 12;

    InputGeom* geom{nullptr};

    void handleClick(const float* s, const float* p, bool shift) override;

    void handleRender() override;

private:
    int m_areaType;
    float m_polyOffset;
    float m_boxHeight;
    float m_boxDescent;

    float m_pts[MAX_PTS * 3];
    int m_npts;
    int m_hull[MAX_PTS];
    int m_nhull;
};
}  // namespace vox::nav