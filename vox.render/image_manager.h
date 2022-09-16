//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/image.h"
#include "vox.render/singleton.h"

namespace vox {
class ImageManager : public Singleton<ImageManager> {
public:
    static ImageManager &GetSingleton();

    static ImageManager *GetSingletonPtr();

    explicit ImageManager(wgpu::Device &device);

    ~ImageManager() = default;

    void collectGarbage();

private:
    wgpu::Device &_device;
    std::unordered_map<std::string, std::shared_ptr<Image>> _image_pool{};
};
template <>
inline ImageManager *Singleton<ImageManager>::ms_singleton{nullptr};

}  // namespace vox