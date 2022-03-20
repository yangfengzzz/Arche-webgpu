//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef drawable_h
#define drawable_h

namespace vox {
namespace ui {
/**
 * Interface for any drawable class
 */
class Drawable {
public:
    virtual void draw() = 0;
    
protected:
    virtual ~Drawable() = default;
};


}
}
#endif /* drawable_h */
