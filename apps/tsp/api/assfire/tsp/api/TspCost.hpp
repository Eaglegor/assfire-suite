#pragma once

namespace assfire::tsp {
    class TspCost {
    public:
        TspCost(double value)
                : value(value)
        {}

        double getValue() const
        {
            return value;
        }

    private:
        double value;
    };
}