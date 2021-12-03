#pragma once

#include <iostream>
#include "../Location.hpp"

namespace assfire::locations {
    std::ostream &operator<<(std::ostream &os, const Coordinate &value);
    std::ostream &operator<<(std::ostream &os, const Location &value);

    std::wostream &operator<<(std::wostream &os, const Coordinate &value);
    std::wostream &operator<<(std::wostream &os, const Location &value);
}