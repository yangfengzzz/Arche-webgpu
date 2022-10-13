//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox::nav {
struct rcChunkyTriMeshNode {
    float bmin[2];
    float bmax[2];
    int i;
    int n;
};

struct rcChunkyTriMesh {
    inline rcChunkyTriMesh() : nodes(nullptr), nnodes(0), tris(nullptr), ntris(0), maxTrisPerChunk(0){};
    inline ~rcChunkyTriMesh() {
        delete[] nodes;
        delete[] tris;
    }

    rcChunkyTriMeshNode* nodes;
    int nnodes;
    int* tris;
    int ntris;
    int maxTrisPerChunk;

    // Explicitly disabled copy constructor and copy assignment operator.
    rcChunkyTriMesh(const rcChunkyTriMesh&) = delete;
    rcChunkyTriMesh& operator=(const rcChunkyTriMesh&) = delete;
};

/// Creates partitioned triangle mesh (AABB tree),
/// where each node contains at max trisPerChunk triangles.
bool rcCreateChunkyTriMesh(const float* verts, const int* tris, int ntris, int trisPerChunk, rcChunkyTriMesh* cm);

/// Returns the chunk indices which overlap the input rectable.
int rcGetChunksOverlappingRect(const rcChunkyTriMesh* cm, float bmin[2], float bmax[2], int* ids, int maxIds);

/// Returns the chunk indices which overlap the input segment.
int rcGetChunksOverlappingSegment(const rcChunkyTriMesh* cm, float p[2], float q[2], int* ids, int maxIds);

}  // namespace vox::nav