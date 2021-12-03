#pragma once

#include "assfire/locations/api/Location.hpp"

namespace assfire::router
{
    class IndexedLocation
    {
    public:
        using Location = locations::Location;
        using Coordinate = locations::Coordinate;
        using Id = int;
        using IndexTag = int;
        static constexpr IndexTag INVALID_TAG = -1;

        IndexedLocation(Id id, IndexTag index_tag, const Location &location)
                :
                id(id),
                index_tag(index_tag),
                location(location)
        {}

        IndexedLocation(const IndexedLocation &rhs) = default;

        bool operator==(const IndexedLocation &rhs) const {
            return id == rhs.id &&
                   index_tag == rhs.index_tag &&
                   location == rhs.location;
        }

        bool operator!=(const IndexedLocation &rhs) const {
            return !(rhs == *this);
        }

        IndexedLocation& operator=(const IndexedLocation& rhs) = default;

        Id getId() const
        {
            return id;
        }

        IndexTag getIndexTag() const
        {
            return index_tag;
        }

        const Location& getRawLocation() const {
            return location;
        }

        const Coordinate &getLatitude() const
        {
            return location.getLatitude();
        }

        const Coordinate &getLongitude() const
        {
            return location.getLongitude();
        }

    private:
        Id id;
        IndexTag index_tag;
        Location location;
    };
}