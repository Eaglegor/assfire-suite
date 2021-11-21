#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspCostFunctionSettings.hpp"

namespace assfire::api::v1::tsp {
    class TspCostFunctionSettingsTranslator {
    public:
        using ProtoCostFunctionSettings = assfire::api::v1::tsp::TspCostFunctionSettings;
        using ProtoCostFunctionType = assfire::api::v1::tsp::TspCostFunctionType;
        using ProtoTotalDistanceCostFunctionSettings = assfire::api::v1::tsp::TotalDistanceTspCostFunctionSettings;

        using ApiCostFunctionSettings = assfire::tsp::TspCostFunctionSettings;
        using ApiCostFunctionType = assfire::tsp::TspCostFunctionType;
        using ApiTotalDistanceCostFunctionSettings = assfire::tsp::TotalDistanceTspCostFunctionSettings;

        static ApiCostFunctionSettings fromProto(const ProtoCostFunctionSettings &value);
        static ProtoCostFunctionSettings toProto(const ApiCostFunctionSettings &value);

        static ApiCostFunctionType fromProto(const ProtoCostFunctionType &value);
        static ProtoCostFunctionType toProto(const ApiCostFunctionType &value);

        static ApiTotalDistanceCostFunctionSettings fromProto(const ProtoTotalDistanceCostFunctionSettings &value);
        static ProtoTotalDistanceCostFunctionSettings toProto(const ApiTotalDistanceCostFunctionSettings &value);
    };
}

