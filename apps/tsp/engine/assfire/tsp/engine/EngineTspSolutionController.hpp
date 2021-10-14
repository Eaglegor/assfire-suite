#pragma once

#include "assfire/tsp/api/TspSolutionController.hpp"
#include "TspAlgorithmStateContainer.hpp"
#include <atomic>
#include <mutex>
#include <future>

namespace assfire::tsp {
    class TspAlgorithm;

    class EngineTspSolutionController : public TspSolutionController {
    public:
        using AlgorithmPtr = std::shared_ptr<TspAlgorithm>;

        explicit EngineTspSolutionController(const TspTask& task, AlgorithmPtr algorithm);

        EngineTspSolutionController(const TspTask& task, AlgorithmPtr algorithm, TspAlgorithmStateContainer state_container);

        void start();

        void interrupt() override;

        void pause() override;

        void resume() override;

        std::optional<TspSolution> getCurrentSolution() override;

        void setSolutionListener(std::function<void(const TspSolution &)> listener) override;

        bool isFinished() override;

        TspAlgorithmStateContainer& getStateContainer();
        void publishSolution(const TspSolution& solution);
        bool isInterrupted() const;
        bool isPaused() const;

        virtual ~EngineTspSolutionController();

    private:
        void launchTask();
        void waitForTaskStop();

        TspTask task;
        TspAlgorithmStateContainer state_container;
        AlgorithmPtr algorithm;
        std::atomic_bool is_started = false;
        std::atomic_bool is_paused = false;
        std::atomic_bool is_interrupted = false;
        std::atomic_bool is_finished = false;
        std::future<void> control_state;
        std::optional<TspSolution> solution;
        std::function<void(const TspSolution&)> solution_listener;
        std::mutex solution_guard;
    };
}