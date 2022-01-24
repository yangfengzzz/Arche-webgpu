//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_QUADTREE_H_
#define INCLUDE_VOX_QUADTREE_H_

#include "intersection_query_engine2.h"
#include "nearest_neighbor_query_engine2.h"
#include <vector>

namespace vox {

//!
//! \brief      Generic quadtree data structure.
//!
//! This class is a generic quadtree representation to store arbitrary spatial
//! data. The quadtree supports closest neighbor search, overlapping test, and
//! ray intersection test.
//!
//! \tparam     T     Value type.
//!
template<typename T>
class Quadtree final : public IntersectionQueryEngine2<T>,
public NearestNeighborQueryEngine2<T> {
public:
    using ContainerType = std::vector<T>;
    using Iterator = typename ContainerType::iterator;
    using ConstIterator = typename ContainerType::const_iterator;
    
    //! Default constructor.
    Quadtree();
    
    //! Builds an quadtree with given list of items, bounding box of the items,
    //! overlapping test function, and max depth of the tree.
    void build(const std::vector<T> &items, const BoundingBox2D &bound,
               const BoxIntersectionTestFunc2<T> &testFunc, size_t maxDepth);
    
    //! Clears all the contents of this instance.
    void clear();
    
    //! Returns the nearest neighbor for given point and distance measure
    //! function.
    NearestNeighborQueryResult2<T>
    nearest(const Point2D &pt, const NearestNeighborDistanceFunc2<T> &distanceFunc) const override;
    
    //! Returns true if given \p box intersects with any of the stored items.
    bool intersects(const BoundingBox2D &box,
                    const BoxIntersectionTestFunc2<T> &testFunc) const override;
    
    //! Returns true if given \p ray intersects with any of the stored items.
    bool intersects(const Ray2D &ray,
                    const RayIntersectionTestFunc2<T> &testFunc) const override;
    
    //! Invokes \p visitorFunc for every intersecting items.
    void forEachIntersectingItem(
                                 const BoundingBox2D &box, const BoxIntersectionTestFunc2<T> &testFunc,
                                 const IntersectionVisitorFunc2<T> &visitorFunc) const override;
    
    //! Invokes \p visitorFunc for every intersecting items.
    void forEachIntersectingItem(
                                 const Ray2D &ray, const RayIntersectionTestFunc2<T> &testFunc,
                                 const IntersectionVisitorFunc2<T> &visitorFunc) const override;
    
    //! Returns the closest intersection for given \p ray.
    ClosestIntersectionQueryResult2<T>
    closestIntersection(const Ray2D &ray, const GetRayIntersectionFunc2<T> &testFunc) const override;
    
    //! Returns the begin iterator of the item.
    Iterator begin();
    
    //! Returns the end iterator of the item.
    Iterator end();
    
    //! Returns the immutable begin iterator of the item.
    ConstIterator begin() const;
    
    //! Returns the immutable end iterator of the item.
    ConstIterator end() const;
    
    //! Returns the number of items.
    size_t numberOfItems() const;
    
    //! Returns the item at \p i.
    const T &item(size_t i) const;
    
    //! Returns the number of quadtree nodes.
    size_t numberOfNodes() const;
    
    //! Returns the list of the items for given noide index.
    const std::vector<size_t> &itemsAtNode(size_t nodeIdx) const;
    
    //!
    //! \brief      Returns a child's index for given node.
    //!
    //! For a given node, its children is stored continuously, such that if the
    //! node's first child's index is i, then i + 1, i + 2, ... , i + 7 are the
    //! indices for its children. The order of octant is x-major.
    //!
    //! \param[in]  nodeIdx The node index.
    //! \param[in]  childIdx The child index (0 to 7).
    //!
    //! \return     Index of the selected child.
    //!
    size_t childIndex(size_t nodeIdx, size_t childIdx) const;
    
    //! Returns the bounding box of this quadtree.
    const BoundingBox2D &boundingBox() const;
    
    //! Returns the maximum depth of the tree.
    size_t maxDepth() const;
    
private:
    struct Node {
        size_t firstChild = kMaxSize;
        std::vector<size_t> items;
        
        bool isLeaf() const;
    };
    
    size_t _maxDepth = 1;
    BoundingBox2D _bbox;
    std::vector<T> _items;
    std::vector<Node> _nodes;
    
    void build(size_t nodeIdx, size_t currentDepth,
               const BoundingBox2D &currentBound,
               const BoxIntersectionTestFunc2<T> &overlapsFunc);
    
    bool intersects(const BoundingBox2D &box,
                    const BoxIntersectionTestFunc2<T> &testFunc, size_t nodeIdx,
                    const BoundingBox2D &currentBound) const;
    
    bool intersects(const Ray2D &ray,
                    const RayIntersectionTestFunc2<T> &testFunc, size_t nodeIdx,
                    const BoundingBox2D &currentBound) const;
    
    void forEachIntersectingItem(const BoundingBox2D &box,
                                 const BoxIntersectionTestFunc2<T> &testFunc,
                                 const IntersectionVisitorFunc2<T> &visitorFunc,
                                 size_t nodeIdx,
                                 const BoundingBox2D &currentBound) const;
    
    void forEachIntersectingItem(const Ray2D &ray,
                                 const RayIntersectionTestFunc2<T> &testFunc,
                                 const IntersectionVisitorFunc2<T> &visitorFunc,
                                 size_t nodeIdx,
                                 const BoundingBox2D &currentBound) const;
    
    ClosestIntersectionQueryResult2<T> closestIntersection(const Ray2D &ray, const GetRayIntersectionFunc2<T> &testFunc,
                                                           size_t nodeIdx, const BoundingBox2D &currentBound,
                                                           ClosestIntersectionQueryResult2<T> best) const;
};

}  // namespace vox

#include "quadtree-inl.h"

#endif  // INCLUDE_VOX_QUADTREE_H_
