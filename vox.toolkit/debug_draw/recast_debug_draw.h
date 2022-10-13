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

void debugDrawHeightfieldSolid(struct debugDraw* dd, const struct rcHeightfield& hf);
void debugDrawHeightfieldWalkable(struct debugDraw* dd, const struct rcHeightfield& hf);

void debugDrawCompactHeightfieldSolid(struct debugDraw* dd, const struct rcCompactHeightfield& chf);
void debugDrawCompactHeightfieldRegions(struct debugDraw* dd, const struct rcCompactHeightfield& chf);
void debugDrawCompactHeightfieldDistance(struct debugDraw* dd, const struct rcCompactHeightfield& chf);

void debugDrawHeightfieldLayer(debugDraw* dd, const struct rcHeightfieldLayer& layer, int idx);
void debugDrawHeightfieldLayers(debugDraw* dd, const struct rcHeightfieldLayerSet& lset);
void debugDrawHeightfieldLayersRegions(debugDraw* dd, const struct rcHeightfieldLayerSet& lset);

void debugDrawRegionConnections(struct debugDraw* dd, const struct rcContourSet& cset, float alpha = 1.0f);
void debugDrawRawContours(struct debugDraw* dd, const struct rcContourSet& cset, float alpha = 1.0f);
void debugDrawContours(struct debugDraw* dd, const struct rcContourSet& cset, float alpha = 1.0f);
void debugDrawPolyMesh(struct debugDraw* dd, const struct rcPolyMesh& mesh);
void debugDrawPolyMeshDetail(struct debugDraw* dd, const struct rcPolyMeshDetail& dmesh);

}  // namespace vox::debug
