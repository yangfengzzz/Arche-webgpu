//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/navigation/tools/nav_mesh_prune_tool.h"

#include <DetourAssert.h>
#include <DetourCommon.h>

#include <cstring>
#include <vector>

namespace vox::nav {
class NavmeshFlags {
    struct TileFlags {
        inline void purge() const { dtFree(flags); }
        unsigned char* flags;
        int nflags;
        dtPolyRef base;
    };

    const dtNavMesh* m_nav;
    TileFlags* m_tiles;
    int m_ntiles;

public:
    NavmeshFlags() : m_nav(nullptr), m_tiles(nullptr), m_ntiles(0) {}

    ~NavmeshFlags() {
        for (int i = 0; i < m_ntiles; ++i) m_tiles[i].purge();
        dtFree(m_tiles);
    }

    bool init(const dtNavMesh* nav) {
        m_ntiles = nav->getMaxTiles();
        if (!m_ntiles) return true;
        m_tiles = (TileFlags*)dtAlloc(sizeof(TileFlags) * m_ntiles, DT_ALLOC_TEMP);
        if (!m_tiles) {
            return false;
        }
        memset(m_tiles, 0, sizeof(TileFlags) * m_ntiles);

        // Alloc flags for each tile.
        for (int i = 0; i < nav->getMaxTiles(); ++i) {
            const dtMeshTile* tile = nav->getTile(i);
            if (!tile->header) continue;
            TileFlags* tf = &m_tiles[i];
            tf->nflags = tile->header->polyCount;
            tf->base = nav->getPolyRefBase(tile);
            if (tf->nflags) {
                tf->flags = (unsigned char*)dtAlloc(tf->nflags, DT_ALLOC_TEMP);
                if (!tf->flags) return false;
                memset(tf->flags, 0, tf->nflags);
            }
        }

        m_nav = nav;

        return false;
    }

    inline void clearAllFlags() {
        for (int i = 0; i < m_ntiles; ++i) {
            TileFlags* tf = &m_tiles[i];
            if (tf->nflags) memset(tf->flags, 0, tf->nflags);
        }
    }

    inline unsigned char getFlags(dtPolyRef ref) {
        dtAssert(m_nav);
        dtAssert(m_ntiles);
        // Assume the ref is valid, no bounds checks.
        unsigned int salt, it, ip;
        m_nav->decodePolyId(ref, salt, it, ip);
        return m_tiles[it].flags[ip];
    }

    inline void setFlags(dtPolyRef ref, unsigned char flags) {
        dtAssert(m_nav);
        dtAssert(m_ntiles);
        // Assume the ref is valid, no bounds checks.
        unsigned int salt, it, ip;
        m_nav->decodePolyId(ref, salt, it, ip);
        m_tiles[it].flags[ip] = flags;
    }
};

namespace {
void floodNavmesh(dtNavMesh* nav, NavmeshFlags* flags, dtPolyRef start, unsigned char flag) {
    // If already visited, skip.
    if (flags->getFlags(start)) return;

    flags->setFlags(start, flag);

    std::vector<dtPolyRef> openList;
    openList.push_back(start);

    while (!openList.empty()) {
        const dtPolyRef ref = openList.back();
        openList.pop_back();

        // Get current poly and tile.
        // The API input has been cheked already, skip checking internal data.
        const dtMeshTile* tile = nullptr;
        const dtPoly* poly = nullptr;
        nav->getTileAndPolyByRefUnsafe(ref, &tile, &poly);

        // Visit linked polygons.
        for (unsigned int i = poly->firstLink; i != DT_NULL_LINK; i = tile->links[i].next) {
            const dtPolyRef neiRef = tile->links[i].ref;
            // Skip invalid and already visited.
            if (!neiRef || flags->getFlags(neiRef)) continue;
            // Mark as visited
            flags->setFlags(neiRef, flag);
            // Visit neighbours
            openList.push_back(neiRef);
        }
    }
}

void disableUnvisitedPolys(dtNavMesh* nav, NavmeshFlags* flags) {
    for (int i = 0; i < nav->getMaxTiles(); ++i) {
        const dtMeshTile* tile = ((const dtNavMesh*)nav)->getTile(i);
        if (!tile->header) continue;
        const dtPolyRef base = nav->getPolyRefBase(tile);
        for (int j = 0; j < tile->header->polyCount; ++j) {
            const dtPolyRef ref = base | (unsigned int)j;
            if (!flags->getFlags(ref)) {
                unsigned short f = 0;
                nav->getPolyFlags(ref, &f);
                nav->setPolyFlags(ref, f | POLY_FLAGS_DISABLED);
            }
        }
    }
}

}  // namespace

NavMeshPruneTool::NavMeshPruneTool() : m_sample(nullptr), m_flags(nullptr), m_hitPosSet(false) {}

NavMeshPruneTool::~NavMeshPruneTool() { delete m_flags; }

void NavMeshPruneTool::init(NavigationManager* sample) { m_sample = sample; }

void NavMeshPruneTool::reset() {
    m_hitPosSet = false;
    delete m_flags;
    m_flags = nullptr;
}

void NavMeshPruneTool::handleClick(const float* s, const float* p, bool shift) {
    rcIgnoreUnused(s);
    rcIgnoreUnused(shift);

    if (!m_sample) return;
    InputGeom* geom = m_sample->getInputGeom();
    if (!geom) return;
    dtNavMesh* nav = m_sample->getNavMesh();
    if (!nav) return;
    dtNavMeshQuery* query = m_sample->getNavMeshQuery();
    if (!query) return;

    dtVcopy(m_hitPos, p);
    m_hitPosSet = true;

    if (!m_flags) {
        m_flags = new NavmeshFlags;
        m_flags->init(nav);
    }

    const float halfExtents[3] = {2, 4, 2};
    dtQueryFilter filter;
    dtPolyRef ref = 0;
    query->findNearestPoly(p, halfExtents, &filter, &ref, nullptr);

    floodNavmesh(nav, m_flags, ref, 1);
}

void NavMeshPruneTool::handleRender() {
    debug::DebugDraw& dd = m_sample->getDebugDraw();

    if (m_hitPosSet) {
        const float s = m_sample->getAgentRadius();
        const unsigned int col = debug::int2RGBA(255, 255, 255, 255);
        dd.begin(wgpu::PrimitiveTopology::LineList);
        dd.vertex(m_hitPos[0] - s, m_hitPos[1], m_hitPos[2], col);
        dd.vertex(m_hitPos[0] + s, m_hitPos[1], m_hitPos[2], col);
        dd.vertex(m_hitPos[0], m_hitPos[1] - s, m_hitPos[2], col);
        dd.vertex(m_hitPos[0], m_hitPos[1] + s, m_hitPos[2], col);
        dd.vertex(m_hitPos[0], m_hitPos[1], m_hitPos[2] - s, col);
        dd.vertex(m_hitPos[0], m_hitPos[1], m_hitPos[2] + s, col);
        dd.end();
    }

    const dtNavMesh* nav = m_sample->getNavMesh();
    if (m_flags && nav) {
        for (int i = 0; i < nav->getMaxTiles(); ++i) {
            const dtMeshTile* tile = nav->getTile(i);
            if (!tile->header) continue;
            const dtPolyRef base = nav->getPolyRefBase(tile);
            for (int j = 0; j < tile->header->polyCount; ++j) {
                const dtPolyRef ref = base | (unsigned int)j;
                if (m_flags->getFlags(ref)) {
                    debug::debugDrawNavMeshPoly(&dd, *nav, ref, debug::int2RGBA(255, 255, 255, 128));
                }
            }
        }
    }
}

}  // namespace vox::nav