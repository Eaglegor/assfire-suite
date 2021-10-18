#include "TspSolverEngine.hpp"

#include <utility>
#include "TspAlgorithm.hpp"
#include <spdlog/spdlog.h>

#include "assfire/router/api/DistanceMatrix.hpp"
#include "assfire/router/api/RouterApi.hpp"

#include "assfire/tsp/api/TspTask.hpp"
#include "assfire/tsp/engine/TspEstimator.hpp"
#include "assfire/tsp/engine/TspCostFunction.hpp"
#include "assfire/tsp/engine/TspValidator.hpp"
#include "EngineTspSolutionController.hpp"
#include "NopTspValidator.hpp"
#include "TotalDistanceTspCostFunction.hpp"

#include "algorithms/TwoOptTspAlgorithm.hpp"

namespace assfire::tsp {

    TspSolverEngine::TspSolverEngine(std::shared_ptr<router::RouterApi> router) : router(std::move(router)) {}

    namespace {
        EngineTspSolutionController::SessionId generateSessionId() {
            return "<todo>";
        }

        EngineTspSolutionController::AlgorithmPtr createAlgorithm(const TspTask &task, TspEstimator estimator, const EngineTspSolutionController::SessionId &session_id) {
            TspAlgorithmType type = task.getSolverSettings().getAlgorithmSettings().getAlgorithmType();
            SPDLOG_INFO("Creating algorithm of type {} for TSP session {}", std::to_string(type), session_id);
            switch (type) {
                case TspAlgorithmType::AUTO:
                case TspAlgorithmType::TWO_OPT:
                    return std::make_shared<TwoOptTspAlgorithm>(std::move(estimator));
                default:
                    throw std::invalid_argument("Unsupported tsp solver type: " + std::to_string(type));
            }
        }

        router::DistanceMatrix createDistanceMatrix(const router::RouterApi &router, const TspRoutingSettings &settings, const EngineTspSolutionController::SessionId &session_id) {
            SPDLOG_INFO("Building distance matrix for TSP session {}", session_id);
            return router.createDistanceMatrix(
                    settings.getEngineType(),
                    router::DistanceMatrixCachingPolicy::FULL_MATRIX_PRECACHING,
                    settings.getRoutingProfile(),
                    settings.getRouteProviderSettings()
            );
        }

        std::vector<router::IndexedLocation> indexLocations(const TspTask &task, router::DistanceMatrix &distance_matrix) {
            std::vector<router::IndexedLocation> indexed_locations;
            indexed_locations.reserve(task.getPoints().size());
            for (const TspPoint &pt : task.getPoints()) {
                indexed_locations.push_back(distance_matrix.addLocation(pt.getLocation()));
            }
            return indexed_locations;
        }

        TspEstimator::CostFunctionPtr createCostFunction(const TspTask &task, router::DistanceMatrix distance_matrix, const EngineTspSolutionController::SessionId &session_id) {
            TspCostFunctionType type = task.getSolverSettings().getEstimatorSettings().getCostFunctionSettings().getType();
            SPDLOG_INFO("Creating cost function of type {} for TSP session {}", std::to_string(type), session_id);
            switch (type) {
                case TspCostFunctionType::DEFAULT:
                case TspCostFunctionType::TOTAL_DISTANCE:
                    return std::make_shared<TotalDistanceTspCostFunction>(std::move(distance_matrix), indexLocations(task, distance_matrix));
                default:
                    throw std::invalid_argument("Unsupported tsp cost function type: " + std::to_string(type));
            }
        }

        TspEstimator::ValidatorPtr createValidator(const TspTask &task, router::DistanceMatrix distance_matrix, const EngineTspSolutionController::SessionId &session_id) {
            TspValidatorType type = task.getSolverSettings().getEstimatorSettings().getValidatorSettings().getType();
            SPDLOG_INFO("Creating validator of type {} for TSP session {}", std::to_string(type), session_id);
            switch (type) {
                case TspValidatorType::FAIL_ON_ANY_VIOLATION:
                case TspValidatorType::DEFAULT:
                    return std::make_shared<NopTspValidator>();
                default:
                    throw std::invalid_argument("Unsupported tsp validator type: " + std::to_string(type));
            }
        }
    }

    TspSolutionSession TspSolverEngine::solveTsp(const TspTask &task) const {
        return solveTsp(task, TspAlgorithmStateContainer());
    }

    TspSolutionSession TspSolverEngine::solveTsp(const TspTask &task, TspAlgorithmStateContainer saved_state) const {
        EngineTspSolutionController::SessionId session_id = generateSessionId();
        SPDLOG_INFO("Starting TSP session {}", session_id);
        router::DistanceMatrix distance_matrix = createDistanceMatrix(*router, task.getSolverSettings().getRoutingSettings(), session_id);
        TspEstimator::CostFunctionPtr cost_function = createCostFunction(task, distance_matrix, session_id);
        TspEstimator::ValidatorPtr validator = createValidator(task, distance_matrix, session_id);
        TspEstimator estimator(validator, cost_function);
        EngineTspSolutionController::AlgorithmPtr algorithm = createAlgorithm(task, estimator, session_id);

        std::unique_ptr<EngineTspSolutionController> solution_controller = std::make_unique<EngineTspSolutionController>(session_id, task, algorithm, std::move(saved_state));
        solution_controller->start();
        return TspSolutionSession(std::move(solution_controller));
    }
}