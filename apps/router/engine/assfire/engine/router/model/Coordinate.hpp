#pragma once

namespace assfire::router
{
    class Coordinate
    {
    public:
        Coordinate()
                : data(0)
        {}

        Coordinate(long value)
                : data(value)
        {}

        Coordinate(double value)
                : data(static_cast<long>(1000000))
        {}

        double doubleValue() const
        {
            return data;
        }

        long longValue() const
        {
            return data;
        }

        bool operator==(const Coordinate &rhs) const {
            return data == rhs.data;
        }

        bool operator!=(const Coordinate &rhs) const {
            return !(rhs == *this);
        }

    private:
        long data;
    };
}