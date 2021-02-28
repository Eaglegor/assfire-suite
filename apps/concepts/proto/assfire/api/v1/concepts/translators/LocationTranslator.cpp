#include "LocationTranslator.hpp"

using namespace assfire::api::v1::concepts;

LocationTranslator::ApiLocation LocationTranslator::fromProto(const ProtoLocation &location)
{
    return ApiLocation::fromEncodedLatLon(location.encoded_latitude(), location.encoded_latitude());
}

LocationTranslator::ProtoLocation LocationTranslator::toProto(const ApiLocation &location)
{
    ProtoLocation result;
    result.set_encoded_latitude(location.getLatitude().encodedValue());
    result.set_encoded_longitude(location.getLongitude().encodedValue());
    return result;
}
