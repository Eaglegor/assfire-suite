#include "TspSolver.hpp"
#include "TspAlgorithm.hpp"

namespace assfire::tsp {

    TspSolver::TspSolver(const AlgorithmPtr &tsp_algorithm)
            : tsp_algorithm(tsp_algorithm)
    {}

    TspSolution TspSolver::solveTsp(const TspTask &task, const TspInterruptor& tsp_interruptor) const
    {
        return tsp_algorithm->solveTsp(task, tsp_interruptor);
    }

    void TspSolver::solveTsp(const TspTask &task, TspResultCallback resultCallback, const TspInterruptor& tsp_interruptor) const
    {
        tsp_algorithm->solveTsp(task, resultCallback, tsp_interruptor);
    }
}