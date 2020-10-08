#pragma once

#include <functional>
#include <chrono>

namespace assfire {
    namespace metrics {
        class stopwatch
        {
        public:
            stopwatch(std::function<void(long)> callback = [](long l) {}) :
                    callback(callback),
                    start(std::chrono::high_resolution_clock::now())
            {}

            stopwatch(stopwatch &&rhs) = default;

            stopwatch(const stopwatch &rhs) = delete;

            ~stopwatch()
            {
                auto end = std::chrono::high_resolution_clock::now();
                callback(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
            }

        private:
            std::function<void(long)> callback;
            std::chrono::high_resolution_clock::time_point start;
        };
    }
}