//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/image_manager.h"

namespace vox {
ImageManager *ImageManager::GetSingletonPtr() { return ms_singleton; }

ImageManager &ImageManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

ImageManager::ImageManager(wgpu::Device &device) : _device(device) {}

}  // namespace vox
