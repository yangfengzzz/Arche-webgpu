//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <DetourNavMesh.h>
#include <Recast.h>

#include "vox.toolkit/navigation/navigation_manager.h"

namespace vox::nav {
class NavigationSoloMesh : public NavigationManager {
public:
    /// Tool types.
    enum ToolType {
        TOOL_NONE = 0,
        TOOL_TILE_EDIT,
        TOOL_TILE_HIGHLIGHT,
        TOOL_TEMP_OBSTACLE,
        TOOL_NAVMESH_TESTER,
        TOOL_NAVMESH_PRUNE,
        TOOL_OFFMESH_CONNECTION,
        TOOL_CONVEX_VOLUME,
        TOOL_CROWD,
        MAX_TOOLS
    };

    explicit NavigationSoloMesh(Entity* entity);
    ~NavigationSoloMesh() override;

    // Explicitly disabled copy constructor and copy assignment operator.
    NavigationSoloMesh(const NavigationSoloMesh&) = delete;
    NavigationSoloMesh& operator=(const NavigationSoloMesh&) = delete;

    void handleRender() override;
    void handleMeshChanged(class InputGeom* geom) override;
    bool handleBuild() override;

protected:
    bool m_keepInterResults;
    float m_totalBuildTimeMs;

    unsigned char* m_triareas;
    rcHeightfield* m_solid;
    rcCompactHeightfield* m_chf;
    rcContourSet* m_cset;
    rcPolyMesh* m_pmesh;
    rcConfig m_cfg{};
    rcPolyMeshDetail* m_dmesh;

    enum DrawMode {
        DRAWMODE_NAVMESH,
        DRAWMODE_NAVMESH_TRANS,
        DRAWMODE_NAVMESH_BVTREE,
        DRAWMODE_NAVMESH_NODES,
        DRAWMODE_NAVMESH_INVIS,
        DRAWMODE_MESH,
        DRAWMODE_VOXELS,
        DRAWMODE_VOXELS_WALKABLE,
        DRAWMODE_COMPACT,
        DRAWMODE_COMPACT_DISTANCE,
        DRAWMODE_COMPACT_REGIONS,
        DRAWMODE_REGION_CONNECTIONS,
        DRAWMODE_RAW_CONTOURS,
        DRAWMODE_BOTH_CONTOURS,
        DRAWMODE_CONTOURS,
        DRAWMODE_POLYMESH,
        DRAWMODE_POLYMESH_DETAIL,
        MAX_DRAWMODE
    };

    DrawMode m_drawMode;

    void cleanup();
};

}  // namespace vox::nav