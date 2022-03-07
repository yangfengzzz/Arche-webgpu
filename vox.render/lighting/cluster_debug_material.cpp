//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cluster_debug_material.h"

namespace vox {
ClusterDebugMaterial::ClusterDebugMaterial(wgpu::Device& device):
BaseMaterial(device, Shader::find("cluster_debug")) {
}

}
