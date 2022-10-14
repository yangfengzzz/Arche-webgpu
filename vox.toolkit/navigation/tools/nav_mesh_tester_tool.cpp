//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/navigation/tools/nav_mesh_tester_tool.h"

#include <DetourCommon.h>

// Uncomment this to dump all the requests in stdout.
#define DUMP_REQS

namespace vox::nav {
namespace {
// Returns a random number [0..1]
float frand() {
    //	return ((float)(rand() & 0xffff)/(float)0xffff);
    return (float)rand() / (float)RAND_MAX;
}

inline bool inRange(const float* v1, const float* v2, const float r, const float h) {
    const float dx = v2[0] - v1[0];
    const float dy = v2[1] - v1[1];
    const float dz = v2[2] - v1[2];
    return (dx * dx + dz * dz) < r * r && fabsf(dy) < h;
}

int fixupCorridor(dtPolyRef* path, const int npath, const int maxPath, const dtPolyRef* visited, const int nvisited) {
    int furthestPath = -1;
    int furthestVisited = -1;

    // Find the furthest common polygon.
    for (int i = npath - 1; i >= 0; --i) {
        bool found = false;
        for (int j = nvisited - 1; j >= 0; --j) {
            if (path[i] == visited[j]) {
                furthestPath = i;
                furthestVisited = j;
                found = true;
            }
        }
        if (found) break;
    }

    // If no intersection found just return current path.
    if (furthestPath == -1 || furthestVisited == -1) return npath;

    // Concatenate paths.

    // Adjust beginning of the buffer to include the visited.
    const int req = nvisited - furthestVisited;
    const int orig = rcMin(furthestPath + 1, npath);
    int size = rcMax(0, npath - orig);
    if (req + size > maxPath) size = maxPath - req;
    if (size) memmove(path + req, path + orig, size * sizeof(dtPolyRef));

    // Store visited
    for (int i = 0; i < req; ++i) path[i] = visited[(nvisited - 1) - i];

    return req + size;
}

// This function checks if the path has a small U-turn, that is,
// a polygon further in the path is adjacent to the first polygon
// in the path. If that happens, a shortcut is taken.
// This can happen if the target (T) location is at tile boundary,
// and we're (S) approaching it parallel to the tile edge.
// The choice at the vertex can be arbitrary,
//  +---+---+
//  |:::|:::|
//  +-S-+-T-+
//  |:::|   | <-- the step can end up in here, resulting U-turn path.
//  +---+---+
int fixupShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* navQuery) {
    if (npath < 3) return npath;

    // Get connected polygons
    static const int maxNeis = 16;
    dtPolyRef neis[maxNeis];
    int nneis = 0;

    const dtMeshTile* tile = nullptr;
    const dtPoly* poly = nullptr;
    if (dtStatusFailed(navQuery->getAttachedNavMesh()->getTileAndPolyByRef(path[0], &tile, &poly))) return npath;

    for (unsigned int k = poly->firstLink; k != DT_NULL_LINK; k = tile->links[k].next) {
        const dtLink* link = &tile->links[k];
        if (link->ref != 0) {
            if (nneis < maxNeis) neis[nneis++] = link->ref;
        }
    }

    // If any of the neighbour polygons is within the next few polygons
    // in the path, shortcut to that polygon directly.
    static const int maxLookAhead = 6;
    int cut = 0;
    for (int i = dtMin(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--) {
        for (int j = 0; j < nneis; j++) {
            if (path[i] == neis[j]) {
                cut = i;
                break;
            }
        }
    }
    if (cut > 1) {
        int offset = cut - 1;
        npath -= offset;
        for (int i = 1; i < npath; i++) path[i] = path[i + offset];
    }

    return npath;
}

bool getSteerTarget(dtNavMeshQuery* navQuery,
                    const float* startPos,
                    const float* endPos,
                    const float minTargetDist,
                    const dtPolyRef* path,
                    const int pathSize,
                    float* steerPos,
                    unsigned char& steerPosFlag,
                    dtPolyRef& steerPosRef,
                    float* outPoints = nullptr,
                    int* outPointCount = nullptr) {
    // Find steer target.
    static const int MAX_STEER_POINTS = 3;
    float steerPath[MAX_STEER_POINTS * 3];
    unsigned char steerPathFlags[MAX_STEER_POINTS];
    dtPolyRef steerPathPolys[MAX_STEER_POINTS];
    int nsteerPath = 0;
    navQuery->findStraightPath(startPos, endPos, path, pathSize, steerPath, steerPathFlags, steerPathPolys, &nsteerPath,
                               MAX_STEER_POINTS);
    if (!nsteerPath) return false;

    if (outPoints && outPointCount) {
        *outPointCount = nsteerPath;
        for (int i = 0; i < nsteerPath; ++i) dtVcopy(&outPoints[i * 3], &steerPath[i * 3]);
    }

    // Find vertex far enough to steer to.
    int ns = 0;
    while (ns < nsteerPath) {
        // Stop at Off-Mesh link or when point is further than slop away.
        if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
            !inRange(&steerPath[ns * 3], startPos, minTargetDist, 1000.0f))
            break;
        ns++;
    }
    // Failed to find good point to steer to.
    if (ns >= nsteerPath) return false;

    dtVcopy(steerPos, &steerPath[ns * 3]);
    steerPos[1] = startPos[1];
    steerPosFlag = steerPathFlags[ns];
    steerPosRef = steerPathPolys[ns];

    return true;
}

void getPolyCenter(dtNavMesh* navMesh, dtPolyRef ref, float* center) {
    center[0] = 0;
    center[1] = 0;
    center[2] = 0;

    const dtMeshTile* tile = nullptr;
    const dtPoly* poly = nullptr;
    dtStatus status = navMesh->getTileAndPolyByRef(ref, &tile, &poly);
    if (dtStatusFailed(status)) return;

    for (int i = 0; i < (int)poly->vertCount; ++i) {
        const float* v = &tile->verts[poly->verts[i] * 3];
        center[0] += v[0];
        center[1] += v[1];
        center[2] += v[2];
    }
    const float s = 1.0f / poly->vertCount;
    center[0] *= s;
    center[1] *= s;
    center[2] *= s;
}
}  // namespace

NavMeshTesterTool::NavMeshTesterTool()
    : m_sample(nullptr),
      m_navMesh(nullptr),
      m_navQuery(nullptr),
      m_pathFindStatus(DT_FAILURE),
      m_toolMode(TOOLMODE_PATHFIND_FOLLOW),
      m_straightPathOptions(0),
      m_startRef(0),
      m_endRef(0),
      m_npolys(0),
      m_nstraightPath(0),
      m_nsmoothPath(0),
      m_nrandPoints(0),
      m_randPointsInCircle(false),
      m_hitResult(false),
      m_distanceToWall(0),
      m_sposSet(false),
      m_eposSet(false),
      m_pathIterNum(0),
      m_pathIterPolyCount(0),
      m_steerPointCount(0) {
    m_filter.setIncludeFlags(POLY_FLAGS_ALL ^ POLY_FLAGS_DISABLED);
    m_filter.setExcludeFlags(0);

    m_polyPickExt[0] = 2;
    m_polyPickExt[1] = 4;
    m_polyPickExt[2] = 2;

    m_neighbourhoodRadius = 2.5f;
    m_randomRadius = 5.0f;
}

void NavMeshTesterTool::init(NavigationManager* sample) {
    m_sample = sample;
    m_navMesh = sample->getNavMesh();
    m_navQuery = sample->getNavMeshQuery();
    recalc();

    if (m_navQuery) {
        // Change costs.
        m_filter.setAreaCost(POLY_AREA_GROUND, 1.0f);
        m_filter.setAreaCost(POLY_AREA_WATER, 10.0f);
        m_filter.setAreaCost(POLY_AREA_ROAD, 1.0f);
        m_filter.setAreaCost(POLY_AREA_DOOR, 1.0f);
        m_filter.setAreaCost(POLY_AREA_GRASS, 2.0f);
        m_filter.setAreaCost(POLY_AREA_JUMP, 1.5f);
    }

    m_neighbourhoodRadius = sample->getAgentRadius() * 20.0f;
    m_randomRadius = sample->getAgentRadius() * 30.0f;
}

void NavMeshTesterTool::handleClick(const float* /*s*/, const float* p, bool shift) {
    if (shift) {
        m_sposSet = true;
        dtVcopy(m_spos, p);
    } else {
        m_eposSet = true;
        dtVcopy(m_epos, p);
    }
    recalc();
}

void NavMeshTesterTool::handleToggle() {
    // TODO: merge separate to a path iterator. Use same code in recalc() too.
    if (m_toolMode != TOOLMODE_PATHFIND_FOLLOW) return;

    if (!m_sposSet || !m_eposSet || !m_startRef || !m_endRef) return;

    static const float STEP_SIZE = 0.5f;
    static const float SLOP = 0.01f;

    if (m_pathIterNum == 0) {
        m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
        m_nsmoothPath = 0;

        m_pathIterPolyCount = m_npolys;
        if (m_pathIterPolyCount) memcpy(m_pathIterPolys, m_polys, sizeof(dtPolyRef) * m_pathIterPolyCount);

        if (m_pathIterPolyCount) {
            // Iterate over the path to find smooth path on the detail mesh surface.
            m_navQuery->closestPointOnPoly(m_startRef, m_spos, m_iterPos, nullptr);
            m_navQuery->closestPointOnPoly(m_pathIterPolys[m_pathIterPolyCount - 1], m_epos, m_targetPos, nullptr);

            m_nsmoothPath = 0;

            dtVcopy(&m_smoothPath[m_nsmoothPath * 3], m_iterPos);
            m_nsmoothPath++;
        }
    }

    dtVcopy(m_prevIterPos, m_iterPos);

    m_pathIterNum++;

    if (!m_pathIterPolyCount) return;

    if (m_nsmoothPath >= MAX_SMOOTH) return;

    // Move towards target a small advancement at a time until target reached or
    // when ran out of memory to store the path.

    // Find location to steer towards.
    float steerPos[3];
    unsigned char steerPosFlag;
    dtPolyRef steerPosRef;

    if (!getSteerTarget(m_navQuery, m_iterPos, m_targetPos, SLOP, m_pathIterPolys, m_pathIterPolyCount, steerPos,
                        steerPosFlag, steerPosRef, m_steerPoints, &m_steerPointCount))
        return;

    dtVcopy(m_steerPos, steerPos);

    bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) != 0;
    bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) != 0;

    // Find movement delta.
    float delta[3], len;
    dtVsub(delta, steerPos, m_iterPos);
    len = sqrtf(dtVdot(delta, delta));
    // If the steer target is end of path or off-mesh link, do not move past the location.
    if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
        len = 1;
    else
        len = STEP_SIZE / len;
    float moveTgt[3];
    dtVmad(moveTgt, m_iterPos, delta, len);

    // Move
    float result[3];
    dtPolyRef visited[16];
    int nvisited = 0;
    m_navQuery->moveAlongSurface(m_pathIterPolys[0], m_iterPos, moveTgt, &m_filter, result, visited, &nvisited, 16);
    m_pathIterPolyCount = fixupCorridor(m_pathIterPolys, m_pathIterPolyCount, MAX_POLYS, visited, nvisited);
    m_pathIterPolyCount = fixupShortcuts(m_pathIterPolys, m_pathIterPolyCount, m_navQuery);

    float h = 0;
    m_navQuery->getPolyHeight(m_pathIterPolys[0], result, &h);
    result[1] = h;
    dtVcopy(m_iterPos, result);

    // Handle end of path and off-mesh links when close enough.
    if (endOfPath && inRange(m_iterPos, steerPos, SLOP, 1.0f)) {
        // Reached end of path.
        dtVcopy(m_iterPos, m_targetPos);
        if (m_nsmoothPath < MAX_SMOOTH) {
            dtVcopy(&m_smoothPath[m_nsmoothPath * 3], m_iterPos);
            m_nsmoothPath++;
        }
        return;
    } else if (offMeshConnection && inRange(m_iterPos, steerPos, SLOP, 1.0f)) {
        // Reached off-mesh connection.
        float startPos[3], endPos[3];

        // Advance the path up to and over the off-mesh connection.
        dtPolyRef prevRef = 0, polyRef = m_pathIterPolys[0];
        int npos = 0;
        while (npos < m_pathIterPolyCount && polyRef != steerPosRef) {
            prevRef = polyRef;
            polyRef = m_pathIterPolys[npos];
            npos++;
        }
        for (int i = npos; i < m_pathIterPolyCount; ++i) m_pathIterPolys[i - npos] = m_pathIterPolys[i];
        m_pathIterPolyCount -= npos;

        // Handle the connection.
        dtStatus status = m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
        if (dtStatusSucceed(status)) {
            if (m_nsmoothPath < MAX_SMOOTH) {
                dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
                m_nsmoothPath++;
                // Hack to make the dotted path not visible during off-mesh connection.
                if (m_nsmoothPath & 1) {
                    dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
                    m_nsmoothPath++;
                }
            }
            // Move position at the other side of the off-mesh link.
            dtVcopy(m_iterPos, endPos);
            float eh = 0.0f;
            m_navQuery->getPolyHeight(m_pathIterPolys[0], m_iterPos, &eh);
            m_iterPos[1] = eh;
        }
    }

    // Store results.
    if (m_nsmoothPath < MAX_SMOOTH) {
        dtVcopy(&m_smoothPath[m_nsmoothPath * 3], m_iterPos);
        m_nsmoothPath++;
    }
}

void NavMeshTesterTool::handleUpdate(const float /*dt*/) {
    if (m_toolMode == TOOLMODE_PATHFIND_SLICED) {
        if (dtStatusInProgress(m_pathFindStatus)) {
            m_pathFindStatus = m_navQuery->updateSlicedFindPath(1, nullptr);
        }
        if (dtStatusSucceed(m_pathFindStatus)) {
            m_navQuery->finalizeSlicedFindPath(m_polys, &m_npolys, MAX_POLYS);
            m_nstraightPath = 0;
            if (m_npolys) {
                // In case of partial path, make sure the end point is clamped to the last polygon.
                float epos[3];
                dtVcopy(epos, m_epos);
                if (m_polys[m_npolys - 1] != m_endRef)
                    m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1], m_epos, epos, nullptr);

                m_navQuery->findStraightPath(m_spos, epos, m_polys, m_npolys, m_straightPath, m_straightPathFlags,
                                             m_straightPathPolys, &m_nstraightPath, MAX_POLYS,
                                             DT_STRAIGHTPATH_ALL_CROSSINGS);
            }

            m_pathFindStatus = DT_FAILURE;
        }
    }
}

void NavMeshTesterTool::reset() {
    m_startRef = 0;
    m_endRef = 0;
    m_npolys = 0;
    m_nstraightPath = 0;
    m_nsmoothPath = 0;
    memset(m_hitPos, 0, sizeof(m_hitPos));
    memset(m_hitNormal, 0, sizeof(m_hitNormal));
    m_distanceToWall = 0;
}

void NavMeshTesterTool::recalc() {
    if (!m_navMesh) return;

    if (m_sposSet)
        m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, nullptr);
    else
        m_startRef = 0;

    if (m_eposSet)
        m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, nullptr);
    else
        m_endRef = 0;

    m_pathFindStatus = DT_FAILURE;

    if (m_toolMode == TOOLMODE_PATHFIND_FOLLOW) {
        m_pathIterNum = 0;
        if (m_sposSet && m_eposSet && m_startRef && m_endRef) {
#ifdef DUMP_REQS
            printf("pi  %f %f %f  %f %f %f  0x%x 0x%x\n", m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1],
                   m_epos[2], m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif

            m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);

            m_nsmoothPath = 0;

            if (m_npolys) {
                // Iterate over the path to find smooth path on the detail mesh surface.
                dtPolyRef polys[MAX_POLYS];
                memcpy(polys, m_polys, sizeof(dtPolyRef) * m_npolys);
                int npolys = m_npolys;

                float iterPos[3], targetPos[3];
                m_navQuery->closestPointOnPoly(m_startRef, m_spos, iterPos, nullptr);
                m_navQuery->closestPointOnPoly(polys[npolys - 1], m_epos, targetPos, nullptr);

                static const float STEP_SIZE = 0.5f;
                static const float SLOP = 0.01f;

                m_nsmoothPath = 0;

                dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
                m_nsmoothPath++;

                // Move towards target a small advancement at a time until target reached or
                // when ran out of memory to store the path.
                while (npolys && m_nsmoothPath < MAX_SMOOTH) {
                    // Find location to steer towards.
                    float steerPos[3];
                    unsigned char steerPosFlag;
                    dtPolyRef steerPosRef;

                    if (!getSteerTarget(m_navQuery, iterPos, targetPos, SLOP, polys, npolys, steerPos, steerPosFlag,
                                        steerPosRef))
                        break;

                    bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) != 0;
                    bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) != 0;

                    // Find movement delta.
                    float delta[3], len;
                    dtVsub(delta, steerPos, iterPos);
                    len = dtMathSqrtf(dtVdot(delta, delta));
                    // If the steer target is end of path or off-mesh link, do not move past the location.
                    if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
                        len = 1;
                    else
                        len = STEP_SIZE / len;
                    float moveTgt[3];
                    dtVmad(moveTgt, iterPos, delta, len);

                    // Move
                    float result[3];
                    dtPolyRef visited[16];
                    int nvisited = 0;
                    m_navQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &m_filter, result, visited, &nvisited, 16);

                    npolys = fixupCorridor(polys, npolys, MAX_POLYS, visited, nvisited);
                    npolys = fixupShortcuts(polys, npolys, m_navQuery);

                    float h = 0;
                    m_navQuery->getPolyHeight(polys[0], result, &h);
                    result[1] = h;
                    dtVcopy(iterPos, result);

                    // Handle end of path and off-mesh links when close enough.
                    if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f)) {
                        // Reached end of path.
                        dtVcopy(iterPos, targetPos);
                        if (m_nsmoothPath < MAX_SMOOTH) {
                            dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
                            m_nsmoothPath++;
                        }
                        break;
                    } else if (offMeshConnection && inRange(iterPos, steerPos, SLOP, 1.0f)) {
                        // Reached off-mesh connection.
                        float startPos[3], endPos[3];

                        // Advance the path up to and over the off-mesh connection.
                        dtPolyRef prevRef = 0, polyRef = polys[0];
                        int npos = 0;
                        while (npos < npolys && polyRef != steerPosRef) {
                            prevRef = polyRef;
                            polyRef = polys[npos];
                            npos++;
                        }
                        for (int i = npos; i < npolys; ++i) polys[i - npos] = polys[i];
                        npolys -= npos;

                        // Handle the connection.
                        dtStatus status =
                                m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
                        if (dtStatusSucceed(status)) {
                            if (m_nsmoothPath < MAX_SMOOTH) {
                                dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
                                m_nsmoothPath++;
                                // Hack to make the dotted path not visible during off-mesh connection.
                                if (m_nsmoothPath & 1) {
                                    dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
                                    m_nsmoothPath++;
                                }
                            }
                            // Move position at the other side of the off-mesh link.
                            dtVcopy(iterPos, endPos);
                            float eh = 0.0f;
                            m_navQuery->getPolyHeight(polys[0], iterPos, &eh);
                            iterPos[1] = eh;
                        }
                    }

                    // Store results.
                    if (m_nsmoothPath < MAX_SMOOTH) {
                        dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
                        m_nsmoothPath++;
                    }
                }
            }

        } else {
            m_npolys = 0;
            m_nsmoothPath = 0;
        }
    } else if (m_toolMode == TOOLMODE_PATHFIND_STRAIGHT) {
        if (m_sposSet && m_eposSet && m_startRef && m_endRef) {
#ifdef DUMP_REQS
            printf("ps  %f %f %f  %f %f %f  0x%x 0x%x\n", m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1],
                   m_epos[2], m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
            m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
            m_nstraightPath = 0;
            if (m_npolys) {
                // In case of partial path, make sure the end point is clamped to the last polygon.
                float epos[3];
                dtVcopy(epos, m_epos);
                if (m_polys[m_npolys - 1] != m_endRef)
                    m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1], m_epos, epos, nullptr);

                m_navQuery->findStraightPath(m_spos, epos, m_polys, m_npolys, m_straightPath, m_straightPathFlags,
                                             m_straightPathPolys, &m_nstraightPath, MAX_POLYS, m_straightPathOptions);
            }
        } else {
            m_npolys = 0;
            m_nstraightPath = 0;
        }
    } else if (m_toolMode == TOOLMODE_PATHFIND_SLICED) {
        if (m_sposSet && m_eposSet && m_startRef && m_endRef) {
#ifdef DUMP_REQS
            printf("ps  %f %f %f  %f %f %f  0x%x 0x%x\n", m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1],
                   m_epos[2], m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
            m_npolys = 0;
            m_nstraightPath = 0;

            m_pathFindStatus = m_navQuery->initSlicedFindPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter,
                                                              DT_FINDPATH_ANY_ANGLE);
        } else {
            m_npolys = 0;
            m_nstraightPath = 0;
        }
    } else if (m_toolMode == TOOLMODE_RAYCAST) {
        m_nstraightPath = 0;
        if (m_sposSet && m_eposSet && m_startRef) {
#ifdef DUMP_REQS
            printf("rc  %f %f %f  %f %f %f  0x%x 0x%x\n", m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1],
                   m_epos[2], m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
            float t = 0;
            m_npolys = 0;
            m_nstraightPath = 2;
            m_straightPath[0] = m_spos[0];
            m_straightPath[1] = m_spos[1];
            m_straightPath[2] = m_spos[2];
            m_navQuery->raycast(m_startRef, m_spos, m_epos, &m_filter, &t, m_hitNormal, m_polys, &m_npolys, MAX_POLYS);
            if (t > 1) {
                // No hit
                dtVcopy(m_hitPos, m_epos);
                m_hitResult = false;
            } else {
                // Hit
                dtVlerp(m_hitPos, m_spos, m_epos, t);
                m_hitResult = true;
            }
            // Adjust height.
            if (m_npolys > 0) {
                float h = 0;
                m_navQuery->getPolyHeight(m_polys[m_npolys - 1], m_hitPos, &h);
                m_hitPos[1] = h;
            }
            dtVcopy(&m_straightPath[3], m_hitPos);
        }
    } else if (m_toolMode == TOOLMODE_DISTANCE_TO_WALL) {
        m_distanceToWall = 0;
        if (m_sposSet && m_startRef) {
#ifdef DUMP_REQS
            printf("dw  %f %f %f  %f  0x%x 0x%x\n", m_spos[0], m_spos[1], m_spos[2], 100.0f, m_filter.getIncludeFlags(),
                   m_filter.getExcludeFlags());
#endif
            m_distanceToWall = 0.0f;
            m_navQuery->findDistanceToWall(m_startRef, m_spos, 100.0f, &m_filter, &m_distanceToWall, m_hitPos,
                                           m_hitNormal);
        }
    } else if (m_toolMode == TOOLMODE_FIND_POLYS_IN_CIRCLE) {
        if (m_sposSet && m_startRef && m_eposSet) {
            const float dx = m_epos[0] - m_spos[0];
            const float dz = m_epos[2] - m_spos[2];
            float dist = sqrtf(dx * dx + dz * dz);
#ifdef DUMP_REQS
            printf("fpc  %f %f %f  %f  0x%x 0x%x\n", m_spos[0], m_spos[1], m_spos[2], dist, m_filter.getIncludeFlags(),
                   m_filter.getExcludeFlags());
#endif
            m_navQuery->findPolysAroundCircle(m_startRef, m_spos, dist, &m_filter, m_polys, m_parent, nullptr,
                                              &m_npolys, MAX_POLYS);
        }
    } else if (m_toolMode == TOOLMODE_FIND_POLYS_IN_SHAPE) {
        if (m_sposSet && m_startRef && m_eposSet) {
            const float nx = (m_epos[2] - m_spos[2]) * 0.25f;
            const float nz = -(m_epos[0] - m_spos[0]) * 0.25f;
            const float agentHeight = m_sample ? m_sample->getAgentHeight() : 0;

            m_queryPoly[0] = m_spos[0] + nx * 1.2f;
            m_queryPoly[1] = m_spos[1] + agentHeight / 2;
            m_queryPoly[2] = m_spos[2] + nz * 1.2f;

            m_queryPoly[3] = m_spos[0] - nx * 1.3f;
            m_queryPoly[4] = m_spos[1] + agentHeight / 2;
            m_queryPoly[5] = m_spos[2] - nz * 1.3f;

            m_queryPoly[6] = m_epos[0] - nx * 0.8f;
            m_queryPoly[7] = m_epos[1] + agentHeight / 2;
            m_queryPoly[8] = m_epos[2] - nz * 0.8f;

            m_queryPoly[9] = m_epos[0] + nx;
            m_queryPoly[10] = m_epos[1] + agentHeight / 2;
            m_queryPoly[11] = m_epos[2] + nz;

#ifdef DUMP_REQS
            printf("fpp  %f %f %f  %f %f %f  %f %f %f  %f %f %f  0x%x 0x%x\n", m_queryPoly[0], m_queryPoly[1],
                   m_queryPoly[2], m_queryPoly[3], m_queryPoly[4], m_queryPoly[5], m_queryPoly[6], m_queryPoly[7],
                   m_queryPoly[8], m_queryPoly[9], m_queryPoly[10], m_queryPoly[11], m_filter.getIncludeFlags(),
                   m_filter.getExcludeFlags());
#endif
            m_navQuery->findPolysAroundShape(m_startRef, m_queryPoly, 4, &m_filter, m_polys, m_parent, nullptr,
                                             &m_npolys, MAX_POLYS);
        }
    } else if (m_toolMode == TOOLMODE_FIND_LOCAL_NEIGHBOURHOOD) {
        if (m_sposSet && m_startRef) {
#ifdef DUMP_REQS
            printf("fln  %f %f %f  %f  0x%x 0x%x\n", m_spos[0], m_spos[1], m_spos[2], m_neighbourhoodRadius,
                   m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
            m_navQuery->findLocalNeighbourhood(m_startRef, m_spos, m_neighbourhoodRadius, &m_filter, m_polys, m_parent,
                                               &m_npolys, MAX_POLYS);
        }
    }
}

void NavMeshTesterTool::handleRender() {
    debug::DebugDraw& dd = m_sample->getDebugDraw();

    static const unsigned int startCol = debug::int2RGBA(128, 25, 0, 192);
    static const unsigned int endCol = debug::int2RGBA(51, 102, 0, 129);
    static const unsigned int pathCol = debug::int2RGBA(0, 0, 0, 64);

    const float agentRadius = m_sample->getAgentRadius();
    const float agentHeight = m_sample->getAgentHeight();
    const float agentClimb = m_sample->getAgentClimb();

    dd.depthMask(false);
    if (m_sposSet) drawAgent(m_spos, agentRadius, agentHeight, agentClimb, startCol);
    if (m_eposSet) drawAgent(m_epos, agentRadius, agentHeight, agentClimb, endCol);
    dd.depthMask(true);

    if (!m_navMesh) {
        return;
    }

    if (m_toolMode == TOOLMODE_PATHFIND_FOLLOW) {
        debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_startRef, startCol);
        debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_endRef, endCol);

        if (m_npolys) {
            for (int i = 0; i < m_npolys; ++i) {
                if (m_polys[i] == m_startRef || m_polys[i] == m_endRef) continue;
                debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
            }
        }

        if (m_nsmoothPath) {
            dd.depthMask(false);
            const unsigned int spathCol = debug::int2RGBA(0, 0, 0, 220);
            dd.begin(wgpu::PrimitiveTopology::LineList, 3.0f);
            for (int i = 0; i < m_nsmoothPath; ++i)
                dd.vertex(m_smoothPath[i * 3], m_smoothPath[i * 3 + 1] + 0.1f, m_smoothPath[i * 3 + 2], spathCol);
            dd.end();
            dd.depthMask(true);
        }

        if (m_pathIterNum) {
            debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_pathIterPolys[0], debug::int2RGBA(255, 255, 255, 128));

            dd.depthMask(false);
            dd.begin(wgpu::PrimitiveTopology::LineList, 1.0f);

            const unsigned int prevCol = debug::int2RGBA(255, 192, 0, 220);
            const unsigned int curCol = debug::int2RGBA(255, 255, 255, 220);
            const unsigned int steerCol = debug::int2RGBA(0, 192, 255, 220);

            dd.vertex(m_prevIterPos[0], m_prevIterPos[1] - 0.3f, m_prevIterPos[2], prevCol);
            dd.vertex(m_prevIterPos[0], m_prevIterPos[1] + 0.3f, m_prevIterPos[2], prevCol);

            dd.vertex(m_iterPos[0], m_iterPos[1] - 0.3f, m_iterPos[2], curCol);
            dd.vertex(m_iterPos[0], m_iterPos[1] + 0.3f, m_iterPos[2], curCol);

            dd.vertex(m_prevIterPos[0], m_prevIterPos[1] + 0.3f, m_prevIterPos[2], prevCol);
            dd.vertex(m_iterPos[0], m_iterPos[1] + 0.3f, m_iterPos[2], prevCol);

            dd.vertex(m_prevIterPos[0], m_prevIterPos[1] + 0.3f, m_prevIterPos[2], steerCol);
            dd.vertex(m_steerPos[0], m_steerPos[1] + 0.3f, m_steerPos[2], steerCol);

            for (int i = 0; i < m_steerPointCount - 1; ++i) {
                dd.vertex(m_steerPoints[i * 3 + 0], m_steerPoints[i * 3 + 1] + 0.2f, m_steerPoints[i * 3 + 2],
                          debug::darkenCol(steerCol));
                dd.vertex(m_steerPoints[(i + 1) * 3 + 0], m_steerPoints[(i + 1) * 3 + 1] + 0.2f,
                          m_steerPoints[(i + 1) * 3 + 2], debug::darkenCol(steerCol));
            }

            dd.end();
            dd.depthMask(true);
        }
    } else if (m_toolMode == TOOLMODE_PATHFIND_STRAIGHT || m_toolMode == TOOLMODE_PATHFIND_SLICED) {
        debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_startRef, startCol);
        debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_endRef, endCol);

        if (m_npolys) {
            for (int i = 0; i < m_npolys; ++i) {
                if (m_polys[i] == m_startRef || m_polys[i] == m_endRef) continue;
                debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
            }
        }

        if (m_nstraightPath) {
            dd.depthMask(false);
            const unsigned int spathCol = debug::int2RGBA(64, 16, 0, 220);
            const unsigned int offMeshCol = debug::int2RGBA(128, 96, 0, 220);
            dd.begin(wgpu::PrimitiveTopology::LineList, 2.0f);
            for (int i = 0; i < m_nstraightPath - 1; ++i) {
                unsigned int col;
                if (m_straightPathFlags[i] & DT_STRAIGHTPATH_OFFMESH_CONNECTION)
                    col = offMeshCol;
                else
                    col = spathCol;

                dd.vertex(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.4f, m_straightPath[i * 3 + 2], col);
                dd.vertex(m_straightPath[(i + 1) * 3], m_straightPath[(i + 1) * 3 + 1] + 0.4f,
                          m_straightPath[(i + 1) * 3 + 2], col);
            }
            dd.end();
            dd.begin(wgpu::PrimitiveTopology::PointList, 6.0f);
            for (int i = 0; i < m_nstraightPath; ++i) {
                unsigned int col;
                if (m_straightPathFlags[i] & DT_STRAIGHTPATH_START)
                    col = startCol;
                else if (m_straightPathFlags[i] & DT_STRAIGHTPATH_END)
                    col = endCol;
                else if (m_straightPathFlags[i] & DT_STRAIGHTPATH_OFFMESH_CONNECTION)
                    col = offMeshCol;
                else
                    col = spathCol;
                dd.vertex(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.4f, m_straightPath[i * 3 + 2], col);
            }
            dd.end();
            dd.depthMask(true);
        }
    } else if (m_toolMode == TOOLMODE_RAYCAST) {
        debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_startRef, startCol);

        if (m_nstraightPath) {
            for (int i = 1; i < m_npolys; ++i) debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);

            dd.depthMask(false);
            const unsigned int spathCol =
                    m_hitResult ? debug::int2RGBA(64, 16, 0, 220) : debug::int2RGBA(240, 240, 240, 220);
            dd.begin(wgpu::PrimitiveTopology::LineList, 2.0f);
            for (int i = 0; i < m_nstraightPath - 1; ++i) {
                dd.vertex(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.4f, m_straightPath[i * 3 + 2], spathCol);
                dd.vertex(m_straightPath[(i + 1) * 3], m_straightPath[(i + 1) * 3 + 1] + 0.4f,
                          m_straightPath[(i + 1) * 3 + 2], spathCol);
            }
            dd.end();
            dd.begin(wgpu::PrimitiveTopology::PointList, 4.0f);
            for (int i = 0; i < m_nstraightPath; ++i)
                dd.vertex(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.4f, m_straightPath[i * 3 + 2], spathCol);
            dd.end();

            if (m_hitResult) {
                const unsigned int hitCol = debug::int2RGBA(0, 0, 0, 128);
                dd.begin(wgpu::PrimitiveTopology::LineList, 2.0f);
                dd.vertex(m_hitPos[0], m_hitPos[1] + 0.4f, m_hitPos[2], hitCol);
                dd.vertex(m_hitPos[0] + m_hitNormal[0] * agentRadius, m_hitPos[1] + 0.4f + m_hitNormal[1] * agentRadius,
                          m_hitPos[2] + m_hitNormal[2] * agentRadius, hitCol);
                dd.end();
            }
            dd.depthMask(true);
        }
    } else if (m_toolMode == TOOLMODE_DISTANCE_TO_WALL) {
        debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_startRef, startCol);
        dd.depthMask(false);
        debug::debugDrawCircle(&dd, m_spos[0], m_spos[1] + agentHeight / 2, m_spos[2], m_distanceToWall,
                               debug::int2RGBA(64, 16, 0, 220), 2.0f);
        dd.begin(wgpu::PrimitiveTopology::LineList, 3.0f);
        dd.vertex(m_hitPos[0], m_hitPos[1] + 0.02f, m_hitPos[2], debug::int2RGBA(0, 0, 0, 192));
        dd.vertex(m_hitPos[0], m_hitPos[1] + agentHeight, m_hitPos[2], debug::int2RGBA(0, 0, 0, 192));
        dd.end();
        dd.depthMask(true);
    } else if (m_toolMode == TOOLMODE_FIND_POLYS_IN_CIRCLE) {
        for (int i = 0; i < m_npolys; ++i) {
            debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
            dd.depthMask(false);
            if (m_parent[i]) {
                float p0[3], p1[3];
                dd.depthMask(false);
                getPolyCenter(m_navMesh, m_parent[i], p0);
                getPolyCenter(m_navMesh, m_polys[i], p1);
                debug::debugDrawArc(&dd, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0.25f, 0.0f, 0.4f,
                                    debug::int2RGBA(0, 0, 0, 128), 2.0f);
                dd.depthMask(true);
            }
            dd.depthMask(true);
        }

        if (m_sposSet && m_eposSet) {
            dd.depthMask(false);
            const float dx = m_epos[0] - m_spos[0];
            const float dz = m_epos[2] - m_spos[2];
            const float dist = sqrtf(dx * dx + dz * dz);
            debug::debugDrawCircle(&dd, m_spos[0], m_spos[1] + agentHeight / 2, m_spos[2], dist,
                                   debug::int2RGBA(64, 16, 0, 220), 2.0f);
            dd.depthMask(true);
        }
    } else if (m_toolMode == TOOLMODE_FIND_POLYS_IN_SHAPE) {
        for (int i = 0; i < m_npolys; ++i) {
            debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
            dd.depthMask(false);
            if (m_parent[i]) {
                float p0[3], p1[3];
                dd.depthMask(false);
                getPolyCenter(m_navMesh, m_parent[i], p0);
                getPolyCenter(m_navMesh, m_polys[i], p1);
                debug::debugDrawArc(&dd, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0.25f, 0.0f, 0.4f,
                                    debug::int2RGBA(0, 0, 0, 128), 2.0f);
                dd.depthMask(true);
            }
            dd.depthMask(true);
        }

        if (m_sposSet && m_eposSet) {
            dd.depthMask(false);
            const unsigned int col = debug::int2RGBA(64, 16, 0, 220);
            dd.begin(wgpu::PrimitiveTopology::LineList, 2.0f);
            for (int i = 0, j = 3; i < 4; j = i++) {
                const float* p0 = &m_queryPoly[j * 3];
                const float* p1 = &m_queryPoly[i * 3];
                dd.vertex(p0, col);
                dd.vertex(p1, col);
            }
            dd.end();
            dd.depthMask(true);
        }
    } else if (m_toolMode == TOOLMODE_FIND_LOCAL_NEIGHBOURHOOD) {
        for (int i = 0; i < m_npolys; ++i) {
            debug::debugDrawNavMeshPoly(&dd, *m_navMesh, m_polys[i], pathCol);
            dd.depthMask(false);
            if (m_parent[i]) {
                float p0[3], p1[3];
                dd.depthMask(false);
                getPolyCenter(m_navMesh, m_parent[i], p0);
                getPolyCenter(m_navMesh, m_polys[i], p1);
                debug::debugDrawArc(&dd, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0.25f, 0.0f, 0.4f,
                                    debug::int2RGBA(0, 0, 0, 128), 2.0f);
                dd.depthMask(true);
            }

            static const int MAX_SEGS = DT_VERTS_PER_POLYGON * 4;
            float segs[MAX_SEGS * 6];
            dtPolyRef refs[MAX_SEGS];
            memset(refs, 0, sizeof(dtPolyRef) * MAX_SEGS);
            int nsegs = 0;
            m_navQuery->getPolyWallSegments(m_polys[i], &m_filter, segs, refs, &nsegs, MAX_SEGS);
            dd.begin(wgpu::PrimitiveTopology::LineList, 2.0f);
            for (int j = 0; j < nsegs; ++j) {
                const float* s = &segs[j * 6];

                // Skip too distant segments.
                float tseg;
                float distSqr = dtDistancePtSegSqr2D(m_spos, s, s + 3, tseg);
                if (distSqr > dtSqr(m_neighbourhoodRadius)) continue;

                float delta[3], norm[3], p0[3], p1[3];
                dtVsub(delta, s + 3, s);
                dtVmad(p0, s, delta, 0.5f);
                norm[0] = delta[2];
                norm[1] = 0;
                norm[2] = -delta[0];
                dtVnormalize(norm);
                dtVmad(p1, p0, norm, agentRadius * 0.5f);

                // Skip backfacing segments.
                if (refs[j]) {
                    unsigned int col = debug::int2RGBA(255, 255, 255, 32);
                    dd.vertex(s[0], s[1] + agentClimb, s[2], col);
                    dd.vertex(s[3], s[4] + agentClimb, s[5], col);
                } else {
                    unsigned int col = debug::int2RGBA(192, 32, 16, 192);
                    if (dtTriArea2D(m_spos, s, s + 3) < 0.0f) col = debug::int2RGBA(96, 32, 16, 192);

                    dd.vertex(p0[0], p0[1] + agentClimb, p0[2], col);
                    dd.vertex(p1[0], p1[1] + agentClimb, p1[2], col);

                    dd.vertex(s[0], s[1] + agentClimb, s[2], col);
                    dd.vertex(s[3], s[4] + agentClimb, s[5], col);
                }
            }
            dd.end();

            dd.depthMask(true);
        }

        if (m_sposSet) {
            dd.depthMask(false);
            debug::debugDrawCircle(&dd, m_spos[0], m_spos[1] + agentHeight / 2, m_spos[2], m_neighbourhoodRadius,
                                   debug::int2RGBA(64, 16, 0, 220), 2.0f);
            dd.depthMask(true);
        }
    }

    if (m_nrandPoints > 0) {
        dd.begin(wgpu::PrimitiveTopology::PointList, 6.0f);
        for (int i = 0; i < m_nrandPoints; i++) {
            const float* p = &m_randPoints[i * 3];
            dd.vertex(p[0], p[1] + 0.1f, p[2], debug::int2RGBA(220, 32, 16, 192));
        }
        dd.end();

        if (m_randPointsInCircle && m_sposSet) {
            debug::debugDrawCircle(&dd, m_spos[0], m_spos[1] + agentHeight / 2, m_spos[2], m_randomRadius,
                                   debug::int2RGBA(64, 16, 0, 220), 2.0f);
        }
    }
}

void NavMeshTesterTool::drawAgent(const float* pos, float r, float h, float c, const unsigned int col) {
    debug::DebugDraw& dd = m_sample->getDebugDraw();

    dd.depthMask(false);

    // Agent dimensions.
    debug::debugDrawCylinderWire(&dd, pos[0] - r, pos[1] + 0.02f, pos[2] - r, pos[0] + r, pos[1] + h, pos[2] + r, col,
                                 2.0f);

    debug::debugDrawCircle(&dd, pos[0], pos[1] + c, pos[2], r, debug::int2RGBA(0, 0, 0, 64), 1.0f);

    unsigned int colb = debug::int2RGBA(0, 0, 0, 196);
    dd.begin(wgpu::PrimitiveTopology::LineList);
    dd.vertex(pos[0], pos[1] - c, pos[2], colb);
    dd.vertex(pos[0], pos[1] + c, pos[2], colb);
    dd.vertex(pos[0] - r / 2, pos[1] + 0.02f, pos[2], colb);
    dd.vertex(pos[0] + r / 2, pos[1] + 0.02f, pos[2], colb);
    dd.vertex(pos[0], pos[1] + 0.02f, pos[2] - r / 2, colb);
    dd.vertex(pos[0], pos[1] + 0.02f, pos[2] + r / 2, colb);
    dd.end();

    dd.depthMask(true);
}

}  // namespace vox::nav