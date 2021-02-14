#pragma once

#include <functional>
#include <memory>
#include <chrono>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <assfire/metrics/stopwatch.hpp>

namespace assfire::router {
	class MetricsCollector
	{
	public:
	    using Stopwatch = assfire::metrics::stopwatch;

        virtual ~MetricsCollector() = default;
    };
}