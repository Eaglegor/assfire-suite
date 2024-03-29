#pragma once

#include "assfire/tsp/api/TspSolverApi.hpp"
#include <memory>
#include <vector>
#include <functional>
#include "TspAlgorithm.hpp"

namespace assfire::router {
    class RouterApi;
}

namespace assfire::tsp
{
    class TspTask;


    class TspSolverEngine : public TspSolverApi
    {
    public:
        TspSolverEngine(std::shared_ptr<router::RouterApi> router);

        TspSolutionSession solveTsp(const TspTask &task, TspSolutionListener solution_listener) const override;
        TspSolutionSession solveTsp(const TspTask &task, TspAlgorithmStateContainer saved_state, TspSolutionListener solution_listener) const;

        TspSolutionSession solveTsp(const std::string &session_id, const TspTask &task, TspSolutionListener solution_listener) const;
        TspSolutionSession solveTsp(const std::string &session_id, const TspTask &task, TspAlgorithmStateContainer saved_state, TspSolutionListener solution_listener) const;

    private:
        std::shared_ptr<router::RouterApi> router;
    };
}

