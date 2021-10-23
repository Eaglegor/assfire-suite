#pragma once

#include <assfire/api/v1/tsp/state.pb.h>
#include "assfire/tsp/api/TspTask.hpp"
#include "assfire/tsp/api/TspSolution.hpp"
#include <atomic>

namespace assfire::tsp {
    class TspAlgorithmStateContainer {
    public:
        using TspAlgorithmStateDto = assfire::api::v1::tsp::TspAlgorithmState;
        using TwoOptAlgorithmStateDto = assfire::api::v1::tsp::TwoOptAlgorithmState;
        using Persister = std::function<void(const TspAlgorithmStateDto &)>;

        explicit TspAlgorithmStateContainer(Persister persister = [](const TspAlgorithmStateDto &) {});

        explicit TspAlgorithmStateContainer(TspAlgorithmStateDto state, Persister persister = [](const TspAlgorithmStateDto &) {});

        void setState(TspAlgorithmStateDto state);

        TspAlgorithmStateDto &getDto();

        const TspAlgorithmStateDto &getDto() const;

        void reset();

        void setPoints(const TspTask::Points &points);

        void setSequence(const TspSolution::Sequence &sequence);

        TspSolution::Sequence getSequence() const;

        bool isConsistent() const;

        bool isCompatibleWith(const TspTask &task) const;

        bool canBeUsedFor(const TspTask &task) const;

        bool containsTwoOptState() const;

        void persist();

    private:
        TspAlgorithmStateDto state;
        Persister persister;
    };
}