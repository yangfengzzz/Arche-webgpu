//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gui_hpp
#define gui_hpp

#include "gui/imgui.h"
#include "rendering/render_context.h"

namespace vox {
class GUI {
public:
    GUI(RenderContext* context);
    
    ~GUI();
    
    void newFrame();
    
    void draw(ImDrawData* drawData,
              wgpu::RenderPassEncoder& passEncoder);
};

}

#endif /* gui_hpp */
