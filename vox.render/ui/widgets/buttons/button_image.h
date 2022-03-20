//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef button_image_hpp
#define button_image_hpp

#include "button.h"
#include "vector2.h"
#include "color.h"
#include <webgpu/webgpu_cpp.h>

namespace vox {
namespace ui {
/**
 * Button widget with an image
 */
class ButtonImage : public Button {
public:
    /**
     * Constructor
     * @param p_textureID p_textureID
     * @param p_size p_size
     */
    ButtonImage(wgpu::TextureView p_textureID, const Vector2F &p_size);
    
protected:
    void _draw_Impl() override;
    
public:
    bool disabled = false;
    
    Color background = {0, 0, 0, 0};
    Color tint = {1, 1, 1, 1};
    
    wgpu::TextureView textureID;
    Vector2F size;
};


}
}
#endif /* button_image_hpp */
