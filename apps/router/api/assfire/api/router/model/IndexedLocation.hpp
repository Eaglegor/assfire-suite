#pragma once

#include "Location.hpp"

namespace assfire::router
{
    class IndexedLocation
    {
    public:
        IndexedLocation(int id, int index_tag, const Location &location)
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

        int getId() const
        {
            return id;
        }

        int getIndexTag() const
        {
            return index_tag;
        }

        const Location& getLocation() const {
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
        int id;
        int index_tag;
        Location location;
    };
}