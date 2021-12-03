#include "Streams.hpp"

namespace assfire::locations {
    std::ostream &operator<<(std::ostream &os, const Coordinate &value) {
        return os << value.doubleValue();
    }
    std::ostream &operator<<(std::ostream &os, const Location &value) {
        return os << "(" << value.getLatitude() << ", " << value.getLongitude() << ")";
    }
    std::wostream &operator<<(std::wostream &os, const Coordinate &value) {
        return os << value.doubleValue();
    }
    std::wostream &operator<<(std::wostream &os, const Location &value) {
        return os << "(" << value.getLatitude() << ", " << value.getLongitude() << ")";
    }
}