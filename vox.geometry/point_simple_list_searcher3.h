//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.geometry/point_neighbor_searcher3.h"

namespace vox {

//!
//! \brief Simple ad-hoc 3-D point searcher.
//!
//! This class implements 3-D point searcher simply by looking up every point in
//! the list. Thus, this class is not ideal for searches involving large number
//! of points, but only for small set of items.
//!
class PointSimpleListSearcher3 final : public PointNeighborSearcher3 {
public:
    JET_NEIGHBOR_SEARCHER3_TYPE_NAME(PointSimpleListSearcher3)

    class Builder;

    //! Default constructor.
    PointSimpleListSearcher3();

    //! Copy constructor.
    PointSimpleListSearcher3(const PointSimpleListSearcher3& other);

    //!
    //! \brief      Builds internal structure for given points list.
    //!
    //! For this class, this function simply copies the given point list to the
    //! internal list.
    //!
    //! \param[in]  points The points to search.
    //!
    void build(const ConstArrayAccessor1<Point3D>& points) override;

    //!
    //! Invokes the callback function for each nearby point around the origin
    //! within given radius.
    //!
    //! \param[in]  origin   The origin position.
    //! \param[in]  radius   The search radius.
    //! \param[in]  callback The callback function.
    //!
    void forEachNearbyPoint(const Point3D& origin,
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
    [[nodiscard]] bool hasNearbyPoint(const Point3D& origin, double radius) const override;

    //!
    //! \brief      Creates a new instance of the object with same properties
    //!             than original.
    //!
    //! \return     Copy of this object.
    //!
    [[nodiscard]] PointNeighborSearcher3Ptr clone() const override;

    //! Assignment operator.
    PointSimpleListSearcher3& operator=(const PointSimpleListSearcher3& other);

    //! Copy from the other instance.
    void set(const PointSimpleListSearcher3& other);

    //! Returns builder fox PointSimpleListSearcher3.
    static Builder builder();

private:
    std::vector<Point3D> _points;
};

//! Shared pointer for the PointSimpleListSearcher3 type.
typedef std::shared_ptr<PointSimpleListSearcher3> PointSimpleListSearcher3Ptr;

//!
//! \brief Front-end to create PointSimpleListSearcher3 objects step by step.
//!
class PointSimpleListSearcher3::Builder final : public PointNeighborSearcherBuilder3 {
public:
    //! Builds PointSimpleListSearcher3 instance.
    [[nodiscard]] PointSimpleListSearcher3 build() const;

    //! Builds shared pointer of PointSimpleListSearcher3 instance.
    [[nodiscard]] PointSimpleListSearcher3Ptr makeShared() const;

    //! Returns shared pointer of PointNeighborSearcher3 type.
    [[nodiscard]] PointNeighborSearcher3Ptr buildPointNeighborSearcher() const override;
};

}  // namespace vox
