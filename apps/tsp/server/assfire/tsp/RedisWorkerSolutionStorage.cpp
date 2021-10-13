#include "RedisWorkerSolutionStorage.hpp"
#include <thread>
#include <atomic>
#include <future>

namespace assfire::tsp {
    using namespace std::chrono_literals;

    class RedisSolutionUpdatePublisher : public WorkerSolutionStorage::SolutionUpdatePublisher {
    public:
        RedisSolutionUpdatePublisher(const std::string &task_id, const RedisWorkerSolutionStorage &redis_storage) :
                task_id(task_id),
                redis_storage(redis_storage),
                interrupted(false),
                stopped(false) {}

        void listen(std::function<void(const WorkerSolutionStorage::Solution &)> onSolution) override {
            stopped = false;
            std::async(std::launch::async, [&] {
                std::optional<WorkerSolutionStorage::Solution> current_solution = redis_storage.fetchSolution(task_id);
                while (!interrupted && (!current_solution || !current_solution->is_final())) {
                    if (current_solution) onSolution(*current_solution);
                    std::this_thread::sleep_for(5000ms);
                }
                if (current_solution) onSolution(*current_solution);
                stopped = true;
            });
        }

        virtual ~RedisSolutionUpdatePublisher() {
            interrupted = true;
            while (!stopped) {
                std::this_thread::yield();
            }
        }

    private:
        std::string task_id;
        const RedisWorkerSolutionStorage &redis_storage;
        std::atomic_bool interrupted;
        std::atomic_bool stopped;
    };

    std::optional<WorkerSolutionStorage::Solution> RedisWorkerSolutionStorage::fetchSolution(const std::string &task_id) const {
        return std::nullopt;
    }

    std::unique_ptr<WorkerSolutionStorage::SolutionUpdatePublisher> RedisWorkerSolutionStorage::subscribeForSolutionUpdate(const std::string &task_id) {
        return std::make_unique<RedisSolutionUpdatePublisher>(task_id, *this);
    }
}
