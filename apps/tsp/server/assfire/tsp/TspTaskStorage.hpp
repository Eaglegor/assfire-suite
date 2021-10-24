#pragma once

#include <optional>
#include <assfire/api/v1/tsp/concepts.pb.h>

namespace assfire::tsp {
    class TspTaskStorage {
    public:
        using TspTask = assfire::api::v1::tsp::TspTask;

        virtual ~TspTaskStorage() {}

        virtual std::optional<TspTask> fetchTask(const std::string &id) const = 0;

        virtual void saveTask(const std::string &id, TspTask task) = 0;

        virtual void dropTask(const std::string &id) = 0;

        virtual bool isInProgress(const std::string& id) = 0;
        virtual void markAsInProgress(const std::string& id) = 0;
        virtual void markAsStopped(const std::string& id) = 0;
    };
}