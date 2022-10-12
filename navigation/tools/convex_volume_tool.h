//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "navigation/tools/sample.h"

namespace vox {
// Tool to create convex volumes for InputGeom
class ConvexVolumeTool : public SampleTool {
    Sample* m_sample;
    int m_areaType;
    float m_polyOffset;
    float m_boxHeight;
    float m_boxDescent;

    static const int MAX_PTS = 12;
    float m_pts[MAX_PTS * 3]{};
    int m_npts;
    int m_hull[MAX_PTS]{};
    int m_nhull;

public:
    ConvexVolumeTool();

    int type() override { return TOOL_CONVEX_VOLUME; }
    void init(Sample* sample) override;
    void reset() override;
    void handleMenu() override;
    void handleClick(const float* s, const float* p, bool shift) override;
    void handleToggle() override;
    void handleStep() override;
    void handleUpdate(float dt) override;
    void handleRender() override;
    void handleRenderOverlay(double* proj, double* model, int* view) override;
};
}  // namespace vox