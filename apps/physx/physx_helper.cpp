//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physx/physx_helper.h"

#include "vox.render/entity.h"
#include "vox.render/physx/physx_dynamic_collider.h"

namespace vox {
using namespace physx;
namespace {
void GenerateArrayOfShapes(Entity* root,
                           PxShape* shape,
                           int nb_x,
                           int nb_y,
                           float altitude,
                           float scale_x,
                           float scale_z,
                           float mass,
                           Layer group,
                           const Vector3F* offset) {
    const float OneOverNbX = OneOverNb(nb_x);
    const float OneOverNbY = OneOverNb(nb_y);
    for (int y = 0; y < nb_y; y++) {
        const float CoeffY = 2.0f * ((float(y) * OneOverNbY) - 0.5f);
        for (int x = 0; x < nb_x; x++) {
            const float CoeffX = 2.0f * ((float(x) * OneOverNbX) - 0.5f);

            Vector3F Origin(CoeffX * scale_x, altitude, CoeffY * scale_z);
            if (offset) Origin += *offset;

            auto collider = root->createChild()->addComponent<PhysxDynamicCollider>();
            auto RigidBody = static_cast<PxRigidDynamic*>(collider->native_actor_);

            RigidBody->attachShape(*shape);
            RigidBody->setGlobalPose(PxTransform(PxVec3(Origin.x, Origin.y, Origin.z)));

            // collision group
            PxSetGroup(*RigidBody, group);

            if (mass > 0) {
                RigidBody->setMass(mass);
                PxRigidBodyExt::setMassAndUpdateInertia(*RigidBody, 1);
            }
        }
    }
}
}  // namespace

void GenerateArrayOfSpheres(Entity* root,
                            float radius,
                            int nb_x,
                            int nb_y,
                            float altitude,
                            float scale_x,
                            float scale_z,
                            float mass,
                            Layer group,
                            const Vector3F* offset) {
    auto native_material_ = PhysxManager::GetSingleton().physics->createMaterial(0, 0, 0);
    auto native_geometry_ = std::make_shared<PxSphereGeometry>(radius);
    auto native_shape_ = PhysxManager::GetSingleton().physics->createShape(*native_geometry_, *native_material_, true);

    GenerateArrayOfShapes(root, native_shape_, nb_x, nb_y, altitude, scale_x, scale_z, mass, group, offset);
}

void GenerateArrayOfBoxes(Entity* root,
                          const Vector3F& extents,
                          int nb_x,
                          int nb_y,
                          float altitude,
                          float scale_x,
                          float scale_z,
                          float mass,
                          Layer group,
                          const Vector3F* offset) {
    auto native_material_ = PhysxManager::GetSingleton().physics->createMaterial(0, 0, 0);
    auto native_geometry_ = std::make_shared<PxBoxGeometry>(extents.x, extents.y, extents.z);
    auto native_shape_ = PhysxManager::GetSingleton().physics->createShape(*native_geometry_, *native_material_, true);

    GenerateArrayOfShapes(root, native_shape_, nb_x, nb_y, altitude, scale_x, scale_z, mass, group, offset);
}

void GenerateArrayOfCapsules(Entity* root,
                             float radius,
                             float half_height,
                             int nb_x,
                             int nb_y,
                             float altitude,
                             float scale_x,
                             float scale_z,
                             float mass,
                             Layer group,
                             const Vector3F* offset) {
    auto native_material_ = PhysxManager::GetSingleton().physics->createMaterial(0, 0, 0);
    auto native_geometry_ = std::make_shared<PxCapsuleGeometry>(radius, half_height * 2.0f);
    auto native_shape_ = PhysxManager::GetSingleton().physics->createShape(*native_geometry_, *native_material_, true);

    GenerateArrayOfShapes(root, native_shape_, nb_x, nb_y, altitude, scale_x, scale_z, mass, group, offset);
}

}  // namespace vox