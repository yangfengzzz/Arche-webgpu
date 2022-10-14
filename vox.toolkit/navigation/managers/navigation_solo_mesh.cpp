//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/navigation/managers/navigation_solo_mesh.h"

#include <DetourNavMeshBuilder.h>

#include "vox.toolkit/navigation/tools/convex_volume_tool.h"
#include "vox.toolkit/navigation/tools/crowd_tool.h"
#include "vox.toolkit/navigation/tools/nav_mesh_prune_tool.h"
#include "vox.toolkit/navigation/tools/nav_mesh_tester_tool.h"
#include "vox.toolkit/navigation/tools/off_mesh_connection_tool.h"

namespace vox::nav {

NavigationSoloMesh::NavigationSoloMesh(Entity* entity)
    : NavigationManager(entity),
      m_keepInterResults(true),
      m_totalBuildTimeMs(0),
      m_triareas(nullptr),
      m_solid(nullptr),
      m_chf(nullptr),
      m_cset(nullptr),
      m_pmesh(nullptr),
      m_dmesh(nullptr),
      m_drawMode(DRAWMODE_NAVMESH) {
    setTool(new NavMeshTesterTool);
}

NavigationSoloMesh::~NavigationSoloMesh() { cleanup(); }

void NavigationSoloMesh::cleanup() {
    delete[] m_triareas;
    m_triareas = nullptr;
    rcFreeHeightField(m_solid);
    m_solid = nullptr;
    rcFreeCompactHeightfield(m_chf);
    m_chf = nullptr;
    rcFreeContourSet(m_cset);
    m_cset = nullptr;
    rcFreePolyMesh(m_pmesh);
    m_pmesh = nullptr;
    rcFreePolyMeshDetail(m_dmesh);
    m_dmesh = nullptr;
    dtFreeNavMesh(m_navMesh);
    m_navMesh = nullptr;
}

void NavigationSoloMesh::handleRender() {
    if (!m_geom || !m_geom->getMesh()) return;

    m_dd->depthMask(true);

    const float texScale = 1.0f / (m_cellSize * 10.0f);

    if (m_drawMode != DRAWMODE_NAVMESH_TRANS) {
        // Draw mesh
        debug::debugDrawTriMeshSlope(m_dd.get(), m_geom->getMesh()->getVerts(), m_geom->getMesh()->getVertCount(),
                                     m_geom->getMesh()->getTris(), m_geom->getMesh()->getNormals(),
                                     m_geom->getMesh()->getTriCount(), m_agentMaxSlope, texScale);
        m_geom->drawOffMeshConnections(m_dd.get());
    }

    m_dd->depthMask(false);

    // Draw bounds
    const float* bmin = m_geom->getNavMeshBoundsMin();
    const float* bmax = m_geom->getNavMeshBoundsMax();
    debug::debugDrawBoxWire(m_dd.get(), bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2],
                            debug::int2RGBA(255, 255, 255, 128), 1.0f);
    m_dd->begin(wgpu::PrimitiveTopology::PointList, 5.0f);
    m_dd->vertex(bmin[0], bmin[1], bmin[2], debug::int2RGBA(255, 255, 255, 128));
    m_dd->end();

    if (m_navMesh && m_navQuery &&
        (m_drawMode == DRAWMODE_NAVMESH || m_drawMode == DRAWMODE_NAVMESH_TRANS ||
         m_drawMode == DRAWMODE_NAVMESH_BVTREE || m_drawMode == DRAWMODE_NAVMESH_NODES ||
         m_drawMode == DRAWMODE_NAVMESH_INVIS)) {
        if (m_drawMode != DRAWMODE_NAVMESH_INVIS)
            debug::debugDrawNavMeshWithClosedList(m_dd.get(), *m_navMesh, *m_navQuery, m_navMeshDrawFlags);
        if (m_drawMode == DRAWMODE_NAVMESH_BVTREE) debug::debugDrawNavMeshBVTree(m_dd.get(), *m_navMesh);
        if (m_drawMode == DRAWMODE_NAVMESH_NODES) debug::debugDrawNavMeshNodes(m_dd.get(), *m_navQuery);
        debug::debugDrawNavMeshPolysWithFlags(m_dd.get(), *m_navMesh, POLY_FLAGS_DISABLED,
                                              debug::int2RGBA(0, 0, 0, 128));
    }

    m_dd->depthMask(true);

    if (m_chf && m_drawMode == DRAWMODE_COMPACT) debug::debugDrawCompactHeightfieldSolid(m_dd.get(), *m_chf);

    if (m_chf && m_drawMode == DRAWMODE_COMPACT_DISTANCE)
        debug::debugDrawCompactHeightfieldDistance(m_dd.get(), *m_chf);
    if (m_chf && m_drawMode == DRAWMODE_COMPACT_REGIONS) debug::debugDrawCompactHeightfieldRegions(m_dd.get(), *m_chf);
    if (m_solid && m_drawMode == DRAWMODE_VOXELS) {
        debug::debugDrawHeightfieldSolid(m_dd.get(), *m_solid);
    }
    if (m_solid && m_drawMode == DRAWMODE_VOXELS_WALKABLE) {
        debug::debugDrawHeightfieldWalkable(m_dd.get(), *m_solid);
    }
    if (m_cset && m_drawMode == DRAWMODE_RAW_CONTOURS) {
        m_dd->depthMask(false);
        debug::debugDrawRawContours(m_dd.get(), *m_cset);
        m_dd->depthMask(true);
    }
    if (m_cset && m_drawMode == DRAWMODE_BOTH_CONTOURS) {
        m_dd->depthMask(false);
        debug::debugDrawRawContours(m_dd.get(), *m_cset, 0.5f);
        debug::debugDrawContours(m_dd.get(), *m_cset);
        m_dd->depthMask(true);
    }
    if (m_cset && m_drawMode == DRAWMODE_CONTOURS) {
        m_dd->depthMask(false);
        debug::debugDrawContours(m_dd.get(), *m_cset);
        m_dd->depthMask(true);
    }
    if (m_chf && m_cset && m_drawMode == DRAWMODE_REGION_CONNECTIONS) {
        debug::debugDrawCompactHeightfieldRegions(m_dd.get(), *m_chf);

        m_dd->depthMask(false);
        debug::debugDrawRegionConnections(m_dd.get(), *m_cset);
        m_dd->depthMask(true);
    }
    if (m_pmesh && m_drawMode == DRAWMODE_POLYMESH) {
        m_dd->depthMask(false);
        debug::debugDrawPolyMesh(m_dd.get(), *m_pmesh);
        m_dd->depthMask(true);
    }
    if (m_dmesh && m_drawMode == DRAWMODE_POLYMESH_DETAIL) {
        m_dd->depthMask(false);
        debug::debugDrawPolyMeshDetail(m_dd.get(), *m_dmesh);
        m_dd->depthMask(true);
    }

    m_geom->drawConvexVolumes(m_dd.get());

    if (m_tool) m_tool->handleRender();
    renderToolStates();

    m_dd->depthMask(true);
}

void NavigationSoloMesh::handleMeshChanged(class InputGeom* geom) {
    NavigationManager::handleMeshChanged(geom);

    dtFreeNavMesh(m_navMesh);
    m_navMesh = nullptr;

    if (m_tool) {
        m_tool->reset();
        m_tool->init(this);
    }
    resetToolStates();
    initToolStates(this);
}

bool NavigationSoloMesh::handleBuild() {
    if (!m_geom || !m_geom->getMesh()) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Input mesh is not specified.");
        return false;
    }

    cleanup();

    const float* bmin = m_geom->getNavMeshBoundsMin();
    const float* bmax = m_geom->getNavMeshBoundsMax();
    const float* verts = m_geom->getMesh()->getVerts();
    const int nverts = m_geom->getMesh()->getVertCount();
    const int* tris = m_geom->getMesh()->getTris();
    const int ntris = m_geom->getMesh()->getTriCount();

    //
    // Step 1. Initialize build config.
    //

    // Init build configuration from GUI
    memset(&m_cfg, 0, sizeof(m_cfg));
    m_cfg.cs = m_cellSize;
    m_cfg.ch = m_cellHeight;
    m_cfg.walkableSlopeAngle = m_agentMaxSlope;
    m_cfg.walkableHeight = (int)ceilf(m_agentHeight / m_cfg.ch);
    m_cfg.walkableClimb = (int)floorf(m_agentMaxClimb / m_cfg.ch);
    m_cfg.walkableRadius = (int)ceilf(m_agentRadius / m_cfg.cs);
    m_cfg.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
    m_cfg.maxSimplificationError = m_edgeMaxError;
    m_cfg.minRegionArea = (int)rcSqr(m_regionMinSize);      // Note: area = size*size
    m_cfg.mergeRegionArea = (int)rcSqr(m_regionMergeSize);  // Note: area = size*size
    m_cfg.maxVertsPerPoly = (int)m_vertsPerPoly;
    m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
    m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

    // Set the area where the navigation will be build.
    // Here the bounds of the input mesh are used, but the
    // area could be specified by an user defined box, etc.
    rcVcopy(m_cfg.bmin, bmin);
    rcVcopy(m_cfg.bmax, bmax);
    rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

    // Reset build times gathering.
    m_ctx->resetTimers();

    // Start the build process.
    m_ctx->startTimer(RC_TIMER_TOTAL);

    m_ctx->log(RC_LOG_PROGRESS, "Building navigation:");
    m_ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
    m_ctx->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts / 1000.0f, ntris / 1000.0f);

    //
    // Step 2. Rasterize input polygon soup.
    //

    // Allocate voxel heightfield where we rasterize our input data to.
    m_solid = rcAllocHeightfield();
    if (!m_solid) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
        return false;
    }
    if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch)) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
        return false;
    }

    // Allocate array that can hold triangle area types.
    // If you have multiple meshes you need to process, allocate
    // and array which can hold the max number of triangles you need to process.
    m_triareas = new unsigned char[ntris];
    if (!m_triareas) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
        return false;
    }

    // Find triangles which are walkable based on their slope and rasterize them.
    // If your input data is multiple meshes, you can transform them here, calculate
    // the are type for each of the meshes and rasterize them.
    memset(m_triareas, 0, ntris * sizeof(unsigned char));
    rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
    if (!rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb)) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
        return false;
    }

    if (!m_keepInterResults) {
        delete[] m_triareas;
        m_triareas = nullptr;
    }

    //
    // Step 3. Filter walkables surfaces.
    //

    // Once all geoemtry is rasterized, we do initial pass of filtering to
    // remove unwanted overhangs caused by the conservative rasterization
    // as well as filter spans where the character cannot possibly stand.
    if (m_filterLowHangingObstacles) rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
    if (m_filterLedgeSpans) rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
    if (m_filterWalkableLowHeightSpans) rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);

    //
    // Step 4. Partition walkable surface to simple regions.
    //

    // Compact the heightfield so that it is faster to handle from now on.
    // This will result more cache coherent data as well as the neighbours
    // between walkable cells will be calculated.
    m_chf = rcAllocCompactHeightfield();
    if (!m_chf) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
        return false;
    }
    if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf)) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
        return false;
    }

    if (!m_keepInterResults) {
        rcFreeHeightField(m_solid);
        m_solid = nullptr;
    }

    // Erode the walkable area by agent radius.
    if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf)) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
        return false;
    }

    // (Optional) Mark areas.
    const ConvexVolume* vols = m_geom->getConvexVolumes();
    for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
        rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax,
                             (unsigned char)vols[i].area, *m_chf);

    // Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
    // There are 3 martitioning methods, each with some pros and cons:
    // 1) Watershed partitioning
    //   - the classic Recast partitioning
    //   - creates the nicest tessellation
    //   - usually slowest
    //   - partitions the heightfield into nice regions without holes or overlaps
    //   - the are some corner cases where this method creates produces holes and overlaps
    //      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
    //      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
    //   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
    // 2) Monotone partioning
    //   - fastest
    //   - partitions the heightfield into regions without holes and overlaps (guaranteed)
    //   - creates long thin polygons, which sometimes causes paths with detours
    //   * use this if you want fast navmesh generation
    // 3) Layer partitoining
    //   - quite fast
    //   - partitions the heighfield into non-overlapping regions
    //   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
    //   - produces better triangles than monotone partitioning
    //   - does not have the corner cases of watershed partitioning
    //   - can be slow and create a bit ugly tessellation (still better than monotone)
    //     if you have large open areas with small obstacles (not a problem if you use tiles)
    //   * good choice to use for tiled navmesh with medium and small sized tiles

    if (m_partitionType == PARTITION_WATERSHED) {
        // Prepare for region partitioning, by calculating distance field along the walkable surface.
        if (!rcBuildDistanceField(m_ctx, *m_chf)) {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
            return false;
        }

        // Partition the walkable surface into simple regions without holes.
        if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea)) {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
            return false;
        }
    } else if (m_partitionType == PARTITION_MONOTONE) {
        // Partition the walkable surface into simple regions without holes.
        // Monotone partitioning does not need distancefield.
        if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea)) {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
            return false;
        }
    } else  // SAMPLE_PARTITION_LAYERS
    {
        // Partition the walkable surface into simple regions without holes.
        if (!rcBuildLayerRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea)) {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
            return false;
        }
    }

    //
    // Step 5. Trace and simplify region contours.
    //

    // Create contours.
    m_cset = rcAllocContourSet();
    if (!m_cset) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
        return false;
    }
    if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset)) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
        return false;
    }

    //
    // Step 6. Build polygons mesh from contours.
    //

    // Build polygon navmesh from the contours.
    m_pmesh = rcAllocPolyMesh();
    if (!m_pmesh) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
        return false;
    }
    if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh)) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
        return false;
    }

    //
    // Step 7. Create detail mesh which allows to access approximate height on each polygon.
    //

    m_dmesh = rcAllocPolyMeshDetail();
    if (!m_dmesh) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
        return false;
    }

    if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh)) {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
        return false;
    }

    if (!m_keepInterResults) {
        rcFreeCompactHeightfield(m_chf);
        m_chf = nullptr;
        rcFreeContourSet(m_cset);
        m_cset = nullptr;
    }

    // At this point the navigation mesh data is ready, you can access it from m_pmesh.
    // See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

    //
    // (Optional) Step 8. Create Detour data from Recast poly mesh.
    //

    // The GUI may allow more max points per polygon than Detour can handle.
    // Only build the detour navmesh if we do not exceed the limit.
    if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON) {
        unsigned char* navData = nullptr;
        int navDataSize = 0;

        // Update poly flags from areas.
        for (int i = 0; i < m_pmesh->npolys; ++i) {
            if (m_pmesh->areas[i] == RC_WALKABLE_AREA) m_pmesh->areas[i] = POLY_AREA_GROUND;

            if (m_pmesh->areas[i] == POLY_AREA_GROUND || m_pmesh->areas[i] == POLY_AREA_GRASS ||
                m_pmesh->areas[i] == POLY_AREA_ROAD) {
                m_pmesh->flags[i] = POLY_FLAGS_WALK;
            } else if (m_pmesh->areas[i] == POLY_AREA_WATER) {
                m_pmesh->flags[i] = POLY_FLAGS_SWIM;
            } else if (m_pmesh->areas[i] == POLY_AREA_DOOR) {
                m_pmesh->flags[i] = POLY_FLAGS_WALK | POLY_FLAGS_DOOR;
            }
        }

        dtNavMeshCreateParams params{};
        memset(&params, 0, sizeof(params));
        params.verts = m_pmesh->verts;
        params.vertCount = m_pmesh->nverts;
        params.polys = m_pmesh->polys;
        params.polyAreas = m_pmesh->areas;
        params.polyFlags = m_pmesh->flags;
        params.polyCount = m_pmesh->npolys;
        params.nvp = m_pmesh->nvp;
        params.detailMeshes = m_dmesh->meshes;
        params.detailVerts = m_dmesh->verts;
        params.detailVertsCount = m_dmesh->nverts;
        params.detailTris = m_dmesh->tris;
        params.detailTriCount = m_dmesh->ntris;
        params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
        params.offMeshConRad = m_geom->getOffMeshConnectionRads();
        params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
        params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
        params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
        params.offMeshConUserID = m_geom->getOffMeshConnectionId();
        params.offMeshConCount = m_geom->getOffMeshConnectionCount();
        params.walkableHeight = m_agentHeight;
        params.walkableRadius = m_agentRadius;
        params.walkableClimb = m_agentMaxClimb;
        rcVcopy(params.bmin, m_pmesh->bmin);
        rcVcopy(params.bmax, m_pmesh->bmax);
        params.cs = m_cfg.cs;
        params.ch = m_cfg.ch;
        params.buildBvTree = true;

        if (!dtCreateNavMeshData(&params, &navData, &navDataSize)) {
            m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
            return false;
        }

        m_navMesh = dtAllocNavMesh();
        if (!m_navMesh) {
            dtFree(navData);
            m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
            return false;
        }

        dtStatus status;

        status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
        if (dtStatusFailed(status)) {
            dtFree(navData);
            m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
            return false;
        }

        status = m_navQuery->init(m_navMesh, 2048);
        if (dtStatusFailed(status)) {
            m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
            return false;
        }
    }

    m_ctx->stopTimer(RC_TIMER_TOTAL);

    m_totalBuildTimeMs = m_ctx->getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;

    if (m_tool) m_tool->init(this);
    initToolStates(this);

    return true;
}
}  // namespace vox::nav