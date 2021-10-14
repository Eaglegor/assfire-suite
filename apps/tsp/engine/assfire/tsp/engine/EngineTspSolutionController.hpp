#pragma once

#include "assfire/tsp/api/TspSolutionController.hpp"
#include "TspAlgorithmStateContainer.hpp"

namespace assfire::tsp {
    class TspAlgorithm;

    class EngineTspSolutionController : public TspSolutionController {
    public:
        using AlgorithmPtr = std::shared_ptr<TspAlgorithm>;

        explicit EngineTspSolutionController(AlgorithmPtr algorithm);

        EngineTspSolutionController(AlgorithmPtr algorithm, TspAlgorithmStateContainer state_container);

        void start();

        void interrupt() override;

        void pause() override;

        void resume() override;

        std::future<TspSolution> getCurrentSolution() override;

        bool isFinished() override;

        TspAlgorithmStateContainer& getStateContainer();
        void publishSolution(const TspSolution& solution);
        bool isInterrupted() const;
        bool isPaused() const;

    private:
        TspAlgorithmStateContainer state_container;
        AlgorithmPtr algorithm;
    };
}