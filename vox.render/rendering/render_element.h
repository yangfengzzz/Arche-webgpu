//
//  render_element.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef render_element_hpp
#define render_element_hpp

#include "scene_forward.h"

namespace vox {
/**
 * Render element.
 */
struct RenderElement {
    /** Render component. */
    Renderer *renderer;
    /** Mesh. */
    MeshPtr mesh;
    /** Sub mesh. */
    const Submesh *subMesh;
    /** Material. */
    MaterialPtr material;
    
    RenderElement(Renderer *renderer, MeshPtr mesh, const Submesh *subMesh, MaterialPtr material);
};

}

#endif /* render_element_hpp */
