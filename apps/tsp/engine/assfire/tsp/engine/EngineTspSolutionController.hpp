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
        using TspSolutionListener = std::function<void(const TspSolution &)>;
        using AlgorithmPtr = std::shared_ptr<TspAlgorithm>;

        EngineTspSolutionController(const SessionId &session_id, const TspTask &task, AlgorithmPtr algorithm, TspAlgorithmStateContainer state_container,
                                    TspSolutionListener solution_listener);

        void start();

        void interrupt() override;

        void pause() override;

        void resume() override;

        bool isFinished() override;

        void waitFor(long milliseconds) override;

        TspAlgorithmStateContainer &getStateContainer();

        void publishSolution(const TspSolution &solution);

        bool isInterrupted() const;

        const SessionId &getSessionId() const override;

        virtual ~EngineTspSolutionController();

    private:
        void launchTask();

        void waitForTaskStop();

        TspTask task;
        TspAlgorithmStateContainer state_container;
        AlgorithmPtr algorithm;
        std::atomic_bool is_started = false;
        std::atomic_bool is_interrupted = true;
        std::atomic_bool is_finished = false;
        std::future<void> control_state;
        TspSolutionListener solution_listener;
        SessionId session_id;
    };
}