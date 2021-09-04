#pragma once

namespace assfire::tsp
{
    class TspValidationResult {
    public:
        TspValidationResult(bool is_valid)
                : is_valid(is_valid)
        {}

        bool isValid() const
        {
            return is_valid;
        }

    private:
        bool is_valid;
    };
}