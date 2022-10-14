//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.toolkit/navigation/navigation_manager.h"

namespace vox::nav {
// Prune navmesh to accessible locations from a point.
class NavMeshPruneTool : public NavigationTool {
public:
    NavMeshPruneTool();
    ~NavMeshPruneTool() override;

    // Explicitly disabled copy constructor and copy assignment operator.
    NavMeshPruneTool(const NavMeshPruneTool&) = delete;
    NavMeshPruneTool& operator=(const NavMeshPruneTool&) = delete;

    void init(NavigationManager* sample) override;
    void reset() override;
    void handleClick(const float* s, const float* p, bool shift) override;
    void handleToggle() override;
    void handleStep() override;
    void handleUpdate(float dt) override;
    void handleRender() override;

private:
    NavigationManager* m_sample;

    class NavmeshFlags* m_flags;

    float m_hitPos[3]{};
    bool m_hitPosSet;
};

}  // namespace vox::nav