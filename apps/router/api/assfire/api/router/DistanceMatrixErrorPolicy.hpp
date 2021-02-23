#pragma once

namespace assfire::router {
    enum class DistanceMatrixErrorPolicy {
        ON_ERROR_RETURN_INFINITY,
        ON_ERROR_THROW
    };
}