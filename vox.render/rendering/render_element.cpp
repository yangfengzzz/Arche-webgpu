//
//  render_element.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "render_element.h"

namespace vox {
RenderElement::RenderElement(Renderer *renderer, MeshPtr mesh,
                             const SubMesh *subMesh, MaterialPtr material):
renderer(renderer),
mesh(mesh),
subMesh(subMesh),
material(material) {
}

}
