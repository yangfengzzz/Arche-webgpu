//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/render_element.h"

#include <utility>

namespace vox {
RenderElement::RenderElement(Renderer *renderer, MeshPtr mesh, const SubMesh *subMesh, MaterialPtr material)
    : renderer(renderer), mesh(std::move(mesh)), subMesh(subMesh), material(std::move(material)) {}

}  // namespace vox
