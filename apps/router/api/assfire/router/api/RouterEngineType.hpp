#pragma once

namespace assfire::router {
    enum class RouterEngineType {
        UNKNOWN = 0,
        OSRM = 1,
        CROWFLIGHT = 2,
        EUCLIDEAN = 3,
        RANDOM = 4
    };
}