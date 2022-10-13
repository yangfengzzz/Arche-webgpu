//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.navigation/test_case.h"

#include <DetourCommon.h>

#include "vox.navigation/perf_timer.h"

namespace vox::nav {
namespace {
char* parseRow(char* buf, const char* bufEnd, char* row, int len) {
    bool start = true;
    bool done = false;
    int n = 0;
    while (!done && buf < bufEnd) {
        char c = *buf;
        buf++;
        // multirow
        switch (c) {
            case '\n':
                if (start) break;
                done = true;
                break;
            case '\r':
                break;
            case '\t':
            case ' ':
                if (start) break;
                // else falls through
            default:
                start = false;
                row[n++] = c;
                if (n >= len - 1) done = true;
                break;
        }
    }
    row[n] = '\0';
    return buf;
}

void copyName(std::string& dst, const char* src) {
    // Skip white spaces
    while (*src && isspace(*src)) src++;
    dst = src;
}
}  // namespace

TestCase::TestCase() : m_tests(nullptr) {}

TestCase::~TestCase() {
    Test* iter = m_tests;
    while (iter) {
        Test* next = iter->next;
        delete iter;
        iter = next;
    }
}

bool TestCase::load(const std::string& filePath) {
    char* buf = nullptr;
    FILE* fp = fopen(filePath.c_str(), "rb");
    if (!fp) return false;
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return false;
    }
    long bufSize = ftell(fp);
    if (bufSize < 0) {
        fclose(fp);
        return false;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return false;
    }
    buf = new char[bufSize];
    if (!buf) {
        fclose(fp);
        return false;
    }
    size_t readLen = fread(buf, bufSize, 1, fp);
    fclose(fp);
    if (readLen != 1) {
        delete[] buf;
        return false;
    }

    char* src = buf;
    char* srcEnd = buf + bufSize;
    char row[512];
    while (src < srcEnd) {
        // Parse one row
        row[0] = '\0';
        src = parseRow(src, srcEnd, row, sizeof(row) / sizeof(char));
        if (row[0] == 's') {
            // Sample name.
            copyName(m_sampleName, row + 1);
        } else if (row[0] == 'f') {
            // File name.
            copyName(m_geomFileName, row + 1);
        } else if (row[0] == 'p' && row[1] == 'f') {
            // Pathfind test.
            Test* test = new Test();
            test->type = TEST_PATHFIND;
            test->expand = false;
            test->next = m_tests;
            m_tests = test;
            sscanf(row + 2, "%f %f %f %f %f %f %hx %hx", &test->spos[0], &test->spos[1], &test->spos[2], &test->epos[0],
                   &test->epos[1], &test->epos[2], &test->includeFlags, &test->excludeFlags);
        } else if (row[0] == 'r' && row[1] == 'c') {
            // Pathfind test.
            Test* test = new Test();
            test->type = TEST_RAYCAST;
            test->expand = false;
            test->next = m_tests;
            m_tests = test;
            sscanf(row + 2, "%f %f %f %f %f %f %hx %hx", &test->spos[0], &test->spos[1], &test->spos[2], &test->epos[0],
                   &test->epos[1], &test->epos[2], &test->includeFlags, &test->excludeFlags);
        }
    }

    delete[] buf;

    return true;
}

void TestCase::resetTimes() {
    for (Test* iter = m_tests; iter; iter = iter->next) {
        iter->findNearestPolyTime = 0;
        iter->findPathTime = 0;
        iter->findStraightPathTime = 0;
    }
}

void TestCase::doTests(dtNavMesh* navmesh, dtNavMeshQuery* navquery) {
    if (!navmesh || !navquery) return;

    resetTimes();

    static const int MAX_POLYS = 256;
    dtPolyRef polys[MAX_POLYS];
    float straight[MAX_POLYS * 3];
    const float polyPickExt[3] = {2, 4, 2};

    for (Test* iter = m_tests; iter; iter = iter->next) {
        delete[] iter->polys;
        iter->polys = nullptr;
        iter->npolys = 0;
        delete[] iter->straight;
        iter->straight = nullptr;
        iter->nstraight = 0;

        dtQueryFilter filter;
        filter.setIncludeFlags(iter->includeFlags);
        filter.setExcludeFlags(iter->excludeFlags);

        // Find start points
        TimeVal findNearestPolyStart = getPerfTime();

        dtPolyRef startRef, endRef;
        navquery->findNearestPoly(iter->spos, polyPickExt, &filter, &startRef, iter->nspos);
        navquery->findNearestPoly(iter->epos, polyPickExt, &filter, &endRef, iter->nepos);

        TimeVal findNearestPolyEnd = getPerfTime();
        iter->findNearestPolyTime += getPerfTimeUsec(findNearestPolyEnd - findNearestPolyStart);

        if (!startRef || !endRef) continue;

        if (iter->type == TEST_PATHFIND) {
            // Find path
            TimeVal findPathStart = getPerfTime();

            navquery->findPath(startRef, endRef, iter->spos, iter->epos, &filter, polys, &iter->npolys, MAX_POLYS);

            TimeVal findPathEnd = getPerfTime();
            iter->findPathTime += getPerfTimeUsec(findPathEnd - findPathStart);

            // Find straight path
            if (iter->npolys) {
                TimeVal findStraightPathStart = getPerfTime();

                navquery->findStraightPath(iter->spos, iter->epos, polys, iter->npolys, straight, nullptr, nullptr,
                                           &iter->nstraight, MAX_POLYS);
                TimeVal findStraightPathEnd = getPerfTime();
                iter->findStraightPathTime += getPerfTimeUsec(findStraightPathEnd - findStraightPathStart);
            }

            // Copy results
            if (iter->npolys) {
                iter->polys = new dtPolyRef[iter->npolys];
                memcpy(iter->polys, polys, sizeof(dtPolyRef) * iter->npolys);
            }
            if (iter->nstraight) {
                iter->straight = new float[iter->nstraight * 3];
                memcpy(iter->straight, straight, sizeof(float) * 3 * iter->nstraight);
            }
        } else if (iter->type == TEST_RAYCAST) {
            float t = 0;
            float hitNormal[3], hitPos[3];

            iter->straight = new float[2 * 3];
            iter->nstraight = 2;

            iter->straight[0] = iter->spos[0];
            iter->straight[1] = iter->spos[1];
            iter->straight[2] = iter->spos[2];

            TimeVal findPathStart = getPerfTime();

            navquery->raycast(startRef, iter->spos, iter->epos, &filter, &t, hitNormal, polys, &iter->npolys,
                              MAX_POLYS);

            TimeVal findPathEnd = getPerfTime();
            iter->findPathTime += getPerfTimeUsec(findPathEnd - findPathStart);

            if (t > 1) {
                // No hit
                dtVcopy(hitPos, iter->epos);
            } else {
                // Hit
                dtVlerp(hitPos, iter->spos, iter->epos, t);
            }
            // Adjust height.
            if (iter->npolys > 0) {
                float h = 0;
                navquery->getPolyHeight(polys[iter->npolys - 1], hitPos, &h);
                hitPos[1] = h;
            }
            dtVcopy(&iter->straight[3], hitPos);

            if (iter->npolys) {
                iter->polys = new dtPolyRef[iter->npolys];
                memcpy(iter->polys, polys, sizeof(dtPolyRef) * iter->npolys);
            }
        }
    }

    printf("Test Results:\n");
    int n = 0;
    for (Test* iter = m_tests; iter; iter = iter->next) {
        const int total = iter->findNearestPolyTime + iter->findPathTime + iter->findStraightPathTime;
        printf(" - Path %02d:     %.4f ms\n", n, (float)total / 1000.0f);
        printf("    - poly:     %.4f ms\n", (float)iter->findNearestPolyTime / 1000.0f);
        printf("    - path:     %.4f ms\n", (float)iter->findPathTime / 1000.0f);
        printf("    - straight: %.4f ms\n", (float)iter->findStraightPathTime / 1000.0f);
        n++;
    }
}

void TestCase::handleRender() {
    // todo
}

bool TestCase::handleRenderOverlay(double* proj, double* model, int* view) {
    // todo
    return false;
}

}  // namespace vox::nav