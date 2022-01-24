//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_TRANSFORM2_H_
#define INCLUDE_VOX_TRANSFORM2_H_

#include "bounding_box2.h"
#include "ray2.h"
#include "vector2.h"

namespace vox {

//!
//! \brief Represents 2-D rigid body transform.
//!
template<typename T>
class Transform2 {
public:
    //! Constructs identity transform.
    Transform2();
    
    //! Constructs a transform with translation and orientation.
    Transform2(const Vector2<T> &translation, double orientation);
    
    //! Returns the translation.
    const Vector2<T> &translation() const;
    
    //! Sets the traslation.
    void setTranslation(const Vector2<T> &translation);
    
    //! Returns the orientation in radians.
    double orientation() const;
    
    //! Sets the orientation in radians.
    void setOrientation(double orientation);
    
    //! Transforms a point in world coordinate to the local frame.
    Point2<T> toLocal(const Point2<T> &pointInWorld) const;
    
    //! Transforms a direction in world coordinate to the local frame.
    Vector2<T> toLocalDirection(const Vector2<T> &dirInWorld) const;
    
    //! Transforms a ray in world coordinate to the local frame.
    Ray2<T> toLocal(const Ray2<T> &rayInWorld) const;
    
    //! Transforms a bounding box in world coordinate to the local frame.
    BoundingBox2<T> toLocal(const BoundingBox2<T> &bboxInWorld) const;
    
    //! Transforms a point in local space to the world coordinate.
    Point2<T> toWorld(const Point2<T> &pointInLocal) const;
    
    //! Transforms a direction in local space to the world coordinate.
    Vector2<T> toWorldDirection(const Vector2<T> &dirInLocal) const;
    
    //! Transforms a ray in local space to the world coordinate.
    Ray2<T> toWorld(const Ray2<T> &rayInLocal) const;
    
    //! Transforms a bounding box in local space to the world coordinate.
    BoundingBox2<T> toWorld(const BoundingBox2<T> &bboxInLocal) const;
    
private:
    Vector2<T> _translation;
    double _orientation = 0.0;
    double _cosAngle = 1.0;
    double _sinAngle = 0.0;
};

//! Float-type 2-D Transform.
using Transform2F = Transform2<float>;

//! Double-type 2-D Transform.
using Transform2D = Transform2<double>;

}  // namespace vox

#include "transform2-inl.h"

#endif  // INCLUDE_VOX_TRANSFORM2_H_
