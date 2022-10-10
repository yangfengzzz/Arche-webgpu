//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/physics_utils.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include "vox.math/perlin.h"

namespace vox {
using namespace JPH;

JPH::Body& PhysicsUtils::createFloor(JPH::BodyInterface& mBodyInterface, float inSize) {
    JPH::Body& floor = *mBodyInterface.CreateBody(JPH::BodyCreationSettings(
            new JPH::BoxShape(worldScale * JPH::Vec3(0.5f * inSize, 1.0f, 0.5f * inSize), 0.0f),
            worldScale * JPH::Vec3(0.0f, -1.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Static,
            PhysicsManager::Layers::NON_MOVING));
    mBodyInterface.AddBody(floor.GetID(), JPH::EActivation::DontActivate);
    return floor;
}

JPH::Body& PhysicsUtils::createLargeTriangleFloor(JPH::BodyInterface& mBodyInterface) {
    TriangleList triangles = {
            Triangle(Float3(427.941376f, 0.000027f, -456.470642f), Float3(427.941376f, 0.000024f, -399.411774f),
                     Float3(512.0f, 0.000031f, -511.999969f)),
            Triangle(Float3(0.0f, 0.000031f, -511.999969f), Float3(28.529310f, 0.000027f, -456.470642f),
                     Float3(427.941376f, 0.000027f, -456.470642f)),
            Triangle(Float3(427.941376f, 0.000027f, -456.470642f), Float3(512.0f, 0.000031f, -511.999969f),
                     Float3(0.0f, 0.000031f, -511.999969f)),
            Triangle(Float3(285.294067f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(313.823395f, 0.000027f, -456.470642f)),
            Triangle(Float3(313.823395f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(342.352936f, 0.000027f, -456.470642f)),
            Triangle(Float3(342.352936f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(370.882507f, 0.000027f, -456.470642f)),
            Triangle(Float3(399.411804f, 0.000024f, -399.411774f), Float3(427.941376f, 0.000024f, -399.411774f),
                     Float3(370.882507f, 0.000027f, -456.470642f)),
            Triangle(Float3(370.882507f, 0.000027f, -456.470642f), Float3(427.941376f, 0.000024f, -399.411774f),
                     Float3(399.411804f, 0.000027f, -456.470642f)),
            Triangle(Float3(399.411804f, 0.000027f, -456.470642f), Float3(427.941376f, 0.000024f, -399.411774f),
                     Float3(427.941376f, 0.000027f, -456.470642f)),
            Triangle(Float3(256.764771f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(285.294067f, 0.000027f, -456.470642f)),
            Triangle(Float3(85.588173f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(114.117729f, 0.000027f, -456.470642f)),
            Triangle(Float3(114.117729f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(142.647034f, 0.000027f, -456.470642f)),
            Triangle(Float3(142.647034f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(171.176590f, 0.000027f, -456.470642f)),
            Triangle(Float3(171.176590f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(199.705902f, 0.000027f, -456.470642f)),
            Triangle(Float3(199.705902f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(228.235214f, 0.000027f, -456.470642f)),
            Triangle(Float3(228.235214f, 0.000027f, -456.470642f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(256.764771f, 0.000027f, -456.470642f)),
            Triangle(Float3(85.588173f, 0.000024f, -399.411774f), Float3(399.411804f, 0.000024f, -399.411774f),
                     Float3(85.588173f, 0.000027f, -456.470642f)),
            Triangle(Float3(427.941376f, 0.000024f, -399.411774f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(512.0f, 0.000031f, -511.999969f)),
            Triangle(Float3(399.411804f, 0.000024f, -399.411774f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(427.941376f, 0.000024f, -399.411774f)),
            Triangle(Float3(285.294067f, 0.000024f, -399.411774f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(313.823395f, 0.000024f, -399.411774f)),
            Triangle(Float3(313.823395f, 0.000024f, -399.411774f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(342.352936f, 0.000024f, -399.411774f)),
            Triangle(Float3(342.352936f, 0.000024f, -399.411774f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(370.882507f, 0.000024f, -399.411774f)),
            Triangle(Float3(370.882507f, 0.000024f, -399.411774f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(399.411804f, 0.000024f, -399.411774f)),
            Triangle(Float3(256.764771f, 0.000024f, -399.411774f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(285.294067f, 0.000024f, -399.411774f)),
            Triangle(Float3(228.235214f, 0.000024f, -399.411774f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(256.764771f, 0.000024f, -399.411774f)),
            Triangle(Float3(199.705902f, 0.000024f, -399.411774f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(228.235214f, 0.000024f, -399.411774f)),
            Triangle(Float3(228.235214f, 0.000019f, -313.823364f), Float3(512.0f, 0.000019f, -313.823364f),
                     Float3(199.705902f, 0.000024f, -399.411774f)),
            Triangle(Float3(142.647034f, 0.000024f, -399.411774f), Float3(228.235214f, 0.000019f, -313.823364f),
                     Float3(171.176590f, 0.000024f, -399.411774f)),
            Triangle(Float3(171.176590f, 0.000024f, -399.411774f), Float3(228.235214f, 0.000019f, -313.823364f),
                     Float3(199.705902f, 0.000024f, -399.411774f)),
            Triangle(Float3(85.588173f, 0.000022f, -370.882477f), Float3(228.235214f, 0.000019f, -313.823364f),
                     Float3(142.647034f, 0.000024f, -399.411774f)),
            Triangle(Float3(85.588173f, 0.000022f, -370.882477f), Float3(199.705902f, 0.000019f, -313.823364f),
                     Float3(228.235214f, 0.000019f, -313.823364f)),
            Triangle(Float3(114.117729f, 0.000024f, -399.411774f), Float3(85.588173f, 0.000022f, -370.882477f),
                     Float3(142.647034f, 0.000024f, -399.411774f)),
            Triangle(Float3(85.588173f, 0.000024f, -399.411774f), Float3(85.588173f, 0.000022f, -370.882477f),
                     Float3(114.117729f, 0.000024f, -399.411774f)),
            Triangle(Float3(28.529310f, 0.000019f, -313.823364f), Float3(199.705902f, 0.000019f, -313.823364f),
                     Float3(85.588173f, 0.000022f, -370.882477f)),
            Triangle(Float3(57.058865f, 0.000019f, -313.823364f), Float3(0.0f, 0.000017f, -285.294037f),
                     Float3(85.588173f, 0.000019f, -313.823364f)),
            Triangle(Float3(28.529310f, 0.000019f, -313.823364f), Float3(0.0f, 0.000017f, -285.294037f),
                     Float3(57.058865f, 0.000019f, -313.823364f)),
            Triangle(Float3(28.529310f, 0.000027f, -456.470642f), Float3(0.0f, 0.000017f, -285.294037f),
                     Float3(57.058865f, 0.000027f, -456.470642f)),
            Triangle(Float3(0.0f, 0.000017f, -285.294037f), Float3(28.529310f, 0.000027f, -456.470642f),
                     Float3(0.0f, 0.000031f, -511.999969f)),
            Triangle(Float3(0.0f, 0.000017f, -285.294037f), Float3(85.588173f, 0.000022f, -370.882477f),
                     Float3(85.588173f, 0.000024f, -399.411774f)),
            Triangle(Float3(0.0f, 0.000017f, -285.294037f), Float3(85.588173f, 0.000024f, -399.411774f),
                     Float3(57.058865f, 0.000027f, -456.470642f)),
            Triangle(Float3(57.058865f, 0.000027f, -456.470642f), Float3(85.588173f, 0.000024f, -399.411774f),
                     Float3(85.588173f, 0.000027f, -456.470642f)),
            Triangle(Float3(399.411804f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(456.470673f, 0.000019f, -313.823364f)),
            Triangle(Float3(456.470673f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(512.0f, 0.000019f, -313.823364f)),
            Triangle(Float3(228.235214f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(256.764771f, 0.000019f, -313.823364f)),
            Triangle(Float3(256.764771f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(285.294067f, 0.000019f, -313.823364f)),
            Triangle(Float3(285.294067f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(313.823395f, 0.000019f, -313.823364f)),
            Triangle(Float3(313.823395f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(342.352936f, 0.000019f, -313.823364f)),
            Triangle(Float3(342.352936f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(370.882507f, 0.000019f, -313.823364f)),
            Triangle(Float3(370.882507f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(399.411804f, 0.000019f, -313.823364f)),
            Triangle(Float3(0.0f, 0.000017f, -285.294037f), Float3(0.0f, 0.000009f, -142.647018f),
                     Float3(512.0f, 0.000003f, -57.058861f)),
            Triangle(Float3(199.705902f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(228.235214f, 0.000019f, -313.823364f)),
            Triangle(Float3(171.176590f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(199.705902f, 0.000019f, -313.823364f)),
            Triangle(Float3(0.0f, 0.000017f, -285.294037f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(85.588173f, 0.000019f, -313.823364f)),
            Triangle(Float3(85.588173f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(142.647034f, 0.000019f, -313.823364f)),
            Triangle(Float3(142.647034f, 0.000019f, -313.823364f), Float3(512.0f, 0.000003f, -57.058861f),
                     Float3(171.176590f, 0.000019f, -313.823364f)),
            Triangle(Float3(485.0f, 0.000002f, -28.529308f), Float3(512.0f, 0.0f, 0.0f),
                     Float3(512.0f, 0.000002f, -28.529308f)),
            Triangle(Float3(512.0f, 0.0f, 0.0f), Float3(427.941376f, 0.000002f, -28.529308f),
                     Float3(285.294067f, 0.000002f, -28.529308f)),
            Triangle(Float3(456.470673f, 0.000002f, -28.529308f), Float3(512.0f, 0.0f, 0.0f),
                     Float3(485.0f, 0.000002f, -28.529308f)),
            Triangle(Float3(427.941376f, 0.000002f, -28.529308f), Float3(512.0f, 0.0f, 0.0f),
                     Float3(456.470673f, 0.000002f, -28.529308f)),
            Triangle(Float3(171.176590f, 0.0f, 0.0f), Float3(512.0f, 0.0f, 0.0f),
                     Float3(285.294067f, 0.000002f, -28.529308f)),
            Triangle(Float3(285.294067f, 0.000002f, -28.529308f), Float3(512.0f, 0.000002f, -28.529308f),
                     Float3(512.0f, 0.000003f, -57.058861f)),
            Triangle(Float3(0.0f, 0.000009f, -142.647018f), Float3(285.294067f, 0.000002f, -28.529308f),
                     Float3(512.0f, 0.000003f, -57.058861f)),
            Triangle(Float3(0.0f, 0.000007f, -114.117722f), Float3(171.176590f, 0.0f, 0.0f),
                     Float3(0.0f, 0.000009f, -142.647018f)),
            Triangle(Float3(0.0f, 0.0f, 0.0f), Float3(171.176590f, 0.0f, 0.0f), Float3(0.0f, 0.000007f, -114.117722f)),
            Triangle(Float3(0.0f, 0.000009f, -142.647018f), Float3(171.176590f, 0.0f, 0.0f),
                     Float3(285.294067f, 0.000002f, -28.529308f))};
    MeshShapeSettings mesh_settings(triangles);
    mesh_settings.SetEmbedded();
    BodyCreationSettings floor_settings(&mesh_settings, Vec3(-256.0f, 0.0f, 256.0f), Quat::sIdentity(),
                                        EMotionType::Static, PhysicsManager::Layers::NON_MOVING);
    Body& floor = *mBodyInterface.CreateBody(floor_settings);
    mBodyInterface.AddBody(floor.GetID(), EActivation::DontActivate);
    return floor;
}

JPH::Body& PhysicsUtils::createMeshTerrain(JPH::BodyInterface& mBodyInterface) {
    const int n = 100;
    const float cell_size = worldScale * 1.0f;
    const float max_height = worldScale * 3.0f;

    // Create heights
    float heights[n + 1][n + 1];
    for (int x = 0; x <= n; ++x)
        for (int z = 0; z <= n; ++z)
            heights[x][z] = max_height * perlinNoise3(float(x) * 8.0f / n, 0, float(z) * 8.0f / n, 256, 256, 256);

    // Create regular grid of triangles
    TriangleList triangles;
    for (int x = 0; x < n; ++x)
        for (int z = 0; z < n; ++z) {
            float center = n * cell_size / 2;

            float x1 = cell_size * x - center;
            float z1 = cell_size * z - center;
            float x2 = x1 + cell_size;
            float z2 = z1 + cell_size;

            Float3 v1 = Float3(x1, heights[x][z], z1);
            Float3 v2 = Float3(x2, heights[x + 1][z], z1);
            Float3 v3 = Float3(x1, heights[x][z + 1], z2);
            Float3 v4 = Float3(x2, heights[x + 1][z + 1], z2);

            triangles.push_back(Triangle(v1, v3, v4));
            triangles.push_back(Triangle(v1, v4, v2));
        }

    // Floor
    Body& floor = *mBodyInterface.CreateBody(BodyCreationSettings(new MeshShapeSettings(triangles), Vec3::sZero(),
                                                                  Quat::sIdentity(), EMotionType::Static,
                                                                  PhysicsManager::Layers::NON_MOVING));
    mBodyInterface.AddBody(floor.GetID(), EActivation::DontActivate);
    return floor;
}

JPH::Body& PhysicsUtils::createHeightFieldTerrain(JPH::BodyInterface& mBodyInterface) {
    const int n = 128;
    const float cell_size = worldScale * 1.0f;
    const float max_height = worldScale * 5.0f;

    // Create height samples
    float heights[n * n];
    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x)
            heights[y * n + x] = max_height * perlinNoise3(float(x) * 8.0f / n, 0, float(y) * 8.0f / n, 256, 256, 256);

    // Create height field
    RefConst<ShapeSettings> height_field = new HeightFieldShapeSettings(
            heights, Vec3(-0.5f * cell_size * n, 0.0f, -0.5f * cell_size * n), Vec3(cell_size, 1.0f, cell_size), n);

    // Floor
    Body& floor = *mBodyInterface.CreateBody(BodyCreationSettings(
            height_field, Vec3::sZero(), Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
    mBodyInterface.AddBody(floor.GetID(), EActivation::DontActivate);
    return floor;
}

void PhysicsUtils::createBalancedDistribution(JPH::BodyManager* inBodyManager,
                                              int inNumBodies,
                                              float inEnvironmentSize) {
    default_random_engine random(0x1ee7c0de);
    uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);
    auto n = float(inNumBodies);
    Vec3 max_box_start = Vec3::sReplicate(inEnvironmentSize * (1.0f - pow(n, -1.0f / 3.0f)));
    Vec3 min_box_size = Vec3::sReplicate(1.0f / inEnvironmentSize);
    Vec3 max_box_size = Vec3::sReplicate(inEnvironmentSize * pow(n, -1.0f / 3.0f)) - min_box_size;
    for (int b = 0; b < inNumBodies; ++b) {
        AABox box;
        box.mMin = max_box_start * Vec3(zero_to_one(random), zero_to_one(random), zero_to_one(random)) -
                   Vec3::sReplicate(0.5f * inEnvironmentSize);
        box.mMax = box.mMin + min_box_size +
                   max_box_size * Vec3(zero_to_one(random), zero_to_one(random), zero_to_one(random));

        BodyCreationSettings s;
        s.SetShape(new BoxShape(box.GetExtent(), 0.0f));
        s.mPosition = box.GetCenter();
        s.mRotation = Quat::sIdentity();
        s.mObjectLayer = (random() % 10) == 0 ? PhysicsManager::Layers::MOVING : PhysicsManager::Layers::NON_MOVING;
        inBodyManager->CreateBody(s);
    }
}

}  // namespace vox