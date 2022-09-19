//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/light.h"

#include "vox.render/entity.h"

namespace vox {
Light::Light(Entity *entity) : Component(entity) {}

Matrix4x4F Light::viewMatrix() { return entity()->transform->worldMatrix().inverse(); }

Matrix4x4F Light::inverseViewMatrix() { return entity()->transform->worldMatrix(); }

}  // namespace vox
