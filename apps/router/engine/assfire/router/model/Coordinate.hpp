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

        double doubleValue() const
        {
            return data;
        }

        long longValue() const
        {
            return data;
        }

    private:
        long data;
    };
}