#include "TspAlgorithmTypeTranslator.hpp"

using namespace assfire::api::v1::tsp;

namespace assfire::api::v1::tsp {

    TspAlgorithmTypeTranslator::ApiAlgorithmType TspAlgorithmTypeTranslator::fromProto(const ProtoAlgorithmType &type) {
        switch (type) {
            case ProtoAlgorithmType::TSP_ALGORITHM_TYPE_UNSPECIFIED:
            case ProtoAlgorithmType::TSP_ALGORITHM_TYPE_AUTO:
                return ApiAlgorithmType::AUTO;
            case ProtoAlgorithmType::TSP_ALGORITHM_TYPE_TWO_OPT:
                return ApiAlgorithmType::TWO_OPT;
            case ProtoAlgorithmType::TSP_ALGORITHM_TYPE_TIME_WASTING:
                return ApiAlgorithmType::TIME_WASTING;
            default:
                throw std::invalid_argument("Unknown or unsupported tsp algorithm type");
        }
    }

    TspAlgorithmTypeTranslator::ProtoAlgorithmType TspAlgorithmTypeTranslator::toProto(const ApiAlgorithmType &type) {
        switch (type) {
            case ApiAlgorithmType::AUTO:
                return ProtoAlgorithmType::TSP_ALGORITHM_TYPE_AUTO;
            case ApiAlgorithmType::TWO_OPT:
                return ProtoAlgorithmType::TSP_ALGORITHM_TYPE_TWO_OPT;
            case ApiAlgorithmType::TIME_WASTING:
                return ProtoAlgorithmType::TSP_ALGORITHM_TYPE_TIME_WASTING;
            default:
                throw std::invalid_argument("Unknown or unsupported tsp algorithm type");
        }
    }
}