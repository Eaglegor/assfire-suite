#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include <optional>
#include <functional>
#include <memory>

namespace assfire::tsp {
    class SolutionStorage {
    public:
        using Solution = assfire::api::v1::tsp::TspSolution;

        virtual std::optional<Solution> fetchSolution(const std::string& task_id) const = 0;
    };
}