#include "TspSolverSettingsTranslator.hpp"
#include "TspRoutingSettingsTranslator.hpp"
#include "TspEstimatorSettingsTranslator.hpp"
#include "TspAlgorithmSettingsTranslator.hpp"

namespace assfire::api::v1::tsp {
    TspSolverSettingsTranslator::ApiSolverSettings TspSolverSettingsTranslator::fromProto(const TspSolverSettingsTranslator::ProtoSolverSettings &value) {
        return ApiSolverSettings(
                TspRoutingSettingsTranslator::fromProto(value.routing_settings()),
                TspEstimatorSettingsTranslator::fromProto(value.estimator_settings()),
                TspAlgorithmSettingsTranslator::fromProto(value.algorithm_settings())
        );
    }

    TspSolverSettingsTranslator::ProtoSolverSettings TspSolverSettingsTranslator::toProto(const TspSolverSettingsTranslator::ApiSolverSettings &value) {
        ProtoSolverSettings result;
        result.mutable_routing_settings()->CopyFrom(TspRoutingSettingsTranslator::toProto(value.getRoutingSettings()));
        result.mutable_estimator_settings()->CopyFrom(TspEstimatorSettingsTranslator::toProto(value.getEstimatorSettings()));
        result.mutable_algorithm_settings()->CopyFrom(TspAlgorithmSettingsTranslator::toProto(value.getAlgorithmSettings()));
        return result;
    }
}