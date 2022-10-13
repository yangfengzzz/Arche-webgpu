//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <DetourCrowd.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <Recast.h>

#include "vox.render/script.h"
#include "vox.toolkit/debug_draw/debug_draw.h"
#include "vox.toolkit/debug_draw/detour_debug_draw.h"
#include "vox.toolkit/debug_draw/recast_debug_draw.h"
#include "vox.toolkit/navigation/geometry/input_geometry.h"
#include "vox.toolkit/navigation/navigation_context.h"

namespace vox::nav {
/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.
enum PolyAreas {
    POLY_AREA_GROUND,
    POLY_AREA_WATER,
    POLY_AREA_ROAD,
    POLY_AREA_DOOR,
    POLY_AREA_GRASS,
    POLY_AREA_JUMP,
};
enum PolyFlags {
    POLY_FLAGS_WALK = 0x01,      // Ability to walk (ground, grass, road)
    POLY_FLAGS_SWIM = 0x02,      // Ability to swim (water).
    POLY_FLAGS_DOOR = 0x04,      // Ability to move through doors.
    POLY_FLAGS_JUMP = 0x08,      // Ability to jump.
    POLY_FLAGS_DISABLED = 0x10,  // Disabled polygon
    POLY_FLAGS_ALL = 0xffff      // All abilities.
};

enum PartitionType {
    PARTITION_WATERSHED,
    PARTITION_MONOTONE,
    PARTITION_LAYERS,
};

struct NavigationTool {
    virtual ~NavigationTool() = default;
    virtual int type() = 0;
    virtual void init(class NavigationManager* sample) = 0;
    virtual void reset() = 0;
    virtual void handleMenu() = 0;
    virtual void handleClick(const float* s, const float* p, bool shift) = 0;
    virtual void handleRender() = 0;
    virtual void handleRenderOverlay(double* proj, double* model, int* view) = 0;
    virtual void handleToggle() = 0;
    virtual void handleStep() = 0;
    virtual void handleUpdate(float dt) = 0;
};

struct NavigationToolState {
    virtual ~NavigationToolState() = default;
    virtual void init(class NavigationManager* sample) = 0;
    virtual void reset() = 0;
    virtual void handleRender() = 0;
    virtual void handleRenderOverlay(double* proj, double* model, int* view) = 0;
    virtual void handleUpdate(float dt) = 0;
};

class NavigationManager : public Script {
public:
    explicit NavigationManager(Entity* entity);
    ~NavigationManager() override;

    void setContext(NavContext* ctx) { m_ctx = ctx; }

    void setTool(NavigationTool* tool);
    NavigationToolState& getToolState(int type) { return *m_toolStates[type]; }
    void setToolState(int type, std::unique_ptr<NavigationToolState>&& s) { m_toolStates[type] = std::move(s); }

    debug::DebugDraw& getDebugDraw() { return *m_dd; }

    virtual void handleSettings();
    virtual void handleTools();
    virtual void handleDebugMode();
    virtual void handleClick(const float* s, const float* p, bool shift);
    virtual void handleToggle();
    virtual void handleStep();
    virtual void handleRender();
    virtual void handleRenderOverlay(double* proj, double* model, int* view);
    virtual void handleMeshChanged(InputGeom* geom);
    virtual bool handleBuild();
    virtual void handleUpdate(float dt);
    virtual void collectSettings(struct BuildSettings& settings);

    virtual InputGeom* getInputGeom() { return m_geom; }
    virtual dtNavMesh* getNavMesh() { return m_navMesh; }
    virtual dtNavMeshQuery* getNavMeshQuery() { return m_navQuery; }
    virtual dtCrowd* getCrowd() { return m_crowd; }
    virtual float getAgentRadius() { return m_agentRadius; }
    virtual float getAgentHeight() { return m_agentHeight; }
    virtual float getAgentClimb() { return m_agentMaxClimb; }

    [[nodiscard]] unsigned char getNavMeshDrawFlags() const { return m_navMeshDrawFlags; }
    void setNavMeshDrawFlags(unsigned char flags) { m_navMeshDrawFlags = flags; }

    void updateToolStates(float dt);
    void initToolStates(NavigationManager* sample);
    void resetToolStates();
    void renderToolStates();
    void renderOverlayToolStates(double* proj, double* model, int* view);

    void resetCommonSettings();

protected:
    InputGeom* m_geom;
    dtNavMesh* m_navMesh;
    dtNavMeshQuery* m_navQuery;
    dtCrowd* m_crowd;

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
    PartitionType m_partitionType;

    bool m_filterLowHangingObstacles;
    bool m_filterLedgeSpans;
    bool m_filterWalkableLowHeightSpans;

    NavigationTool* m_tool;
    std::vector<std::unique_ptr<NavigationToolState>> m_toolStates;

    NavContext* m_ctx;

    std::unique_ptr<debug::DebugDraw> m_dd;

    static dtNavMesh* loadAll(const char* path);
    static void saveAll(const char* path, const dtNavMesh* mesh);

private:
    // Explicitly disabled copy constructor and copy assignment operator.
    NavigationManager(const NavigationManager&);
    NavigationManager& operator=(const NavigationManager&);
};

}  // namespace vox::nav