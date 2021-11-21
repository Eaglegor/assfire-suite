#include "TspTaskTranslator.hpp"
#include "TspPointTranslator.hpp"
#include "TspSolverSettingsTranslator.hpp"

namespace assfire::api::v1::tsp {
    TspTaskTranslator::ApiTask TspTaskTranslator::fromProto(const TspTaskTranslator::ProtoTask &value) {
        ApiTask::Points points;
        points.reserve(value.points_size());
        for(const auto& pt : value.points()) {
            points.push_back(TspPointTranslator::fromProto(pt));
        }
        return ApiTask(points, TspSolverSettingsTranslator::fromProto(value.solver_settings()));
    }

    TspTaskTranslator::ProtoTask TspTaskTranslator::toProto(const TspTaskTranslator::ApiTask &value) {
        ProtoTask result;
        for(const auto& pt : value.getPoints()) {
            result.add_points()->CopyFrom(TspPointTranslator::toProto(pt));
        }
        result.mutable_solver_settings()->CopyFrom(TspSolverSettingsTranslator::toProto(value.getSolverSettings()));
        return result;
    }
}