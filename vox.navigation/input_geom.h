//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <DebugDraw.h>

#include "vox.navigation/chunky_tri_mesh.h"
#include "vox.navigation/mesh_loader_obj.h"

namespace vox {
static const int MAX_CONVEXVOL_PTS = 12;
struct ConvexVolume {
    float verts[MAX_CONVEXVOL_PTS * 3];
    float hmin, hmax;
    int nverts;
    int area;
};

struct BuildSettings {
    // Cell size in world units
    float cellSize;
    // Cell height in world units
    float cellHeight;
    // Agent height in world units
    float agentHeight;
    // Agent radius in world units
    float agentRadius;
    // Agent max climb in world units
    float agentMaxClimb;
    // Agent max slope in degrees
    float agentMaxSlope;
    // Region minimum size in voxels.
    // regionMinSize = sqrt(regionMinArea)
    float regionMinSize;
    // Region merge size in voxels.
    // regionMergeSize = sqrt(regionMergeArea)
    float regionMergeSize;
    // Edge max length in world units
    float edgeMaxLen;
    // Edge max error in voxels
    float edgeMaxError;
    float vertsPerPoly;
    // Detail sample distance in voxels
    float detailSampleDist;
    // Detail sample max error in voxel heights.
    float detailSampleMaxError;
    // Partition type, see SamplePartitionType
    int partitionType;
    // Bounds of the area to mesh
    float navMeshBMin[3];
    float navMeshBMax[3];
    // Size of the tiles in voxels
    float tileSize;
};

class InputGeom {
    rcChunkyTriMesh* m_chunkyMesh;
    rcMeshLoaderObj* m_mesh;
    float m_meshBMin[3]{}, m_meshBMax[3]{};
    BuildSettings m_buildSettings{};
    bool m_hasBuildSettings;

    /// @name Off-Mesh connections.
    ///@{
    static const int MAX_OFFMESH_CONNECTIONS = 256;
    float m_offMeshConVerts[MAX_OFFMESH_CONNECTIONS * 3 * 2]{};
    float m_offMeshConRads[MAX_OFFMESH_CONNECTIONS]{};
    unsigned char m_offMeshConDirs[MAX_OFFMESH_CONNECTIONS]{};
    unsigned char m_offMeshConAreas[MAX_OFFMESH_CONNECTIONS]{};
    unsigned short m_offMeshConFlags[MAX_OFFMESH_CONNECTIONS]{};
    unsigned int m_offMeshConId[MAX_OFFMESH_CONNECTIONS]{};
    int m_offMeshConCount;
    ///@}

    /// @name Convex Volumes.
    ///@{
    static const int MAX_VOLUMES = 256;
    ConvexVolume m_volumes[MAX_VOLUMES]{};
    int m_volumeCount;
    ///@}

    bool loadMesh(class rcContext* ctx, const std::string& filepath);
    bool loadGeomSet(class rcContext* ctx, const std::string& filepath);

public:
    InputGeom();
    ~InputGeom();

    bool load(class rcContext* ctx, const std::string& filepath);
    bool saveGeomSet(const BuildSettings* settings);

    /// Method to return static mesh data.
    [[nodiscard]] const rcMeshLoaderObj* getMesh() const { return m_mesh; }
    [[nodiscard]] const float* getMeshBoundsMin() const { return m_meshBMin; }
    [[nodiscard]] const float* getMeshBoundsMax() const { return m_meshBMax; }
    [[nodiscard]] const float* getNavMeshBoundsMin() const {
        return m_hasBuildSettings ? m_buildSettings.navMeshBMin : m_meshBMin;
    }
    [[nodiscard]] const float* getNavMeshBoundsMax() const {
        return m_hasBuildSettings ? m_buildSettings.navMeshBMax : m_meshBMax;
    }
    [[nodiscard]] const rcChunkyTriMesh* getChunkyMesh() const { return m_chunkyMesh; }
    [[nodiscard]] const BuildSettings* getBuildSettings() const {
        return m_hasBuildSettings ? &m_buildSettings : nullptr;
    }
    bool raycastMesh(float* src, float* dst, float& tmin);

    /// @name Off-Mesh connections.
    ///@{
    [[nodiscard]] int getOffMeshConnectionCount() const { return m_offMeshConCount; }
    [[nodiscard]] const float* getOffMeshConnectionVerts() const { return m_offMeshConVerts; }
    [[nodiscard]] const float* getOffMeshConnectionRads() const { return m_offMeshConRads; }
    [[nodiscard]] const unsigned char* getOffMeshConnectionDirs() const { return m_offMeshConDirs; }
    [[nodiscard]] const unsigned char* getOffMeshConnectionAreas() const { return m_offMeshConAreas; }
    [[nodiscard]] const unsigned short* getOffMeshConnectionFlags() const { return m_offMeshConFlags; }
    [[nodiscard]] const unsigned int* getOffMeshConnectionId() const { return m_offMeshConId; }
    void addOffMeshConnection(const float* spos,
                              const float* epos,
                              float rad,
                              unsigned char bidir,
                              unsigned char area,
                              unsigned short flags);
    void deleteOffMeshConnection(int i);
    void drawOffMeshConnections(struct duDebugDraw* dd, bool hilight = false);
    ///@}

    /// @name Box Volumes.
    ///@{
    [[nodiscard]] int getConvexVolumeCount() const { return m_volumeCount; }
    [[nodiscard]] const ConvexVolume* getConvexVolumes() const { return m_volumes; }
    void addConvexVolume(const float* verts, int nverts, float minh, float maxh, unsigned char area);
    void deleteConvexVolume(int i);
    void drawConvexVolumes(struct duDebugDraw* dd, bool hilight = false);
    ///@}

private:
    // Explicitly disabled copy constructor and copy assignment operator.
    InputGeom(const InputGeom&);
    InputGeom& operator=(const InputGeom&);
};
}  // namespace vox