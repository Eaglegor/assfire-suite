#pragma once

namespace assfire::router {
    enum class DistanceMatrixCachingPolicy {
        NO_CACHING,
        FULL_MATRIX_PRECACHING,
        AUTO
    };
}