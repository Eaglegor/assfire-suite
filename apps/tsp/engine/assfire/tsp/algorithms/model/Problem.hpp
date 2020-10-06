#pragma once

#include <utility>

#include "Job.hpp"
#include "Resource.hpp"
#include "assfire/router/client/DistanceMatrix.hpp"
#include "assfire/router/client/RouterClient.hpp"
#include "assfire/api/v1/service/tsp/tsp.pb.h"

namespace assfire::tsp
{
    struct Problem
    {
        Problem(router::DistanceMatrix distance_matrix, std::vector<Job> jobs, Resource resource)
                : distance_matrix(std::move(distance_matrix)), jobs(std::move(jobs)), resource(std::move(resource))
        {}

        const router::DistanceMatrix distance_matrix;
        const std::vector<Job> jobs;
        const Resource resource;

        static Problem of(const assfire::api::v1::service::tsp::TspTask &task,
                           const router::RouterClient &router_client)
        {
            router::DistanceMatrix distance_matrix = router_client.createDistanceMatrix(router::RouterClient::RequestStrategy::DIRECT_REQUEST);
            std::vector<Job> trip;
            std::transform(task.jobs().begin(), task.jobs().end(), std::back_inserter(trip), [&](const auto &api_job) {
                return Job(Location::of(api_job.location(), distance_matrix), api_job);
            });

            Resource resource(RoutingOptions::of(task.resource().routing_options(), distance_matrix), task.resource());

            for (const Job &from : trip) {
                for (const Job &to : trip) {
                    distance_matrix.prepareRoute(from.location.id, to.location.id, resource.routing_options.id);
                }
            }

            return Problem(std::move(distance_matrix), std::move(trip), std::move(resource));
        }
    };
}