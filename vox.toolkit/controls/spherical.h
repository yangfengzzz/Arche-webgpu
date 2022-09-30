//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector3.h"

namespace vox::control {
// Spherical.
class Spherical {
public:
    explicit Spherical(float radius = 1.0, float phi = 0, float theta = 0);

    void set(float radius, float phi, float theta);

    void makeSafe();

    void setFromVec3(const Vector3F &v3);

    void setToVec3(Vector3F &v3) const;

private:
    friend class OrbitControl;

    friend class FreeControl;

    float _radius;
    float _phi;
    float _theta;
};

}  // namespace vox::control