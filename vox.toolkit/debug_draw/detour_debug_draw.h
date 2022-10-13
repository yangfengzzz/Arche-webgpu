//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <DetourTileCacheBuilder.h>

#include "vox.toolkit/debug_draw/debug_draw.h"

namespace vox::debug {

enum DrawNavMeshFlags {
    DRAW_NAVMESH_OFF_MESH_CONS = 0x01,
    DRAW_NAVMESH_CLOSED_LIST = 0x02,
    DRAW_NAVMESH_COLOR_TILES = 0x04,
};

void debugDrawNavMesh(DebugDraw* dd, const dtNavMesh& mesh, unsigned char flags);
void debugDrawNavMeshWithClosedList(DebugDraw* dd,
                                    const dtNavMesh& mesh,
                                    const dtNavMeshQuery& query,
                                    unsigned char flags);
void debugDrawNavMeshNodes(DebugDraw* dd, const dtNavMeshQuery& query);
void debugDrawNavMeshBVTree(DebugDraw* dd, const dtNavMesh& mesh);
void debugDrawNavMeshPortals(DebugDraw* dd, const dtNavMesh& mesh);
void debugDrawNavMeshPolysWithFlags(DebugDraw* dd, const dtNavMesh& mesh, unsigned short polyFlags, unsigned int col);
void debugDrawNavMeshPoly(DebugDraw* dd, const dtNavMesh& mesh, dtPolyRef ref, unsigned int col);

void debugDrawTileCacheLayerAreas(DebugDraw* dd, const dtTileCacheLayer& layer, float cs, float ch);
void debugDrawTileCacheLayerRegions(DebugDraw* dd, const dtTileCacheLayer& layer, float cs, float ch);
void debugDrawTileCacheContours(
        DebugDraw* dd, const struct dtTileCacheContourSet& lcset, const float* orig, float cs, float ch);
void debugDrawTileCachePolyMesh(
        DebugDraw* dd, const struct dtTileCachePolyMesh& lmesh, const float* orig, float cs, float ch);

}  // namespace vox::debug
