//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef plane3_h
#define plane3_h

#include "vector3.h"
#include "point3.h"
#include "bounding_plane.h"

namespace vox {

//!
//! \brief      Class for 3-D plane.
//!
//! \tparam     T     The value type.
//!
template<typename T>
class BoundingPlane<T, 3> final {
public:
    static_assert(std::is_floating_point<T>::value,
                  "Plane only can be instantiated with floating point types");
    
    //! The normal of the plane.
    Vector3<T> normal;
    
    //! The distance of the plane along its normal to the origin.
    T distance;
    
    //! Constructs an empty plane that points (1, 0) from (0, 0).
    BoundingPlane();
    
    //! Constructs a plane with given origin and riection.
    BoundingPlane(const Vector3<T> &newNormal, const T &newDistance);
    
    /**
     * Calculate the plane that contains the three specified points.
     * @param point0 - The first point
     * @param point1 - The second point
     * @param point2 - The third point
     */
    BoundingPlane(const Point3<T> &point0, const Point3<T> &point1, const Point3<T> &point2);
    
    //! Copy constructor.
    BoundingPlane(const BoundingPlane &other);
    
    /**
     * Normalize the normal vector of this plane.
     * @returns The plane after normalize
     */
    BoundingPlane<T, 3> normalized() const;
    
    /**
     * Normalize the normal vector of this plane.
     */
    void normalize();
};

//! Type alias for 3-D plane.
template<typename T> using BoundingPlane3 = BoundingPlane<T, 3>;

//! Float-type 3-D plane.
using BoundingPlane3F = BoundingPlane3<float>;

//! Double-type 3-D plane.
using BoundingPlane3D = BoundingPlane3<double>;

}  // namespace vox

#include "bounding_plane3-inl.h"

#endif /* plane3_h */
