#pragma once

#include "TimePoint.hpp"
#include <compare>

namespace assfire {
    class TimeInterval {
    private:
        using value_type = std::int64_t;

        explicit TimeInterval(value_type seconds) : seconds(seconds) {}

        static constexpr value_type INFINITE_TIME_INTERVAL = 3600000000;

    public:
        auto operator<=>(const TimeInterval& rhs) const = default;

        std::int64_t toSeconds() const;

        static TimeInterval fromSeconds(std::int64_t seconds);

        bool isInfinity() const;

        bool isZero() const;

        bool isPositive() const;

        static TimeInterval zero();

        static TimeInterval infinity();

        TimeInterval operator+(const TimeInterval& rhs);

        TimeInterval operator-(const TimeInterval& rhs);

        TimeInterval& operator+=(const TimeInterval& rhs);

        TimeInterval& operator-=(const TimeInterval& rhs);

        template<typename T>
        TimeInterval operator*(const T& val) {
            static_assert(std::is_arithmetic<T>::value, "Multiplier must be an arithmetic type");
            return TimeInterval::fromSeconds(val * seconds);
        }

        template<typename T>
        TimeInterval operator/(const T& val) {
            static_assert(std::is_arithmetic<T>::value, "Multiplier must be an arithmetic type");
            return TimeInterval::fromSeconds(val / seconds);
        }

        template<typename T>
        TimeInterval& operator*=(const T& val) {
            static_assert(std::is_arithmetic<T>::value, "Multiplier must be an arithmetic type");
            val *= seconds;
            return *this;
        }

        template<typename T>
        TimeInterval& operator/=(const T& val) {
            static_assert(std::is_arithmetic<T>::value, "Multiplier must be an arithmetic type");
            val /= seconds;
            return *this;
        }

    private:
        value_type seconds;
    };
}