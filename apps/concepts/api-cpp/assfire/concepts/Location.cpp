#include "Location.hpp"

using namespace assfire;

Location::Location(const Coordinate &latitude, const Coordinate &longitude)
        :
        latitude(latitude),
        longitude(longitude)
{}

const Coordinate &Location::getLatitude() const
{
    return latitude;
}

const Coordinate &Location::getLongitude() const
{
    return longitude;
}

bool Location::operator==(const Location &rhs) const
{
    return latitude == rhs.latitude &&
           longitude == rhs.longitude;
}

bool Location::operator!=(const Location &rhs) const
{
    return !(rhs == *this);
}

Location Location::fromDoubleLatLon(double lat, double lon)
{
    return Location(Coordinate::fromDoubleValue(lat), Coordinate::fromDoubleValue(lon));
}

Location Location::fromEncodedLatLon(Coordinate::encoded_type lat, Coordinate::encoded_type lon)
{
    return Location(Coordinate::fromEncodedValue(lat), Coordinate::fromEncodedValue(lon));
}
