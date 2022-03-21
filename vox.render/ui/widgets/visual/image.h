//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef ui_image_hpp
#define ui_image_hpp

#include "ui/widgets/widget.h"
#include "vector2.h"
#include <webgpu/webgpu_cpp.h>

namespace vox {
namespace ui {
/**
 * Simple widget that display an image
 */
class Image : public Widget {
public:
    /**
     * Constructor
     * @param p_textureID p_textureID
     * @parma p_size p_size
     */
    Image(wgpu::TextureView p_textureID, const Vector2F &p_size);
    
protected:
    void _draw_Impl() override;
    
public:
    wgpu::TextureView textureID;
    Vector2F size;
};

}
}
#endif /* ui_image_hpp */
