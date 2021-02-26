#pragma once

#include "WaybillAllocation.hpp"

namespace assfire::scheduler {
    class Waybill {
    public:
        using size_t = int;

        virtual ~Waybill() = default;

        virtual WaybillAllocation& operator[](size_t i) = 0;
        virtual const WaybillAllocation& operator[](size_t i) const = 0;
        virtual size_t size() const = 0;
    };
}