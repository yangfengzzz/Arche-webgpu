//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/navigation/navigation_manager.h"

namespace vox::nav {
NavigationManager::NavigationManager(Entity* entity)
    : Script(entity),
      m_geom(nullptr),
      m_navMesh(nullptr),
      m_navQuery(nullptr),
      m_crowd(nullptr),
      m_navMeshDrawFlags(debug::DRAW_NAVMESH_OFF_MESH_CONS | debug::DRAW_NAVMESH_CLOSED_LIST),
      m_filterLowHangingObstacles(true),
      m_filterLedgeSpans(true),
      m_filterWalkableLowHeightSpans(true),
      m_tool(nullptr),
      m_ctx(nullptr) {
    resetCommonSettings();
    m_navQuery = dtAllocNavMeshQuery();
    m_crowd = dtAllocCrowd();
    m_toolStates.clear();
}

NavigationManager::~NavigationManager() {
    dtFreeNavMeshQuery(m_navQuery);
    dtFreeNavMesh(m_navMesh);
    dtFreeCrowd(m_crowd);
    delete m_tool;
    m_toolStates.clear();
}

void NavigationManager::setTool(NavigationTool* tool) {
    delete m_tool;
    m_tool = tool;
    if (tool) m_tool->init(this);
}

void NavigationManager::handleSettings() {}

void NavigationManager::handleTools() {}

void NavigationManager::handleDebugMode() {}

void NavigationManager::handleRender() {
    if (!m_geom) return;

    // Draw mesh
    debug::debugDrawTriMesh(m_dd.get(), m_geom->getMesh()->getVerts(), m_geom->getMesh()->getVertCount(),
                            m_geom->getMesh()->getTris(), m_geom->getMesh()->getNormals(),
                            m_geom->getMesh()->getTriCount(), nullptr, 1.0f);
    // Draw bounds
    const float* bmin = m_geom->getMeshBoundsMin();
    const float* bmax = m_geom->getMeshBoundsMax();
    debug::debugDrawBoxWire(m_dd.get(), bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2],
                            debug::int2RGBA(255, 255, 255, 128), 1.0f);
}

void NavigationManager::handleMeshChanged(InputGeom* geom) {
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
        m_partitionType = (PartitionType)buildSettings->partitionType;
    }
}

void NavigationManager::collectSettings(BuildSettings& settings) {
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

void NavigationManager::resetCommonSettings() {
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
    m_partitionType = PartitionType::PARTITION_LAYERS;
}

void NavigationManager::handleClick(const float* s, const float* p, bool shift) {
    if (m_tool) m_tool->handleClick(s, p, shift);
}

void NavigationManager::handleToggle() {
    if (m_tool) m_tool->handleToggle();
}

void NavigationManager::handleStep() {
    if (m_tool) m_tool->handleStep();
}

bool NavigationManager::handleBuild() { return true; }

void NavigationManager::handleUpdate(const float dt) {
    if (m_tool) m_tool->handleUpdate(dt);
    updateToolStates(dt);
}

void NavigationManager::updateToolStates(const float dt) {
    for (auto& m_toolState : m_toolStates) {
        if (m_toolState) m_toolState->handleUpdate(dt);
    }
}

void NavigationManager::initToolStates(NavigationManager* sample) {
    for (auto& m_toolState : m_toolStates) {
        if (m_toolState) m_toolState->init(sample);
    }
}

void NavigationManager::resetToolStates() {
    for (auto& m_toolState : m_toolStates) {
        if (m_toolState) m_toolState->reset();
    }
}

void NavigationManager::renderToolStates() {
    for (auto& m_toolState : m_toolStates) {
        if (m_toolState) m_toolState->handleRender();
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

dtNavMesh* NavigationManager::loadAll(const char* path) {
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

void NavigationManager::saveAll(const char* path, const dtNavMesh* mesh) {
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