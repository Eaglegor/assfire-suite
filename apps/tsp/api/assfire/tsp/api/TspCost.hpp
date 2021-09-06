#pragma once

namespace assfire::tsp {
    class TspCost {
    public:
        explicit TspCost(double value)
                : value(value)
        {}

        double getValue() const
        {
            return value;
        }

        int operator<=>(const TspCost& rhs) const {
            if(value < rhs.value) return -1;
            if(value > rhs.value) return 1;
            return 0;
        }

    private:
        double value;
    };
}