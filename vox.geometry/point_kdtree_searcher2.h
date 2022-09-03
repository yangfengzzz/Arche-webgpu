//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.geometry/kdtree.h"
#include "vox.geometry/point_neighbor_searcher2.h"
#include "vox.math/point2.h"
#include "vox.math/size2.h"

namespace vox {

//!
//! \brief KdTree-based 2-D point searcher.
//!
//! This class implements 2-D point searcher by using KdTree for its internal
//! acceleration data structure.
//!
class PointKdTreeSearcher2 final : public PointNeighborSearcher2 {
public:
    JET_NEIGHBOR_SEARCHER2_TYPE_NAME(PointKdTreeSearcher2)

    class Builder;

    //! Constructs an empty kD-tree instance.
    PointKdTreeSearcher2();

    //! Copy constructor.
    PointKdTreeSearcher2(const PointKdTreeSearcher2& other);

    //! Builds internal acceleration structure for given points list.
    void build(const ConstArrayAccessor1<Point2D>& points) override;

    //!
    //! Invokes the callback function for each nearby point around the origin
    //! within given radius.
    //!
    //! \param[in]  origin   The origin position.
    //! \param[in]  radius   The search radius.
    //! \param[in]  callback The callback function.
    //!
    void forEachNearbyPoint(const Point2D& origin,
                            double radius,
                            const ForEachNearbyPointFunc& callback) const override;

    //!
    //! Returns true if there are any nearby points for given origin within
    //! radius.
    //!
    //! \param[in]  origin The origin.
    //! \param[in]  radius The radius.
    //!
    //! \return     True if has nearby point, false otherwise.
    //!
    [[nodiscard]] bool hasNearbyPoint(const Point2D& origin, double radius) const override;

    //!
    //! \brief      Creates a new instance of the object with same properties
    //!             than original.
    //!
    //! \return     Copy of this object.
    //!
    [[nodiscard]] PointNeighborSearcher2Ptr clone() const override;

    //! Assignment operator.
    PointKdTreeSearcher2& operator=(const PointKdTreeSearcher2& other);

    //! Copy from the other instance.
    void set(const PointKdTreeSearcher2& other);

    //! Returns builder fox PointKdTreeSearcher2.
    static Builder builder();

private:
    KdTree<double, 2> _tree;
};

//! Shared pointer for the PointKdTreeSearcher2 type.
typedef std::shared_ptr<PointKdTreeSearcher2> PointKdTreeSearcher2Ptr;

//!
//! \brief Front-end to create PointKdTreeSearcher2 objects step by step.
//!
class PointKdTreeSearcher2::Builder final : public PointNeighborSearcherBuilder2 {
public:
    //! Builds PointKdTreeSearcher2 instance.
    [[nodiscard]] PointKdTreeSearcher2 build() const;

    //! Builds shared pointer of PointKdTreeSearcher2 instance.
    [[nodiscard]] PointKdTreeSearcher2Ptr makeShared() const;

    //! Returns shared pointer of PointNeighborSearcher3 type.
    [[nodiscard]] PointNeighborSearcher2Ptr buildPointNeighborSearcher() const override;
};

}  // namespace vox
