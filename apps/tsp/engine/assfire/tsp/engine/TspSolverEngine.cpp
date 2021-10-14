#include "TspSolverEngine.hpp"
#include "TspAlgorithm.hpp"

#include "EngineTspSolutionController.hpp"

namespace assfire::tsp {

    TspSolverEngine::TspSolverEngine(const AlgorithmPtr &tsp_algorithm)
            : tsp_algorithm(tsp_algorithm) {}

    TspSolutionSession TspSolverEngine::solveTsp(const TspTask &task) const {
        return solveTsp(task, TspAlgorithmStateContainer());
    }

    TspSolutionSession TspSolverEngine::solveTsp(const TspTask &task, TspAlgorithmStateContainer saved_state) const {
        std::unique_ptr<EngineTspSolutionController> solution_controller = std::make_unique<EngineTspSolutionController>(task, tsp_algorithm, std::move(saved_state));
        solution_controller->start();
        return TspSolutionSession(std::move(solution_controller));
    }
}