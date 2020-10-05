#pragma once

#include <assfire/api/v1/model/optimization/transport/tsp.pb.h>

namespace assfire::tsp
{
    class TspSolver
    {
    public:
        using Task = assfire::api::v1::model::optimization::transport::TspTask;
        using Solution = assfire::api::v1::model::optimization::transport::TspSolution;

        Solution solveTsp(const Task& task);

        virtual ~TspSolver() = default;
    };
}