//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>

#include <string>

namespace vox::nav {
class TestCase {
    enum TestType {
        TEST_PATHFIND,
        TEST_RAYCAST,
    };

    struct Test {
        Test()
            : type(),
              spos(),
              epos(),
              nspos(),
              nepos(),
              radius(0),
              includeFlags(0),
              excludeFlags(0),
              expand(false),
              straight(nullptr),
              nstraight(0),
              polys(nullptr),
              npolys(0),
              findNearestPolyTime(0),
              findPathTime(0),
              findStraightPathTime(0),
              next(nullptr) {}

        ~Test() {
            delete[] straight;
            delete[] polys;
        }

        TestType type;
        float spos[3];
        float epos[3];
        float nspos[3];
        float nepos[3];
        float radius;
        unsigned short includeFlags;
        unsigned short excludeFlags;
        bool expand;

        float* straight;
        int nstraight;
        dtPolyRef* polys;
        int npolys;

        int findNearestPolyTime;
        int findPathTime;
        int findStraightPathTime;

        Test* next;

        Test(const Test&) = delete;
        Test& operator=(const Test&) = delete;
    };

    std::string m_sampleName;
    std::string m_geomFileName;
    Test* m_tests;

    void resetTimes();

public:
    TestCase();
    ~TestCase();

    bool load(const std::string& filePath);

    [[nodiscard]] const std::string& getSampleName() const { return m_sampleName; }
    [[nodiscard]] const std::string& getGeomFileName() const { return m_geomFileName; }

    void doTests(class dtNavMesh* navmesh, class dtNavMeshQuery* navquery);

    void handleRender();
    bool handleRenderOverlay(double* proj, double* model, int* view);

private:
    // Explicitly disabled copy constructor and copy assignment operator.
    TestCase(const TestCase&);
    TestCase& operator=(const TestCase&);
};
}  // namespace vox::nav