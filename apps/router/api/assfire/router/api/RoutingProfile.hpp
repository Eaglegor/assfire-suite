#pragma once

#include "assfire/concepts/Speed.hpp"

namespace assfire::router
{
    class RoutingProfile
    {
    public:
        enum class VehicleType
        {
            CAR
        };

        RoutingProfile(const Speed &speed, VehicleType vehicle_type = VehicleType::CAR)
                : speed(speed), vehicle_type(vehicle_type) {}

        Speed getSpeed() const {
            return speed;
        }

        VehicleType getVehicleType() const {
            return vehicle_type;
        }

    private:
        Speed speed;
        VehicleType vehicle_type = VehicleType::CAR;
    };
}