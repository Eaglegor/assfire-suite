#include "TspSolutionListener.hpp"

namespace assfire::tsp {
    void TspSolutionListener::setOnNextSolutionCallback(const SolutionCallback &on_next_solution, bool use_for_final) {
        TspSolutionListener::on_next_solution = on_next_solution;
        if(use_for_final) setOnFinalSolutionCallback(on_next_solution);
    }

    void TspSolutionListener::setOnFinalSolutionCallback(const SolutionCallback &on_final_solution) {
        TspSolutionListener::on_final_solution = on_final_solution;
    }

    void TspSolutionListener::setOnErrorCallback(const ErrorCallback &on_error) {
        TspSolutionListener::on_error = on_error;
    }

    void TspSolutionListener::onNextSolution(const TspSolution &solution) {
        if (on_next_solution) {
            on_next_solution(solution);
        }
    }

    void TspSolutionListener::onFinalSolution(const TspSolution &solution) {
        if (on_final_solution) {
            on_final_solution(solution);
        }
    }

    void TspSolutionListener::onError() {
        if (on_error) {
            on_error();
        }
    }
}