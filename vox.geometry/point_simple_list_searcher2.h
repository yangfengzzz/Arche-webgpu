//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.geometry/point_neighbor_searcher2.h"

namespace vox {

//!
//! \brief Simple ad-hoc 2-D point searcher.
//!
//! This class implements 2-D point searcher simply by looking up every point in
//! the list. Thus, this class is not ideal for searches involving large number
//! of points, but only for small set of items.
//!
class PointSimpleListSearcher2 final : public PointNeighborSearcher2 {
public:
    JET_NEIGHBOR_SEARCHER2_TYPE_NAME(PointSimpleListSearcher2)

    class Builder;

    //! Default constructor.
    PointSimpleListSearcher2();

    //! Copy constructor.
    PointSimpleListSearcher2(const PointSimpleListSearcher2& other);

    //!
    //! \brief      Builds internal structure for given points list.
    //!
    //! For this class, this function simply copies the given point list to the
    //! internal list.
    //!
    //! \param[in]  points The points to search.
    //!
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
    PointSimpleListSearcher2& operator=(const PointSimpleListSearcher2& other);

    //! Copy from the other instance.
    void set(const PointSimpleListSearcher2& other);

    //! Returns builder fox PointSimpleListSearcher2.
    static Builder builder();

private:
    std::vector<Point2D> _points;
};

//! Shared pointer for the PointSimpleListSearcher2 type.
typedef std::shared_ptr<PointSimpleListSearcher2> PointSimpleListSearcher2Ptr;

//!
//! \brief Front-end to create PointSimpleListSearcher2 objects step by step.
//!
class PointSimpleListSearcher2::Builder final : public PointNeighborSearcherBuilder2 {
public:
    //! Builds PointSimpleListSearcher2 instance.
    [[nodiscard]] PointSimpleListSearcher2 build() const;

    //! Builds shared pointer of PointSimpleListSearcher2 instance.
    [[nodiscard]] PointSimpleListSearcher2Ptr makeShared() const;

    //! Returns shared pointer of PointNeighborSearcher3 type.
    [[nodiscard]] PointNeighborSearcher2Ptr buildPointNeighborSearcher() const override;
};

}  // namespace vox
