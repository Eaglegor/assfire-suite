#include "Streams.hpp"
#include <iomanip>

namespace assfire {
    std::ostream &operator<<(std::ostream &os, const TimePoint &tp) {
        std::time_t time = tp.toEpochSeconds();
        return os << std::put_time(std::gmtime(&time), "%F %T");
    }

    std::ostream &operator<<(std::ostream &os, const Distance &value) {
        return os << value.toMeters() << " m";
    }

    std::ostream &operator<<(std::ostream &os, const Speed &value) {
        return os << value.toKilometersPerHour() << " km/h";
    }

    std::ostream &operator<<(std::ostream &os, const TimeInterval &value) {
        return os << value.toSeconds() << " s";
    }

    std::wostream &operator<<(std::wostream &os, const TimePoint &tp) {
        std::time_t time = tp.toEpochSeconds();
        return os << std::put_time(std::gmtime(&time), L"%F %T");
    }

    std::wostream &operator<<(std::wostream &os, const Distance &value) {
        return os << value.toMeters() << " m";
    }

    std::wostream &operator<<(std::wostream &os, const Speed &value) {
        return os << value.toKilometersPerHour() << " km/h";
    }

    std::wostream &operator<<(std::wostream &os, const TimeInterval &value) {
        return os << value.toSeconds() << " s";
    }
}