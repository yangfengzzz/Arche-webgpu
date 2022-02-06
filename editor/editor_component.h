//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef editor_component_hpp
#define editor_component_hpp

#include <stdio.h>

namespace vox {
namespace editor {
class EditorComponent {
public:
    virtual ~EditorComponent() {
    }
    
    virtual void onUpdate() = 0;
};

}
}

#endif /* editor_component_hpp */
