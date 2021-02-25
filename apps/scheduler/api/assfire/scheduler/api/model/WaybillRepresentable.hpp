#include <concepts>

#include "WaybillSchedulable.hpp"

namespace assfire::scheduler {
    template<typename T, typename TimePoint = typename T::AllocationType::TimePointType, typename TimeInterval = decltype(typename T::TimePointType() - typename T::TimePointType())>
    concept WaybillRepresentable = requires(T s) {
        { s[int()] } -> std::same_as<typename T::AllocationType&>;
    };
}