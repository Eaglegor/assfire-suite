#pragma once

#include <chrono>

namespace assfire::util
{
    class RedisRetryPolicy
    {
    public:
        static RedisRetryPolicy noRetry() {
            return {0, std::chrono::milliseconds(0)};
        }

        static RedisRetryPolicy retryUntilSuccess(std::chrono::milliseconds interval) {
            return {-1, interval};
        }

        static RedisRetryPolicy retryFor(int times, std::chrono::milliseconds interval) {
            return {times, interval};
        }

        bool retryAllowed() const {
            return retry_attempts > 0;
        }

        bool attemptAllowed(int number) const {
            return number == 0 || retry_attempts < 0 || retry_attempts > number;
        }

        std::chrono::milliseconds getDelayMs(int attempt) const {
            return delay_ms;
        }

    private:
        RedisRetryPolicy(int retry_attempts, const std::chrono::milliseconds &delay_ms)
                : retry_attempts(retry_attempts), delay_ms(delay_ms) {}

        int retry_attempts = 0;
        std::chrono::milliseconds delay_ms = std::chrono::milliseconds(0);
    };
}