#include "TaskIdGenerator.hpp"

#include <atomic>

namespace assfire::tsp {
    class IncrementalTaskIdGenerator : public TaskIdGenerator {
    public:
        IncrementalTaskIdGenerator() : counter(0) {}

        std::string newId() override {
            return std::to_string(++counter);
        }

    private:
        std::atomic_int64_t counter;
    };
}