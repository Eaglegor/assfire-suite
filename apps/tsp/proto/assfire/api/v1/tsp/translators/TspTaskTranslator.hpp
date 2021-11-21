#pragma once

#include "assfire/api/v1/tsp/concepts.pb.h"
#include "assfire/tsp/api/TspTask.hpp"

namespace assfire::api::v1::tsp {
    class TspTaskTranslator {
    public:
        using ProtoTask = assfire::api::v1::tsp::TspTask;

        using ApiTask = assfire::tsp::TspTask;

        static ApiTask fromProto(const ProtoTask &value);
        static ProtoTask toProto(const ApiTask &value);
    };
}

