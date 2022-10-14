//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <DetourCrowd.h>
#include <DetourNavMesh.h>
#include <DetourObstacleAvoidance.h>

#include "vox.toolkit/navigation/navigation_manager.h"
#include "vox.toolkit/navigation/tools/value_history.h"

namespace vox::nav {
// Tool to create crowds.
struct CrowdToolParams {
    bool m_expandSelectedDebugDraw;
    bool m_showCorners;
    bool m_showCollisionSegments;
    bool m_showPath;
    bool m_showVO;
    bool m_showOpt;
    bool m_showNeis;

    bool m_expandDebugDraw;
    bool m_showLabels;
    bool m_showGrid;
    bool m_showNodes;
    bool m_showPerfGraph;
    bool m_showDetailAll;

    bool m_expandOptions;
    bool m_anticipateTurns;
    bool m_optimizeVis;
    bool m_optimizeTopo;
    bool m_obstacleAvoidance;
    float m_obstacleAvoidanceType;
    bool m_separation;
    float m_separationWeight;
};

class CrowdToolState : public NavigationToolState {
public:
    CrowdToolState();
    ~CrowdToolState() override;

    // Explicitly disabled copy constructor and copy assignment operator.
    CrowdToolState(const CrowdToolState&) = delete;
    CrowdToolState& operator=(const CrowdToolState&) = delete;

    void init(NavigationManager* sample) override;
    void handleRender() override;
    void handleUpdate(float dt) override;

    [[nodiscard]] inline bool isRunning() const { return m_run; }
    inline void setRunning(const bool s) { m_run = s; }

    void addAgent(const float* pos);
    void removeAgent(int idx);
    void hilightAgent(int idx);
    void updateAgentParams();
    int hitTestAgents(const float* s, const float* p);
    void setMoveTarget(const float* p, bool adjust);
    void updateTick(float dt);

    inline CrowdToolParams* getToolParams() { return &m_toolParams; }

private:
    NavigationManager* m_sample;
    dtNavMesh* m_nav;
    dtCrowd* m_crowd;

    float m_targetPos[3]{};
    dtPolyRef m_targetRef;

    dtCrowdAgentDebugInfo m_agentDebug{};
    dtObstacleAvoidanceDebugData* m_vod;

    static const int AGENT_MAX_TRAIL = 64;
    static const int MAX_AGENTS = 128;
    struct AgentTrail {
        float trail[AGENT_MAX_TRAIL * 3];
        int htrail;
    };
    AgentTrail m_trails[MAX_AGENTS]{};

    ValueHistory m_crowdTotalTime;
    ValueHistory m_crowdSampleCount;

    CrowdToolParams m_toolParams{};

    bool m_run;
};

class CrowdTool : public NavigationTool {
public:
    CrowdTool();

    void init(NavigationManager* sample) override;
    void handleClick(const float* s, const float* p, bool shift) override;
    void handleToggle() override;
    void handleStep() override;
    void handleUpdate(float dt) override;

private:
    NavigationManager* m_sample;
    CrowdToolState* m_state;

    enum ToolMode {
        TOOL_MODE_CREATE,
        TOOL_MODE_MOVE_TARGET,
        TOOL_MODE_SELECT,
        TOOL_MODE_TOGGLE_POLYS,
    };
    ToolMode m_mode;
};

}  // namespace vox::nav
