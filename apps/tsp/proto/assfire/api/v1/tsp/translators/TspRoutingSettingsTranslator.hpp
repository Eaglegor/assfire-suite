#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspRoutingSettings.hpp"

namespace assfire::api::v1::tsp {
    class TspRoutingSettingsTranslator {
    public:
        using ProtoRoutingSettings = assfire::api::v1::tsp::TspRoutingSettings;

        using ApiRoutingSettings = assfire::tsp::TspRoutingSettings;

        static ApiRoutingSettings fromProto(const ProtoRoutingSettings &value);
        static ProtoRoutingSettings toProto(const ApiRoutingSettings &value);
    };
}

