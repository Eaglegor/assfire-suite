#pragma once

#include <functional>
#include <assfire/tsp/api/TspSolution.hpp>

namespace assfire::tsp {
    class TspSolutionListener {
    public:
        using SolutionCallback = std::function<void(const TspSolution &solution)>;
        using ErrorCallback = std::function<void()>;

        void setOnNextSolutionCallback(const SolutionCallback &on_next_solution, bool use_for_final = true);

        void setOnFinalSolutionCallback(const SolutionCallback &on_final_solution);

        void setOnErrorCallback(const ErrorCallback &on_error);

        void onNextSolution(const TspSolution &solution);

        void onFinalSolution(const TspSolution &solution);

        void onError();

    private:
        SolutionCallback on_next_solution;
        SolutionCallback on_final_solution;
        ErrorCallback on_error;
    };
}