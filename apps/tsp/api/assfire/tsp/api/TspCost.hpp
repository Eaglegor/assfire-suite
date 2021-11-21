#pragma once

namespace assfire::tsp {
    class TspCost {
    public:
        using value_type = double;

        explicit TspCost(value_type value)
                : value(value)
        {}

        value_type getValue() const
        {
            return value;
        }

        int operator<=>(const TspCost& rhs) const {
            if(value < rhs.value) return -1;
            if(value > rhs.value) return 1;
            return 0;
        }

    private:
        value_type value;
    };
}