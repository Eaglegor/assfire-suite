#include "TspAlgorithmStateContainer.hpp"
#include <iterator>
#include <algorithm>
#include <utility>

namespace assfire::tsp {
    TspAlgorithmStateContainer::TspAlgorithmStateContainer(TspAlgorithmStateContainer::Persister persister) :
            persister(std::move(persister)) {
    }

    TspAlgorithmStateContainer::TspAlgorithmStateContainer(TspAlgorithmStateContainer::TspAlgorithmStateDto state, TspAlgorithmStateContainer::Persister persister) :
            state(std::move(state)), persister(std::move(persister)) {
    }

    TspAlgorithmStateContainer::TspAlgorithmStateDto &TspAlgorithmStateContainer::getDto() {
        return state;
    }

    const TspAlgorithmStateContainer::TspAlgorithmStateDto &TspAlgorithmStateContainer::getDto() const {
        return state;
    }

    void TspAlgorithmStateContainer::reset() {
        state.clear_points();
        state.clear_sequence();
        state.clear_instance();
    }

    void TspAlgorithmStateContainer::setPoints(const TspTask::Points &points) {
        state.clear_points();
        for (const TspPoint &pt : points) {
            state.add_points(pt.getId());
        }
    }

    void TspAlgorithmStateContainer::setSequence(const TspSolution::Sequence &sequence) {
        state.clear_sequence();
        for (const auto &pt : sequence) {
            state.add_sequence(pt);
        }
    }

    TspSolution::Sequence TspAlgorithmStateContainer::getSequence() const {
        TspSolution::Sequence result;
        std::copy(state.sequence().begin(), state.sequence().end(), std::back_inserter(result));
        return result;
    }

    bool TspAlgorithmStateContainer::isConsistent() const {
        return state.points_size() == state.sequence_size() &&
               std::find_if(state.sequence().begin(), state.sequence().end(), [&](int id) { return id >= state.points_size(); }) == state.sequence().end();
    }

    bool TspAlgorithmStateContainer::isCompatibleWith(const TspTask &task) const {
        return std::equal(state.points().begin(), state.points().end(), task.getPoints().begin(), task.getPoints().end(), [](const auto &saved_id,
                                                                                                                             const auto &task_point) {
            return saved_id == task_point.getId();
        });
    }

    bool TspAlgorithmStateContainer::canBeUsedFor(const TspTask &task) const {
        return isConsistent() && isCompatibleWith(task);
    }

    bool TspAlgorithmStateContainer::containsTwoOptAlgorithmState() const {
        return state.has_two_opt_algorithm_state();
    }

    void TspAlgorithmStateContainer::setState(TspAlgorithmStateContainer::TspAlgorithmStateDto state) {
        TspAlgorithmStateContainer::state = std::move(state);
    }

    void TspAlgorithmStateContainer::persist() {
        persister(state);
    }

    bool TspAlgorithmStateContainer::containsTimeWastingAlgorithmState() const {
        return state.has_time_wasting_algorithm_state();
    }
}