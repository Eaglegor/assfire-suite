#pragma once

#include <assfire/api/v1/service/tsp/tsp.pb.h>

namespace assfire::tsp
{
    class TspAlgorithm
    {
    public:
        using Task = assfire::api::v1::service::tsp::TspTask;
        using Solution = assfire::api::v1::service::tsp::TspSolution;
        using Settings = assfire::api::v1::service::tsp::Settings;
        using TspSolutionStatus = assfire::api::v1::service::tsp::TspSolutionStatus;

        using StatusConsumer = std::function<void(const TspSolutionStatus &status)>;
        using InterruptCondition = std::function<bool ()>;

        virtual Solution solveTsp(const Task &, const Settings &, const StatusConsumer &, const InterruptCondition&) = 0;

        virtual ~TspAlgorithm() = default;
    };
}