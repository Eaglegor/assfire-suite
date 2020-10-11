#include "MetricsCollector.hpp"
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <prometheus/counter.h>
#include <prometheus/summary.h>
#include <spdlog/spdlog.h>

using namespace assfire::scheduler::transport;

MetricsCollector::MetricsCollector(const std::shared_ptr<prometheus::Exposer> &exposer)
        : exposer(exposer)
{
    if (exposer) {
        registry = std::make_shared<prometheus::Registry>();
    }
}

MetricsCollector::~MetricsCollector() = default;
