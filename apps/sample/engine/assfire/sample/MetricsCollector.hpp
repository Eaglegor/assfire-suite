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

namespace assfire {
    class MetricsCollector
    {
    public:
        explicit MetricsCollector(const std::shared_ptr<prometheus::Exposer>& exposer = nullptr);
        MetricsCollector(const MetricsCollector& rhs) = default;
        ~MetricsCollector();

        void incrementGreetsStartingFromA();

        void incrementGreetsStartingFromB();

        void incrementGreetsStartingFromNotAB();

        assfire::metrics::stopwatch greetingStopwatch();

    private:
        std::shared_ptr<prometheus::Exposer> exposer;
        std::shared_ptr<prometheus::Registry> registry;

        prometheus::Family<prometheus::Counter> *greets_count_family;
        prometheus::Counter *greets_count_starting_from_a;
        prometheus::Counter *greets_count_starting_from_b;
        prometheus::Counter *greets_count_starting_from_not_a_b;

        prometheus::Family<prometheus::Summary> *greets_calculation_time_family;
        prometheus::Summary *greets_calculation_time;
    };
}