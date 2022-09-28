//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/macros.h"

namespace vox {

// Defines a rectangle by the integer coordinates of its lower-left and
// width-height.
struct VOX_BASE_DLL RectInt {
    // Constructs a uninitialized rectangle.
    RectInt() = default;

    // Constructs a rectangle with the specified arguments.
    RectInt(int _left, int _bottom, int _width, int _height)
        : left(_left), bottom(_bottom), width(_width), height(_height) {}

    // Tests whether _x and _y coordinates are within rectangle bounds.
    [[nodiscard]] bool is_inside(int _x, int _y) const {
        return _x >= left && _x < left + width && _y >= bottom && _y < bottom + height;
    }

    // Gets the rectangle x coordinate of the right rectangle side.
    [[nodiscard]] int right() const { return left + width; }

    // Gets the rectangle y coordinate of the top rectangle side.
    [[nodiscard]] int top() const { return bottom + height; }

    // Specifies the x-coordinate of the lower side.
    int left;
    // Specifies the x-coordinate of the left side.
    int bottom;
    // Specifies the width of the rectangle.
    int width;
    // Specifies the height of the rectangle..
    int height;
};

// Defines a rectangle by the floating point coordinates of its lower-left
// and width-height.
struct VOX_BASE_DLL RectFloat {
    // Constructs a uninitialized rectangle.
    RectFloat() = default;

    // Constructs a rectangle with the specified arguments.
    RectFloat(float _left, float _bottom, float _width, float _height)
        : left(_left), bottom(_bottom), width(_width), height(_height) {}

    // Tests whether _x and _y coordinates are within rectangle bounds
    [[nodiscard]] bool is_inside(float _x, float _y) const {
        return _x >= left && _x < left + width && _y >= bottom && _y < bottom + height;
    }

    // Gets the rectangle x coordinate of the right rectangle side.
    [[nodiscard]] float right() const { return left + width; }

    // Gets the rectangle y coordinate of the top rectangle side.
    [[nodiscard]] float top() const { return bottom + height; }

    // Specifies the x-coordinate of the lower side.
    float left;
    // Specifies the x-coordinate of the left side.
    float bottom;
    // Specifies the width of the rectangle.
    float width;
    // Specifies the height of the rectangle.
    float height;
};
}  // namespace vox
