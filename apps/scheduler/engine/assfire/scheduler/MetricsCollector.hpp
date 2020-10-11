#pragma once

#include <memory>
#include <prometheus/family.h>
#include <assfire/metrics/stopwatch.hpp>

namespace prometheus {
    class Exposer;

    class Registry;

    class Counter;

    class Summary;
}

namespace assfire::scheduler::transport {
    class MetricsCollector
    {
    public:
        explicit MetricsCollector(const std::shared_ptr<prometheus::Exposer>& exposer = nullptr);
        MetricsCollector(const MetricsCollector& rhs) = default;
        ~MetricsCollector();

    private:
        std::shared_ptr<prometheus::Exposer> exposer;
        std::shared_ptr<prometheus::Registry> registry;
    };
}