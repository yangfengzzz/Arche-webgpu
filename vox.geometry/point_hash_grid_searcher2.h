//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.geometry/point_neighbor_searcher2.h"
#include "vox.math/point2.h"
#include "vox.math/size2.h"

namespace vox {

//!
//! \brief Hash grid-based 2-D point searcher.
//!
//! This class implements 2-D point searcher by using hash grid for its internal
//! acceleration data structure. Each point is recorded to its corresponding
//! bucket where the hashing function is 2-D grid mapping.
//!
class PointHashGridSearcher2 final : public PointNeighborSearcher2 {
public:
    JET_NEIGHBOR_SEARCHER2_TYPE_NAME(PointHashGridSearcher2)

    class Builder;

    //!
    //! \brief      Constructs hash grid with given resolution and grid spacing.
    //!
    //! This constructor takes hash grid resolution and its grid spacing as
    //! its input parameters. The grid spacing must be 2x or greater than
    //! search radius.
    //!
    //! \param[in]  resolution  The resolution.
    //! \param[in]  gridSpacing The grid spacing.
    //!
    PointHashGridSearcher2(const Size2& resolution, double gridSpacing);

    //!
    //! \brief      Constructs hash grid with given resolution and grid spacing.
    //!
    //! This constructor takes hash grid resolution and its grid spacing as
    //! its input parameters. The grid spacing must be 2x or greater than
    //! search radius.
    //!
    //! \param[in]  resolutionX The resolution x.
    //! \param[in]  resolutionY The resolution y.
    //! \param[in]  gridSpacing The grid spacing.
    //!
    PointHashGridSearcher2(size_t resolutionX, size_t resolutionY, double gridSpacing);

    //! Copy constructor.
    PointHashGridSearcher2(const PointHashGridSearcher2& other);

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
    //! \brief      Adds a single point to the hash grid.
    //!
    //! This function adds a single point to the hash grid for future queries.
    //! It can be used for a hash grid that is already built by calling function
    //! PointHashGridSearcher2::build.
    //!
    //! \param[in]  point The point to be added.
    //!
    void add(const Point2D& point);

    //!
    //! \brief      Returns the internal bucket.
    //!
    //! A bucket is a list of point indices that has same hash value. This
    //! function returns the (immutable) internal bucket structure.
    //!
    //! \return     List of buckets.
    //!
    [[nodiscard]] const std::vector<std::vector<size_t>>& buckets() const;

    //!
    //! Returns the hash value for given 2-D bucket index.
    //!
    //! \param[in]  bucketIndex The bucket index.
    //!
    //! \return     The hash key from bucket index.
    //!
    [[nodiscard]] size_t getHashKeyFromBucketIndex(const Point2I& bucketIndex) const;

    //!
    //! Gets the bucket index from a point.
    //!
    //! \param[in]  position The position of the point.
    //!
    //! \return     The bucket index.
    //!
    [[nodiscard]] Point2I getBucketIndex(const Point2D& position) const;

    //!
    //! \brief      Creates a new instance of the object with same properties
    //!             than original.
    //!
    //! \return     Copy of this object.
    //!
    [[nodiscard]] PointNeighborSearcher2Ptr clone() const override;

    //! Assignment operator.
    PointHashGridSearcher2& operator=(const PointHashGridSearcher2& other);

    //! Copy from the other instance.
    void set(const PointHashGridSearcher2& other);

    //! Returns builder fox PointHashGridSearcher2.
    static Builder builder();

private:
    double _gridSpacing = 1.0;
    Point2I _resolution = Point2I(1, 1);
    std::vector<Point2D> _points;
    std::vector<std::vector<size_t>> _buckets;

    [[nodiscard]] size_t getHashKeyFromPosition(const Point2D& position) const;

    void getNearbyKeys(const Point2D& position, size_t* bucketIndices) const;
};

//! Shared pointer for the PointHashGridSearcher2 type.
typedef std::shared_ptr<PointHashGridSearcher2> PointHashGridSearcher2Ptr;

//!
//! \brief Front-end to create PointHashGridSearcher2 objects step by step.
//!
class PointHashGridSearcher2::Builder final : public PointNeighborSearcherBuilder2 {
public:
    //! Returns builder with resolution.
    Builder& withResolution(const Size2& resolution);

    //! Returns builder with grid spacing.
    Builder& withGridSpacing(double gridSpacing);

    //! Builds PointHashGridSearcher2 instance.
    [[nodiscard]] PointHashGridSearcher2 build() const;

    //! Builds shared pointer of PointHashGridSearcher2 instance.
    [[nodiscard]] PointHashGridSearcher2Ptr makeShared() const;

    //! Returns shared pointer of PointNeighborSearcher3 type.
    [[nodiscard]] PointNeighborSearcher2Ptr buildPointNeighborSearcher() const override;

private:
    Size2 _resolution{64, 64};
    double _gridSpacing = 1.0;
};

}  // namespace vox
