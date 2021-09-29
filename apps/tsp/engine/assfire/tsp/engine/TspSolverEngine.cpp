#include "TspSolverEngine.hpp"
#include "TspAlgorithm.hpp"

namespace assfire::tsp {

    TspSolverEngine::TspSolverEngine(const AlgorithmPtr &tsp_algorithm)
            : tsp_algorithm(tsp_algorithm)
    {}

    TspSolution TspSolverEngine::solveTsp(const TspTask &task, const TspInterruptor& tsp_interruptor) const
    {
        return tsp_algorithm->solveTsp(task, tsp_interruptor);
    }

    void TspSolverEngine::solveTsp(const TspTask &task, TspResultCallback resultCallback, const TspInterruptor& tsp_interruptor) const
    {
        tsp_algorithm->solveTsp(task, resultCallback, tsp_interruptor);
    }
}