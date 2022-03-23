//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_resources.h"
#include "raw_icon.h"
#include "filesystem.h"
#include <vector>

namespace vox {
namespace editor {
EditorResources::EditorResources(wgpu::Device& device,
                                 const std::string& p_editorAssetsPath):
_device(device) {
    /* Buttons */
    {
        _textures["Button_Play"] = _createFromPixelBuffer(BUTTON_PLAY, 64);
        _textures["Button_Pause"] = _createFromPixelBuffer(BUTTON_PAUSE, 64);
        _textures["Button_Stop"] = _createFromPixelBuffer(BUTTON_STOP, 64);
        _textures["Button_Next"] = _createFromPixelBuffer(BUTTON_NEXT, 64);
        _textures["Button_Refresh"] = _createFromPixelBuffer(BUTTON_REFRESH, 64);
    }
    
    /* Icons */
    {
        _textures["Icon_Unknown"] = _createFromPixelBuffer(ICON_FILE, 16);
        _textures["Icon_Folder"] = _createFromPixelBuffer(ICON_FOLDER, 16);
        _textures["Icon_Texture"] = _createFromPixelBuffer(ICON_TEXTURE, 16);
        _textures["Icon_Model"] = _createFromPixelBuffer(ICON_MODEL, 16);
        _textures["Icon_Shader"] = _createFromPixelBuffer(ICON_SHADER, 16);
        _textures["Icon_Material"] = _createFromPixelBuffer(ICON_MATERIAL, 16);
        _textures["Icon_Scene"] = _createFromPixelBuffer(ICON_SCENE, 16);
        _textures["Icon_Sound"] = _createFromPixelBuffer(ICON_SOUND, 16);
        _textures["Icon_Script"] = _createFromPixelBuffer(ICON_SCRIPT, 16);
        _textures["Icon_Font"] = _createFromPixelBuffer(ICON_FONT, 16);
        
        _textures["Bill_Point_Light"] = _createFromPixelBuffer(BILL_PLIGHT, 128);
        _textures["Bill_Spot_Light"] = _createFromPixelBuffer(BILL_SLIGHT, 128);
        _textures["Bill_Directional_Light"] = _createFromPixelBuffer(BILL_DLIGHT, 128);
        _textures["Bill_Ambient_Box_Light"] = _createFromPixelBuffer(BILL_ABLIGHT, 128);
        _textures["Bill_Ambient_Sphere_Light"] = _createFromPixelBuffer(BILL_ASLIGHT, 128);
        
    }
}

EditorResources::~EditorResources() {
    
}

wgpu::TextureView EditorResources::getFileIcon(const std::string& p_filename) {
    return getTexture("Icon_" + fs::fileTypeToString(fs::extraFileType(p_filename)));
}


wgpu::TextureView EditorResources::getTexture(const std::string& p_id) {
    if (_textures.find(p_id) != _textures.end())
        return _textures.at(p_id).CreateView();
    
    return nullptr;
}

//MARK: - Helper
wgpu::Texture EditorResources::_createFromPixelBuffer(const std::vector<uint64_t>& data, uint8_t width) {
    wgpu::TextureDescriptor textureDesc;
    textureDesc.size.width = width;
    textureDesc.size.height = width;
    textureDesc.size.depthOrArrayLayers = 1;
    textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
    textureDesc.usage = wgpu::TextureUsage::TextureBinding  | wgpu::TextureUsage::CopyDst;
    textureDesc.mipLevelCount = 1;
    auto nativeTexture = _device.CreateTexture(&textureDesc);
    
    wgpu::ImageCopyTexture imageCopyTexture = _createImageCopyTexture(nativeTexture, 0, {0, 0, 0});
    wgpu::TextureDataLayout textureLayout = _createTextureDataLayout(0, 4 * width, wgpu::kCopyStrideUndefined);
    wgpu::Extent3D copySize = {width, width, 1};
    _device.GetQueue().WriteTexture(&imageCopyTexture, data.data(), sizeof(uint64_t) * data.size(), &textureLayout, &copySize);
    
    return nativeTexture;
}

wgpu::ImageCopyTexture EditorResources::_createImageCopyTexture(wgpu::Texture texture,
                                                                uint32_t mipLevel,
                                                                wgpu::Origin3D origin,
                                                                wgpu::TextureAspect aspect) {
    wgpu::ImageCopyTexture imageCopyTexture;
    imageCopyTexture.texture = texture;
    imageCopyTexture.mipLevel = mipLevel;
    imageCopyTexture.origin = origin;
    imageCopyTexture.aspect = aspect;
    
    return imageCopyTexture;
}

wgpu::TextureDataLayout EditorResources::_createTextureDataLayout(uint64_t offset,
                                                                  uint32_t bytesPerRow,
                                                                  uint32_t rowsPerImage) {
    wgpu::TextureDataLayout textureDataLayout;
    textureDataLayout.offset = offset;
    textureDataLayout.bytesPerRow = bytesPerRow;
    textureDataLayout.rowsPerImage = rowsPerImage;
    
    return textureDataLayout;
}


}
}
