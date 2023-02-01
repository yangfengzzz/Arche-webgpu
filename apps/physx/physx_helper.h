//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
#include "vox.render/layer.h"

namespace vox {
static const float gValveScale = 1.0f;

static const float gSQMaxDist = 1000.0f;

inline float OneOverNb(int nb) { return nb > 1 ? 1.0f / float(nb - 1) : 0.0f; }

void GenerateArrayOfSpheres(Entity* root,
                            float radius,
                            int nb_x,
                            int nb_y,
                            float altitude,
                            float scale_x,
                            float scale_z,
                            float mass,
                            Layer group,
                            const Vector3F* offset = nullptr);

void GenerateArrayOfBoxes(Entity* root,
                          const Vector3F& extents,
                          int nb_x,
                          int nb_y,
                          float altitude,
                          float scale_x,
                          float scale_z,
                          float mass,
                          Layer group,
                          const Vector3F* offset = nullptr);

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
                             const Vector3F* offset = nullptr);
}  // namespace vox