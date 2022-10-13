//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <DetourTileCacheBuilder.h>

namespace vox::debug {

enum DrawNavMeshFlags {
    DRAW_NAVMESH_OFF_MESH_CONS = 0x01,
    DRAW_NAVMESH_CLOSED_LIST = 0x02,
    DRAW_NAVMESH_COLOR_TILES = 0x04,
};

void debugDrawNavMesh(struct debugDraw* dd, const dtNavMesh& mesh, unsigned char flags);
void debugDrawNavMeshWithClosedList(struct debugDraw* dd,
                                    const dtNavMesh& mesh,
                                    const dtNavMeshQuery& query,
                                    unsigned char flags);
void debugDrawNavMeshNodes(struct debugDraw* dd, const dtNavMeshQuery& query);
void debugDrawNavMeshBVTree(struct debugDraw* dd, const dtNavMesh& mesh);
void debugDrawNavMeshPortals(struct debugDraw* dd, const dtNavMesh& mesh);
void debugDrawNavMeshPolysWithFlags(struct debugDraw* dd,
                                    const dtNavMesh& mesh,
                                    unsigned short polyFlags,
                                    unsigned int col);
void debugDrawNavMeshPoly(struct DebugDraw* dd, const dtNavMesh& mesh, dtPolyRef ref, unsigned int col);

void debugDrawTileCacheLayerAreas(struct debugDraw* dd, const dtTileCacheLayer& layer, float cs, float ch);
void debugDrawTileCacheLayerRegions(struct debugDraw* dd, const dtTileCacheLayer& layer, float cs, float ch);
void debugDrawTileCacheContours(
        debugDraw* dd, const struct dtTileCacheContourSet& lcset, const float* orig, float cs, float ch);
void debugDrawTileCachePolyMesh(
        debugDraw* dd, const struct dtTileCachePolyMesh& lmesh, const float* orig, float cs, float ch);

}  // namespace vox::debug
