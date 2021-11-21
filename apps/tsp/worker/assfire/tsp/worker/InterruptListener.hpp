#pragma once

#include <string>
#include <functional>

namespace assfire::tsp {
    class InterruptListener {
    public:
        using SignalProcessor = std::function<void(int)>;
        virtual ~InterruptListener() = default;

        static constexpr int INTERRUPT = 0;
        static constexpr int PAUSE = 1;

        virtual void startListening() = 0;
        virtual void subscribe(const std::string& task_id, SignalProcessor processor) = 0;
        virtual void unsubscribe(const std::string& task_id) = 0;
    };
}