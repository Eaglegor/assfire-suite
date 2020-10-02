#pragma once

#include <string>
#include "MetricsCollector.hpp"

namespace assfire {
    class Greeter
    {
    public:
        explicit Greeter(const MetricsCollector &metrics_collector);

        std::string greet(const std::string &name) const;

    private:
        mutable MetricsCollector metrics_collector;
    };
}