#include "Greeter.hpp"
#include <spdlog/spdlog.h>

namespace assfire::sample {
    Greeter::Greeter(const MetricsCollector &metrics_collector) :
            metrics_collector(metrics_collector)
    {

    }

    std::string Greeter::greet(const std::string &name) const
    {
        auto stopwatch = metrics_collector.greetingStopwatch();
        if (name.empty()) return "Bad name";
        SPDLOG_INFO("Greeting user {}", name);
        switch (name[0]) {
            case 'A':
            case 'a':
                metrics_collector.incrementGreetsStartingFromA();
                break;
            case 'B':
            case 'b':
                metrics_collector.incrementGreetsStartingFromB();
                break;
            default:
                metrics_collector.incrementGreetsStartingFromNotAB();
                break;
        }

        return std::string("Hello, ") + name;
    }


}