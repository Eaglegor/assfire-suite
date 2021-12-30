#include <gtest/gtest.h>
#include <vector>
#include <chrono>
#include <thread>
#include "assfire/tsp/engine/TspAlgorithm.hpp"
#include <random>
#include <spdlog/spdlog.h>
#include "assfire/tsp/engine/EngineTspSolutionController.hpp"
#include <thread>

namespace assfire::tsp {
    using namespace std::chrono_literals;

    namespace {
        class EngineControllerTestAlgorithm : public TspAlgorithm {
        public:
            void solveTsp(const TspTask &task, EngineTspSolutionController &solution_controller) const override {
                int initial_i = solution_controller.getStateContainer().getDto().has_two_opt_algorithm_state() ?
                                solution_controller.getStateContainer().getDto().two_opt_algorithm_state().i() : 0;
                SPDLOG_INFO("Starting solution with initial i = {}", initial_i);
                for (i = initial_i; i < 1000 && !solution_controller.isInterrupted(); ++i) {
                    SPDLOG_INFO("Starting iteration with i = {}", i);
                    std::this_thread::sleep_for(50ms);
                    vec.push_back(i);
                    SPDLOG_INFO("Finished iteration with i = {}", i);
                    if (solution_controller.isInterrupted()) {
                        SPDLOG_INFO("Pausing solution, current i = {}, saving next state: {}", i, i + 1);
                        solution_controller.getStateContainer().getDto().mutable_two_opt_algorithm_state()->set_i(i + 1);
                    }
                }
            }

            int getI() const {
                return i;
            }

            const std::vector<int> &getVec() const {
                return vec;
            }

        private:
            mutable std::vector<int> vec;
            mutable std::atomic_int i;
        };
    }

    TEST(EngineTspSolutionControllerTest, InterruptResumeWorkflowIsThreadSafe) {
        std::shared_ptr<EngineControllerTestAlgorithm> algorithm = std::make_shared<EngineControllerTestAlgorithm>();
        EngineTspSolutionController controller("test", TspTask(TspTask::Points(), TspSolverSettings()), algorithm, TspAlgorithmStateContainer(), TspSolutionListener());
        controller.start();
        while (algorithm->getI() < 1000) {
            int randomAction = rand() % 4;
            switch (randomAction) {
                case 0:
                    SPDLOG_INFO("PAUSING SOLUTION");
                    controller.pause();
                    break;
                case 1:
                    SPDLOG_INFO("RESUMING SOLUTION");
                    controller.resume();
                    break;
            }
            std::this_thread::sleep_for(30ms);
        }

        const std::vector<int> &vec = algorithm->getVec();

        ASSERT_EQ(1000, vec.size());
        for (int i = 0; i < 1000; ++i) {
            ASSERT_EQ(i, vec[i]);
        }
    }
}