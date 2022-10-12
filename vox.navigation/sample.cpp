//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.navigation/sample.h"

#include <imgui.h>

#include <cstdio>

#include "vox.navigation/input_geom.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace vox::nav {

Sample::Sample()
    : m_geom(nullptr),
      m_navMesh(nullptr),
      m_navQuery(nullptr),
      m_crowd(nullptr),
      m_navMeshDrawFlags(DU_DRAWNAVMESH_OFFMESHCONS | DU_DRAWNAVMESH_CLOSEDLIST),
      m_filterLowHangingObstacles(true),
      m_filterLedgeSpans(true),
      m_filterWalkableLowHeightSpans(true),
      m_tool(nullptr) {
    resetCommonSettings();
    m_navQuery = dtAllocNavMeshQuery();
    m_crowd = dtAllocCrowd();

    for (auto& m_toolState : m_toolStates) m_toolState = nullptr;
}

Sample::~Sample() {
    dtFreeNavMeshQuery(m_navQuery);
    dtFreeNavMesh(m_navMesh);
    dtFreeCrowd(m_crowd);
    delete m_tool;
    for (auto& m_toolState : m_toolStates) delete m_toolState;
}

void Sample::setTool(SampleTool* tool) {
    delete m_tool;
    m_tool = tool;
    if (tool) m_tool->init(this);
}

void Sample::handleSettings() {}

void Sample::handleTools() {}

void Sample::handleDebugMode() {}

void Sample::handleRender() {
    if (!m_geom) return;

    //    // Draw mesh
    //    duDebugDrawTriMesh(&m_dd, m_geom->getMesh()->getVerts(), m_geom->getMesh()->getVertCount(),
    //                       m_geom->getMesh()->getTris(), m_geom->getMesh()->getNormals(),
    //                       m_geom->getMesh()->getTriCount(), 0, 1.0f);
    //    // Draw bounds
    //    const float* bmin = m_geom->getMeshBoundsMin();
    //    const float* bmax = m_geom->getMeshBoundsMax();
    //    duDebugDrawBoxWire(&m_dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], duRGBA(255, 255, 255,
    //    128), 1.0f);
}

void Sample::handleRenderOverlay(double* /*proj*/, double* /*model*/, int* /*view*/) {}

void Sample::handleMeshChanged(InputGeom* geom) {
    m_geom = geom;

    const BuildSettings* buildSettings = geom->getBuildSettings();
    if (buildSettings) {
        m_cellSize = buildSettings->cellSize;
        m_cellHeight = buildSettings->cellHeight;
        m_agentHeight = buildSettings->agentHeight;
        m_agentRadius = buildSettings->agentRadius;
        m_agentMaxClimb = buildSettings->agentMaxClimb;
        m_agentMaxSlope = buildSettings->agentMaxSlope;
        m_regionMinSize = buildSettings->regionMinSize;
        m_regionMergeSize = buildSettings->regionMergeSize;
        m_edgeMaxLen = buildSettings->edgeMaxLen;
        m_edgeMaxError = buildSettings->edgeMaxError;
        m_vertsPerPoly = buildSettings->vertsPerPoly;
        m_detailSampleDist = buildSettings->detailSampleDist;
        m_detailSampleMaxError = buildSettings->detailSampleMaxError;
        m_partitionType = buildSettings->partitionType;
    }
}

void Sample::collectSettings(BuildSettings& settings) {
    settings.cellSize = m_cellSize;
    settings.cellHeight = m_cellHeight;
    settings.agentHeight = m_agentHeight;
    settings.agentRadius = m_agentRadius;
    settings.agentMaxClimb = m_agentMaxClimb;
    settings.agentMaxSlope = m_agentMaxSlope;
    settings.regionMinSize = m_regionMinSize;
    settings.regionMergeSize = m_regionMergeSize;
    settings.edgeMaxLen = m_edgeMaxLen;
    settings.edgeMaxError = m_edgeMaxError;
    settings.vertsPerPoly = m_vertsPerPoly;
    settings.detailSampleDist = m_detailSampleDist;
    settings.detailSampleMaxError = m_detailSampleMaxError;
    settings.partitionType = m_partitionType;
}

void Sample::resetCommonSettings() {
    m_cellSize = 0.3f;
    m_cellHeight = 0.2f;
    m_agentHeight = 2.0f;
    m_agentRadius = 0.6f;
    m_agentMaxClimb = 0.9f;
    m_agentMaxSlope = 45.0f;
    m_regionMinSize = 8;
    m_regionMergeSize = 20;
    m_edgeMaxLen = 12.0f;
    m_edgeMaxError = 1.3f;
    m_vertsPerPoly = 6.0f;
    m_detailSampleDist = 6.0f;
    m_detailSampleMaxError = 1.0f;
    m_partitionType = SAMPLE_PARTITION_WATERSHED;
}

void Sample::handleCommonSettings() {
    ImGui::Text("Rasterization");
    ImGui::SliderFloat("Cell Size", &m_cellSize, 0.1f, 1.0f);
    ImGui::SliderFloat("Cell Height", &m_cellHeight, 0.1f, 1.0f);

    if (m_geom) {
        const float* bmin = m_geom->getNavMeshBoundsMin();
        const float* bmax = m_geom->getNavMeshBoundsMax();
        int gw = 0, gh = 0;
        rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
        char text[64];
        snprintf(text, 64, "Voxels  %d x %d", gw, gh);
        ImGui::Text("%s", text);
    }

    ImGui::Separator();
    ImGui::Text("Agent");
    ImGui::SliderFloat("Height", &m_agentHeight, 0.1f, 5.0f);
    ImGui::SliderFloat("Radius", &m_agentRadius, 0.0f, 5.0f);
    ImGui::SliderFloat("Max Climb", &m_agentMaxClimb, 0.1f, 5.0f);
    ImGui::SliderFloat("Max Slope", &m_agentMaxSlope, 0.0f, 90.0f);

    ImGui::Separator();
    ImGui::Text("Region");
    ImGui::SliderFloat("Min Region Size", &m_regionMinSize, 0.0f, 150.0f);
    ImGui::SliderFloat("Merged Region Size", &m_regionMergeSize, 0.0f, 150.0f);

    ImGui::Separator();
    ImGui::Text("Partitioning");
    if (ImGui::CheckboxFlags("Watershed", &m_partitionType, SAMPLE_PARTITION_WATERSHED))
        m_partitionType = SAMPLE_PARTITION_WATERSHED;
    if (ImGui::CheckboxFlags("Monotone", &m_partitionType, SAMPLE_PARTITION_MONOTONE))
        m_partitionType = SAMPLE_PARTITION_MONOTONE;
    if (ImGui::CheckboxFlags("Layers", &m_partitionType, SAMPLE_PARTITION_LAYERS))
        m_partitionType = SAMPLE_PARTITION_LAYERS;

    ImGui::Separator();
    ImGui::Text("Filtering");
    if (ImGui::Checkbox("Low Hanging Obstacles", &m_filterLowHangingObstacles))
        m_filterLowHangingObstacles = !m_filterLowHangingObstacles;
    if (ImGui::Checkbox("Ledge Spans", &m_filterLedgeSpans)) m_filterLedgeSpans = !m_filterLedgeSpans;
    if (ImGui::Checkbox("Walkable Low Height Spans", &m_filterWalkableLowHeightSpans))
        m_filterWalkableLowHeightSpans = !m_filterWalkableLowHeightSpans;

    ImGui::Separator();
    ImGui::Text("Polygonization");
    ImGui::SliderFloat("Max Edge Length", &m_edgeMaxLen, 0.0f, 50.0f);
    ImGui::SliderFloat("Max Edge Error", &m_edgeMaxError, 0.1f, 3.0f);
    ImGui::SliderFloat("Verts Per Poly", &m_vertsPerPoly, 3.0f, 12.0f);

    ImGui::Separator();
    ImGui::Text("Detail Mesh");
    ImGui::SliderFloat("Sample Distance", &m_detailSampleDist, 0.0f, 16.0f);
    ImGui::SliderFloat("Max Sample Error", &m_detailSampleMaxError, 0.0f, 16.0f);

    ImGui::Separator();
}

void Sample::handleClick(const float* s, const float* p, bool shift) {
    if (m_tool) m_tool->handleClick(s, p, shift);
}

void Sample::handleToggle() {
    if (m_tool) m_tool->handleToggle();
}

void Sample::handleStep() {
    if (m_tool) m_tool->handleStep();
}

bool Sample::handleBuild() { return true; }

void Sample::handleUpdate(const float dt) {
    if (m_tool) m_tool->handleUpdate(dt);
    updateToolStates(dt);
}

void Sample::updateToolStates(const float dt) {
    for (auto& m_toolState : m_toolStates) {
        if (m_toolState) m_toolState->handleUpdate(dt);
    }
}

void Sample::initToolStates(Sample* sample) {
    for (auto& m_toolState : m_toolStates) {
        if (m_toolState) m_toolState->init(sample);
    }
}

void Sample::resetToolStates() {
    for (auto& m_toolState : m_toolStates) {
        if (m_toolState) m_toolState->reset();
    }
}

void Sample::renderToolStates() {
    for (auto& m_toolState : m_toolStates) {
        if (m_toolState) m_toolState->handleRender();
    }
}

void Sample::renderOverlayToolStates(double* proj, double* model, int* view) {
    for (auto& m_toolState : m_toolStates) {
        if (m_toolState) m_toolState->handleRenderOverlay(proj, model, view);
    }
}

static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T';  //'MSET';
static const int NAVMESHSET_VERSION = 1;

struct NavMeshSetHeader {
    int magic;
    int version;
    int numTiles;
    dtNavMeshParams params;
};

struct NavMeshTileHeader {
    dtTileRef tileRef;
    int dataSize;
};

dtNavMesh* Sample::loadAll(const char* path) {
    FILE* fp = fopen(path, "rb");
    if (!fp) return nullptr;

    // Read header.
    NavMeshSetHeader header{};
    size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
    if (readLen != 1) {
        fclose(fp);
        return nullptr;
    }
    if (header.magic != NAVMESHSET_MAGIC) {
        fclose(fp);
        return nullptr;
    }
    if (header.version != NAVMESHSET_VERSION) {
        fclose(fp);
        return nullptr;
    }

    dtNavMesh* mesh = dtAllocNavMesh();
    if (!mesh) {
        fclose(fp);
        return nullptr;
    }
    dtStatus status = mesh->init(&header.params);
    if (dtStatusFailed(status)) {
        fclose(fp);
        return nullptr;
    }

    // Read tiles.
    for (int i = 0; i < header.numTiles; ++i) {
        NavMeshTileHeader tileHeader{};
        readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
        if (readLen != 1) {
            fclose(fp);
            return nullptr;
        }

        if (!tileHeader.tileRef || !tileHeader.dataSize) break;

        auto* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
        if (!data) break;
        memset(data, 0, tileHeader.dataSize);
        readLen = fread(data, tileHeader.dataSize, 1, fp);
        if (readLen != 1) {
            dtFree(data);
            fclose(fp);
            return nullptr;
        }

        mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, nullptr);
    }

    fclose(fp);

    return mesh;
}

void Sample::saveAll(const char* path, const dtNavMesh* mesh) {
    if (!mesh) return;

    FILE* fp = fopen(path, "wb");
    if (!fp) return;

    // Store header.
    NavMeshSetHeader header{};
    header.magic = NAVMESHSET_MAGIC;
    header.version = NAVMESHSET_VERSION;
    header.numTiles = 0;
    for (int i = 0; i < mesh->getMaxTiles(); ++i) {
        const dtMeshTile* tile = mesh->getTile(i);
        if (!tile || !tile->header || !tile->dataSize) continue;
        header.numTiles++;
    }
    memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
    fwrite(&header, sizeof(NavMeshSetHeader), 1, fp);

    // Store tiles.
    for (int i = 0; i < mesh->getMaxTiles(); ++i) {
        const dtMeshTile* tile = mesh->getTile(i);
        if (!tile || !tile->header || !tile->dataSize) continue;

        NavMeshTileHeader tileHeader{};
        tileHeader.tileRef = mesh->getTileRef(tile);
        tileHeader.dataSize = tile->dataSize;
        fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

        fwrite(tile->data, tile->dataSize, 1, fp);
    }

    fclose(fp);
}
}  // namespace vox::nav