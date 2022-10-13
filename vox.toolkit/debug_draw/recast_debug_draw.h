//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Recast.h>

#include "vox.toolkit/debug_draw/debug_draw.h"

namespace vox::debug {

void debugDrawTriMesh(DebugDraw* dd,
                      const float* verts,
                      int nverts,
                      const int* tris,
                      const float* normals,
                      int ntris,
                      const unsigned char* flags,
                      float texScale);
void debugDrawTriMeshSlope(DebugDraw* dd,
                           const float* verts,
                           int nverts,
                           const int* tris,
                           const float* normals,
                           int ntris,
                           float walkableSlopeAngle,
                           float texScale);

void debugDrawHeightfieldSolid(DebugDraw* dd, const struct rcHeightfield& hf);
void debugDrawHeightfieldWalkable(DebugDraw* dd, const struct rcHeightfield& hf);

void debugDrawCompactHeightfieldSolid(DebugDraw* dd, const struct rcCompactHeightfield& chf);
void debugDrawCompactHeightfieldRegions(DebugDraw* dd, const struct rcCompactHeightfield& chf);
void debugDrawCompactHeightfieldDistance(DebugDraw* dd, const struct rcCompactHeightfield& chf);

void debugDrawHeightfieldLayer(DebugDraw* dd, const struct rcHeightfieldLayer& layer, int idx);
void debugDrawHeightfieldLayers(DebugDraw* dd, const struct rcHeightfieldLayerSet& lset);
void debugDrawHeightfieldLayersRegions(DebugDraw* dd, const struct rcHeightfieldLayerSet& lset);

void debugDrawRegionConnections(DebugDraw* dd, const struct rcContourSet& cset, float alpha = 1.0f);
void debugDrawRawContours(DebugDraw* dd, const struct rcContourSet& cset, float alpha = 1.0f);
void debugDrawContours(DebugDraw* dd, const struct rcContourSet& cset, float alpha = 1.0f);
void debugDrawPolyMesh(DebugDraw* dd, const struct rcPolyMesh& mesh);
void debugDrawPolyMeshDetail(DebugDraw* dd, const struct rcPolyMeshDetail& dmesh);

}  // namespace vox::debug
