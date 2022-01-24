//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "collision_utils.h"
#include "ray.h"

namespace vox {

float distancePlaneAndPoint(const BoundingPlane3F &plane, const Point3F &point) {
    return point.dot(plane.normal) + plane.distance;
}

PlaneIntersectionType intersectsPlaneAndPoint(const BoundingPlane3F &plane, const Point3F &point) {
    const auto distance = distancePlaneAndPoint(plane, point);
    if (distance > 0) {
        return PlaneIntersectionType::Front;
    }
    if (distance < 0) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType intersectsPlaneAndBox(const BoundingPlane3F &plane, const BoundingBox3F &box) {
    const auto &min = box.lowerCorner;
    const auto &max = box.upperCorner;
    const auto &normal = plane.normal;
    Point3F front;
    Point3F back;
    
    if (normal.x >= 0) {
        front.x = max.x;
        back.x = min.x;
    } else {
        front.x = min.x;
        back.x = max.x;
    }
    if (normal.y >= 0) {
        front.y = max.y;
        back.y = min.y;
    } else {
        front.y = min.y;
        back.y = max.y;
    }
    if (normal.z >= 0) {
        front.z = max.z;
        back.z = min.z;
    } else {
        front.z = min.z;
        back.z = max.z;
    }
    
    if (distancePlaneAndPoint(plane, front) < 0) {
        return PlaneIntersectionType::Back;
    }
    
    if (distancePlaneAndPoint(plane, back) > 0) {
        return PlaneIntersectionType::Front;
    }
    
    return PlaneIntersectionType::Intersecting;
}

float intersectsRayAndPlane(const Ray3F &ray, const BoundingPlane3F &plane) {
    const auto &normal = plane.normal;
    
    const auto dir = normal.dot(ray.direction);
    // Parallel
    if (std::abs(dir) < kEpsilonF) {
        return -1;
    }
    
    const auto position = ray.origin.dot(normal);
    auto distance = (-plane.distance - position) / dir;
    
    if (distance < 0) {
        if (distance < -kEpsilonF) {
            return -1;
        }
        
        distance = 0;
    }
    
    return distance;
}

float intersectsRayAndBox(const Ray3F &ray, const BoundingBox3F &box) {
    const auto &origin = ray.origin;
    const auto &direction = ray.direction;
    const auto &min = box.lowerCorner;
    const auto &max = box.upperCorner;
    const auto &dirX = direction.x;
    const auto &dirY = direction.y;
    const auto &dirZ = direction.z;
    const auto &oriX = origin.x;
    const auto &oriY = origin.y;
    const auto &oriZ = origin.z;
    float distance = 0;
    float tmax = std::numeric_limits<float>::max();
    
    if (std::abs(dirX) < kEpsilonF) {
        if (oriX < min.x || oriX > max.x) {
            return -1;
        }
    } else {
        const float inverse = 1.0 / dirX;
        float t1 = (min.x - oriX) * inverse;
        float t2 = (max.x - oriX) * inverse;
        
        if (t1 > t2) {
            const auto temp = t1;
            t1 = t2;
            t2 = temp;
        }
        
        distance = std::max(t1, distance);
        tmax = std::min(t2, tmax);
        
        if (distance > tmax) {
            return -1;
        }
    }
    
    if (std::abs(dirY) < kEpsilonF) {
        if (oriY < min.y || oriY > max.y) {
            return -1;
        }
    } else {
        const float inverse = 1.0 / dirY;
        float t1 = (min.y - oriY) * inverse;
        float t2 = (max.y - oriY) * inverse;
        
        if (t1 > t2) {
            const auto temp = t1;
            t1 = t2;
            t2 = temp;
        }
        
        distance = std::max(t1, distance);
        tmax = std::min(t2, tmax);
        
        if (distance > tmax) {
            return -1;
        }
    }
    
    if (std::abs(dirZ) < kEpsilonF) {
        if (oriZ < min.z || oriZ > max.z) {
            return -1;
        }
    } else {
        const float inverse = 1.0 / dirZ;
        float t1 = (min.z - oriZ) * inverse;
        float t2 = (max.z - oriZ) * inverse;
        
        if (t1 > t2) {
            const auto temp = t1;
            t1 = t2;
            t2 = temp;
        }
        
        distance = std::max(t1, distance);
        tmax = std::min(t2, tmax);
        
        if (distance > tmax) {
            return -1;
        }
    }
    
    return distance;
}

bool intersectsBoxAndBox(const BoundingBox3F &boxA, const BoundingBox3F &boxB) {
    if (boxA.lowerCorner.x > boxB.upperCorner.x || boxB.lowerCorner.x > boxA.upperCorner.x) {
        return false;
    }
    
    if (boxA.lowerCorner.y > boxB.upperCorner.y || boxB.lowerCorner.y > boxA.upperCorner.y) {
        return false;
    }
    
    return !(boxA.lowerCorner.z > boxB.upperCorner.z || boxB.lowerCorner.z > boxA.upperCorner.z);
}

bool intersectsFrustumAndBox(const BoundingFrustum &frustum, const BoundingBox3F &box) {
    const auto &min = box.lowerCorner;
    const auto &max = box.upperCorner;
    Vector3F back;
    
    for (int i = 0; i < 6; ++i) {
        const auto plane = frustum.getPlane(i);
        const auto &normal = plane.normal;
        
        back.x = normal.x >= 0 ? min.x : max.x;
        back.y = normal.y >= 0 ? min.y : max.y;
        back.z = normal.z >= 0 ? min.z : max.z;
        if (plane.normal.dot(back) > -plane.distance) {
            return false;
        }
    }
    
    return true;
}

ContainmentType frustumContainsBox(const BoundingFrustum &frustum, const BoundingBox3F &box) {
    const auto &min = box.lowerCorner;
    const auto &max = box.upperCorner;
    Point3F front;
    Point3F back;
    auto result = ContainmentType::Contains;
    
    for (int i = 0; i < 6; ++i) {
        const auto plane = frustum.getPlane(i);
        const auto &normal = plane.normal;
        
        if (normal.x >= 0) {
            front.x = max.x;
            back.x = min.x;
        } else {
            front.x = min.x;
            back.x = max.x;
        }
        if (normal.y >= 0) {
            front.y = max.y;
            back.y = min.y;
        } else {
            front.y = min.y;
            back.y = max.y;
        }
        if (normal.z >= 0) {
            front.z = max.z;
            back.z = min.z;
        } else {
            front.z = min.z;
            back.z = max.z;
        }
        
        if (intersectsPlaneAndPoint(plane, back) == PlaneIntersectionType::Front) {
            return ContainmentType::Disjoint;
        }
        
        if (intersectsPlaneAndPoint(plane, front) == PlaneIntersectionType::Front) {
            result = ContainmentType::Intersects;
        }
    }
    
    return result;
}

}
