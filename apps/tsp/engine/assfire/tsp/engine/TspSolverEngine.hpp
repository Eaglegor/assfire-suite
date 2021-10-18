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

        TspSolutionSession solveTsp(const TspTask &task) const override;
        TspSolutionSession solveTsp(const TspTask &task, TspAlgorithmStateContainer saved_state) const;

    private:
        std::shared_ptr<router::RouterApi> router;
    };
}

