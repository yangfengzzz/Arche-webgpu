//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "vox.toolkit/navigation/navigation_manager.h"

namespace vox::nav {
class NavMeshTesterTool : public NavigationTool {
public:
    NavMeshTesterTool();

    void init(NavigationManager* sample) override;
    void reset() override;
    void handleClick(const float* s, const float* p, bool shift) override;
    void handleToggle() override;
    void handleStep() override;
    void handleUpdate(float dt) override;
    void handleRender() override;

    void recalc();
    void drawAgent(const float* pos, float r, float h, float c, unsigned int col);

private:
    NavigationManager* m_sample;

    dtNavMesh* m_navMesh;
    dtNavMeshQuery* m_navQuery;

    dtQueryFilter m_filter;

    dtStatus m_pathFindStatus;

    enum ToolMode {
        TOOLMODE_PATHFIND_FOLLOW,
        TOOLMODE_PATHFIND_STRAIGHT,
        TOOLMODE_PATHFIND_SLICED,
        TOOLMODE_RAYCAST,
        TOOLMODE_DISTANCE_TO_WALL,
        TOOLMODE_FIND_POLYS_IN_CIRCLE,
        TOOLMODE_FIND_POLYS_IN_SHAPE,
        TOOLMODE_FIND_LOCAL_NEIGHBOURHOOD,
    };

    ToolMode m_toolMode;

    int m_straightPathOptions;

    static const int MAX_POLYS = 256;
    static const int MAX_SMOOTH = 2048;

    dtPolyRef m_startRef;
    dtPolyRef m_endRef;
    dtPolyRef m_polys[MAX_POLYS]{};
    dtPolyRef m_parent[MAX_POLYS]{};
    int m_npolys;
    float m_straightPath[MAX_POLYS * 3]{};
    unsigned char m_straightPathFlags[MAX_POLYS]{};
    dtPolyRef m_straightPathPolys[MAX_POLYS]{};
    int m_nstraightPath;
    float m_polyPickExt[3]{};
    float m_smoothPath[MAX_SMOOTH * 3]{};
    int m_nsmoothPath;
    float m_queryPoly[4 * 3]{};

    static const int MAX_RAND_POINTS = 64;
    float m_randPoints[MAX_RAND_POINTS * 3]{};
    int m_nrandPoints;
    bool m_randPointsInCircle;

    float m_spos[3]{};
    float m_epos[3]{};
    float m_hitPos[3]{};
    float m_hitNormal[3]{};
    bool m_hitResult;
    float m_distanceToWall;
    float m_neighbourhoodRadius;
    float m_randomRadius;
    bool m_sposSet;
    bool m_eposSet;

    int m_pathIterNum;
    dtPolyRef m_pathIterPolys[MAX_POLYS]{};
    int m_pathIterPolyCount;
    float m_prevIterPos[3]{}, m_iterPos[3]{}, m_steerPos[3]{}, m_targetPos[3]{};

    static const int MAX_STEER_POINTS = 10;
    float m_steerPoints[MAX_STEER_POINTS * 3]{};
    int m_steerPointCount;
};

}  // namespace vox::nav