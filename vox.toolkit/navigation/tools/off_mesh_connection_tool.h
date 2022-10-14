//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.toolkit/navigation/navigation_manager.h"

namespace vox::nav {
// Tool to create off-mesh connection for InputGeom
class OffMeshConnectionTool : public NavigationTool {
public:
    OffMeshConnectionTool();
    ~OffMeshConnectionTool() override;

    void init(NavigationManager* sample) override;
    void reset() override;
    void handleClick(const float* s, const float* p, bool shift) override;
    void handleRender() override;

private:
    NavigationManager* m_sample;
    float m_hitPos[3]{};
    bool m_hitPosSet;
    bool m_bidir;
    unsigned char m_oldFlags;
};

}  // namespace vox::nav