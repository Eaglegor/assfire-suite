#pragma once

#include <assfire/api/v1/scheduler/concepts.pb.h>
#include <assfire/scheduler/api/Waybill.hpp>

namespace assfire::api::v1::scheduler {
    class WaybillTranslator {
    public:
        using ApiWaybill = assfire::scheduler::Waybill;
        using ProtoWaybill = assfire::api::v1::scheduler::Waybill;

        static ApiWaybill fromProto(const ProtoWaybill& obj);
        static ProtoWaybill toProto(const ApiWaybill& obj);
    };
}


