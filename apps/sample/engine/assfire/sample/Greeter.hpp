#pragma once

#include <string>
#include "MetricsCollector.hpp"

namespace assfire::sample {
    class Greeter
    {
    public:
        explicit Greeter(const MetricsCollector &metrics_collector);

        std::string greet(const std::string &name) const;

    private:
        mutable MetricsCollector metrics_collector;
    };
}