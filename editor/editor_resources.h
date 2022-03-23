//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef editor_resources_hpp
#define editor_resources_hpp

#include <string>
#include <unordered_map>
#include <webgpu/webgpu_cpp.h>

namespace vox {
namespace editor {
/**
 * Handle the creation and storage of editor specific resources
 */
class EditorResources {
public:
    /**
     * Constructor
     * @param p_editorAssetsPath p_editorAssetsPath
     */
    EditorResources(wgpu::Device& device,
                    const std::string& p_editorAssetsPath);
    
    /**
     * Destructor
     */
    ~EditorResources();
    
    /**
     * Returns the file icon identified by the given string or nullptr on fail
     * @param p_filename p_filename
     */
    wgpu::TextureView getFileIcon(const std::string& p_filename);
    
    /**
     * Returns the texture identified by the given string or nullptr on fail
     * @param p_id p_id
     */
    wgpu::TextureView getTexture(const std::string& p_id);
    
private:
    wgpu::Device& _device;
    std::unordered_map<std::string, wgpu::Texture> _textures;
    
    wgpu::Texture _createFromPixelBuffer(const std::vector<uint64_t>& data, uint8_t width);
    
    wgpu::ImageCopyTexture
    _createImageCopyTexture(wgpu::Texture texture,
                            uint32_t level,
                            wgpu::Origin3D origin,
                            wgpu::TextureAspect aspect = wgpu::TextureAspect::All);
    
    wgpu::TextureDataLayout
    _createTextureDataLayout(uint64_t offset,
                             uint32_t bytesPerRow,
                             uint32_t rowsPerImage = wgpu::kCopyStrideUndefined);
};


}
}
#endif /* editor_resources_hpp */
