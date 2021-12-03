#include "LocationTranslator.hpp"

namespace assfire::api::v1::locations
{
    LocationTranslator::ApiLocation LocationTranslator::fromProto(const ProtoLocation &location) {
        return ApiLocation::fromEncodedLatLon(location.encoded_latitude(), location.encoded_longitude());
    }

    LocationTranslator::ProtoLocation LocationTranslator::toProto(const ApiLocation &location) {
        ProtoLocation result;
        result.set_encoded_latitude(location.getLatitude().encodedValue());
        result.set_encoded_longitude(location.getLongitude().encodedValue());
        return result;
    }
}