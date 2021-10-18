#include "TspSolverEngine.hpp"

#include <utility>
#include "TspAlgorithm.hpp"
#include <spdlog/spdlog.h>

#include "EngineTspSolutionController.hpp"

namespace assfire::tsp {

    namespace {
        EngineTspSolutionController::SessionId generateSessionId() {
            return "<todo>";
        }
    }

    TspSolverEngine::TspSolverEngine(AlgorithmPtr tsp_algorithm)
            : tsp_algorithm(std::move(tsp_algorithm)) {}

    TspSolutionSession TspSolverEngine::solveTsp(const TspTask &task) const {
        return solveTsp(task, TspAlgorithmStateContainer());
    }

    TspSolutionSession TspSolverEngine::solveTsp(const TspTask &task, TspAlgorithmStateContainer saved_state) const {
        EngineTspSolutionController::SessionId session_id = generateSessionId();
        SPDLOG_INFO("Starting TSP session {}", session_id);
        std::unique_ptr<EngineTspSolutionController> solution_controller = std::make_unique<EngineTspSolutionController>(session_id, task, tsp_algorithm, std::move(saved_state));
        solution_controller->start();
        return TspSolutionSession(std::move(solution_controller));
    }
}