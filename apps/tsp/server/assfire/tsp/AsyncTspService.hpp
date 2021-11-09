#pragma once

#include <assfire/api/v1/tsp/service.grpc.pb.h>

namespace assfire::tsp {
    class AsyncTspService : public assfire::api::v1::tsp::TspService::CallbackService {

    };
}