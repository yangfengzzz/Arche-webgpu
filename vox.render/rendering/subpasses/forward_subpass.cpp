//
//  forward_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/25.
//

#include "forward_subpass.h"

namespace vox {
ForwardSubpass::ForwardSubpass(RenderContext* renderContext,
                               Scene* scene,
                               Camera* camera):
Subpass(renderContext, scene, camera) {
}

}
