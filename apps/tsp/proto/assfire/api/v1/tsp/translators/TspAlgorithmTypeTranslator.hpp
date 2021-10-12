#pragma once

#include <assfire/api/v1/tsp/concepts.pb.h>
#include <assfire/tsp/api/TspAlgorithmType.hpp>

namespace assfire::api::v1::tsp {

    class TspAlgorithmTypeTranslator {
    public:

        using ProtoAlgorithmType = assfire::api::v1::tsp::TspAlgorithmType;
        using ApiAlgorithmType = assfire::tsp::TspAlgorithmType;

        static ApiAlgorithmType fromProto(const ProtoAlgorithmType& type);

        static ProtoAlgorithmType toProto(const ApiAlgorithmType& type);
    };

}

