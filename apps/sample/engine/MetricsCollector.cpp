#include "assfire/sample/MetricsCollector.hpp"
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <prometheus/counter.h>
#include <prometheus/summary.h>
#include <spdlog/spdlog.h>

assfire::MetricsCollector::MetricsCollector(const std::shared_ptr<prometheus::Exposer>& exposer) :
        exposer(exposer)
{
    if (exposer) {
        registry = std::make_shared<prometheus::Registry>();

        prometheus::Summary::Quantiles quantiles{{0.99, 0.1},
                                                 {0.95, 0.1},
                                                 {0.9,  0.1},
                                                 {0.5,  0.1}};

        greets_count_family = &prometheus::BuildCounter()
                .Name("assfire_sample_greets_count")
                .Help("Count of served greet requests")
                .Register(*registry);
        greets_count_starting_from_a = &greets_count_family->Add({{"firstLetter", "a"}});
        greets_count_starting_from_b = &greets_count_family->Add({{"firstLetter", "b"}});
        greets_count_starting_from_not_a_b = &greets_count_family->Add({{"firstLetter", "not_a_b"}});

        greets_calculation_time_family = &prometheus::BuildSummary()
                .Name("assfire_sample_greeting_time")
                .Help("Greeting time summary")
                .Register(*registry);
        greets_calculation_time = &greets_calculation_time_family->Add({}, quantiles);
    }
}

void assfire::MetricsCollector::incrementGreetsStartingFromA()
{
    if (!exposer) return;

    greets_count_starting_from_a->Increment();
}

void assfire::MetricsCollector::incrementGreetsStartingFromB()
{
    if (!exposer) return;

    greets_count_starting_from_b->Increment();
}

void assfire::MetricsCollector::incrementGreetsStartingFromNotAB()
{
    if (!exposer) return;

    greets_count_starting_from_not_a_b->Increment();
}

assfire::metrics::stopwatch assfire::MetricsCollector::greetingStopwatch()
{
    if (!exposer) return assfire::metrics::stopwatch();

    return assfire::metrics::stopwatch([this](long ns) {
        greets_calculation_time->Observe(ns);
        SPDLOG_TRACE("Observed greeting calculation time: {} ns", ns);
    });
}

assfire::MetricsCollector::~MetricsCollector() = default;

