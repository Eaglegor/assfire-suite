#pragma once

#include <iostream>
#include "../Coordinate.hpp"
#include "../Distance.hpp"
#include "../Location.hpp"
#include "../Speed.hpp"
#include "../TimeInterval.hpp"
#include "../TimePoint.hpp"

namespace assfire {
    std::ostream &operator<<(std::ostream &os, const Coordinate &value);
    std::ostream &operator<<(std::ostream &os, const Distance &value);
    std::ostream &operator<<(std::ostream &os, const Location &value);
    std::ostream &operator<<(std::ostream &os, const Speed &value);
    std::ostream &operator<<(std::ostream &os, const TimeInterval &value);
    std::ostream &operator<<(std::ostream &os, const TimePoint &value);

    std::wostream &operator<<(std::wostream &os, const Coordinate &value);
    std::wostream &operator<<(std::wostream &os, const Distance &value);
    std::wostream &operator<<(std::wostream &os, const Location &value);
    std::wostream &operator<<(std::wostream &os, const Speed &value);
    std::wostream &operator<<(std::wostream &os, const TimeInterval &value);
    std::wostream &operator<<(std::wostream &os, const TimePoint &value);
}