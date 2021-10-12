#pragma once

#include <string>

namespace assfire::tsp {
    class TaskIdGenerator {
    public:
        virtual std::string newId() = 0;
    };
}