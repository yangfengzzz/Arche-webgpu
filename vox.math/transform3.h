//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_TRANSFORM3_H_
#define INCLUDE_VOX_TRANSFORM3_H_

#include "bounding_box3.h"
#include "quaternion.h"
#include "ray3.h"
#include "vector3.h"

namespace vox {

//!
//! \brief Represents 3-D rigid body transform.
//!
template<typename T>
class Transform3 {
public:
    //! Constructs identity transform.
    Transform3();
    
    //! Constructs a transform with translation and orientation.
    Transform3(const Vector3<T> &translation, const Quaternion<T> &orientation);
    
    //! Returns the translation.
    const Vector3<T> &translation() const;
    
    //! Sets the traslation.
    void setTranslation(const Vector3<T> &translation);
    
    //! Returns the orientation.
    const Quaternion<T> &orientation() const;
    
    //! Sets the orientation.
    void setOrientation(const Quaternion<T> &orientation);
    
    //! Transforms a point in world coordinate to the local frame.
    Point3<T> toLocal(const Point3<T> &pointInWorld) const;
    
    //! Transforms a direction in world coordinate to the local frame.
    Vector3<T> toLocalDirection(const Vector3<T> &dirInWorld) const;
    
    //! Transforms a ray in world coordinate to the local frame.
    Ray3<T> toLocal(const Ray3<T> &rayInWorld) const;
    
    //! Transforms a bounding box in world coordinate to the local frame.
    BoundingBox3<T> toLocal(const BoundingBox3<T> &bboxInWorld) const;
    
    //! Transforms a point in local space to the world coordinate.
    Point3<T> toWorld(const Point3<T> &pointInLocal) const;
    
    //! Transforms a direction in local space to the world coordinate.
    Vector3<T> toWorldDirection(const Vector3<T> &dirInLocal) const;
    
    //! Transforms a ray in local space to the world coordinate.
    Ray3<T> toWorld(const Ray3<T> &rayInLocal) const;
    
    //! Transforms a bounding box in local space to the world coordinate.
    BoundingBox3<T> toWorld(const BoundingBox3<T> &bboxInLocal) const;
    
private:
    Vector3<T> _translation;
    Quaternion<T> _orientation;
    Matrix3x3<T> _orientationMat3;
    Matrix3x3<T> _inverseOrientationMat3;
};

//! Float-type 3-D Transform.
using Transform3F = Transform3<float>;

//! Double-type 3-D Transform.
using Transform3D = Transform3<double>;

}  // namespace vox

#include "transform3-inl.h"

#endif  // INCLUDE_VOX_TRANSFORM3_H_
