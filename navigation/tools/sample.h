//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <DetourCrowd.h>
#include <DetourDebugDraw.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <Recast.h>
#include <RecastDebugDraw.h>

#include <memory>

namespace vox {
/// Tool types.
enum SampleToolType {
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

/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.
enum SamplePolyAreas {
    SAMPLE_POLYAREA_GROUND,
    SAMPLE_POLYAREA_WATER,
    SAMPLE_POLYAREA_ROAD,
    SAMPLE_POLYAREA_DOOR,
    SAMPLE_POLYAREA_GRASS,
    SAMPLE_POLYAREA_JUMP,
};
enum SamplePolyFlags {
    SAMPLE_POLYFLAGS_WALK = 0x01,      // Ability to walk (ground, grass, road)
    SAMPLE_POLYFLAGS_SWIM = 0x02,      // Ability to swim (water).
    SAMPLE_POLYFLAGS_DOOR = 0x04,      // Ability to move through doors.
    SAMPLE_POLYFLAGS_JUMP = 0x08,      // Ability to jump.
    SAMPLE_POLYFLAGS_DISABLED = 0x10,  // Disabled polygon
    SAMPLE_POLYFLAGS_ALL = 0xffff      // All abilities.
};

enum SamplePartitionType {
    SAMPLE_PARTITION_WATERSHED,
    SAMPLE_PARTITION_MONOTONE,
    SAMPLE_PARTITION_LAYERS,
};

struct SampleTool {
    virtual ~SampleTool() = default;
    virtual int type() = 0;
    virtual void init(class Sample* sample) = 0;
    virtual void reset() = 0;
    virtual void handleMenu() = 0;
    virtual void handleClick(const float* s, const float* p, bool shift) = 0;
    virtual void handleRender() = 0;
    virtual void handleRenderOverlay(double* proj, double* model, int* view) = 0;
    virtual void handleToggle() = 0;
    virtual void handleStep() = 0;
    virtual void handleUpdate(float dt) = 0;
};

struct SampleToolState {
    virtual ~SampleToolState() = default;
    virtual void init(class Sample* sample) = 0;
    virtual void reset() = 0;
    virtual void handleRender() = 0;
    virtual void handleRenderOverlay(double* proj, double* model, int* view) = 0;
    virtual void handleUpdate(float dt) = 0;
};

class Sample {
protected:
    class InputGeom* m_geom;
    class dtNavMesh* m_navMesh;
    class dtNavMeshQuery* m_navQuery;
    class dtCrowd* m_crowd;

    unsigned char m_navMeshDrawFlags;

    float m_cellSize{};
    float m_cellHeight{};
    float m_agentHeight{};
    float m_agentRadius{};
    float m_agentMaxClimb{};
    float m_agentMaxSlope{};
    float m_regionMinSize{};
    float m_regionMergeSize{};
    float m_edgeMaxLen{};
    float m_edgeMaxError{};
    float m_vertsPerPoly{};
    float m_detailSampleDist{};
    float m_detailSampleMaxError{};
    int m_partitionType{};

    bool m_filterLowHangingObstacles;
    bool m_filterLedgeSpans;
    bool m_filterWalkableLowHeightSpans;

    SampleTool* m_tool;
    SampleToolState* m_toolStates[MAX_TOOLS]{};

    std::unique_ptr<duDebugDraw> m_dd{};

    dtNavMesh* loadAll(const char* path);
    void saveAll(const char* path, const dtNavMesh* mesh);

public:
    Sample();
    virtual ~Sample();

    void setTool(SampleTool* tool);
    SampleToolState* getToolState(int type) { return m_toolStates[type]; }
    void setToolState(int type, SampleToolState* s) { m_toolStates[type] = s; }

    duDebugDraw& getDebugDraw() { return *m_dd; }

    virtual void handleSettings();
    virtual void handleTools();
    virtual void handleDebugMode();
    virtual void handleClick(const float* s, const float* p, bool shift);
    virtual void handleToggle();
    virtual void handleStep();
    virtual void handleRender();
    virtual void handleRenderOverlay(double* proj, double* model, int* view);
    virtual void handleMeshChanged(class InputGeom* geom);
    virtual bool handleBuild();
    virtual void handleUpdate(float dt);
    virtual void collectSettings(struct BuildSettings& settings);

    virtual class InputGeom* getInputGeom() { return m_geom; }
    virtual class dtNavMesh* getNavMesh() { return m_navMesh; }
    virtual class dtNavMeshQuery* getNavMeshQuery() { return m_navQuery; }
    virtual class dtCrowd* getCrowd() { return m_crowd; }
    virtual float getAgentRadius() { return m_agentRadius; }
    virtual float getAgentHeight() { return m_agentHeight; }
    virtual float getAgentClimb() { return m_agentMaxClimb; }

    [[nodiscard]] unsigned char getNavMeshDrawFlags() const { return m_navMeshDrawFlags; }
    void setNavMeshDrawFlags(unsigned char flags) { m_navMeshDrawFlags = flags; }

    void updateToolStates(float dt);
    void initToolStates(Sample* sample);
    void resetToolStates();
    void renderToolStates();
    void renderOverlayToolStates(double* proj, double* model, int* view);

    void resetCommonSettings();
    void handleCommonSettings();

private:
    // Explicitly disabled copy constructor and copy assignment operator.
    Sample(const Sample&);
    Sample& operator=(const Sample&);
};

}  // namespace vox
