#include "WaybillBuilder.hpp"
#include "io/markdown/MarkdownTableParser.hpp"
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
#include "assfire/router/api/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp"
#include <stdexcept>

using namespace assfire;
using namespace assfire::scheduler;

namespace {
    enum WaybillEntryType {
        SHIFT_START,
        SHIFT_END,
        ALLOCATION,
        ROUTE,
        UNKNOWN
    };

    WaybillEntryType parseType(const std::string& type) {
        if(type.find("SHIFT_START") != std::string::npos) {
            return SHIFT_START;
        }
        if(type.find("SHIFT_END") != std::string::npos) {
            return SHIFT_END;
        }
        if(type.find("ALLOCATION") != std::string::npos) {
            return ALLOCATION;
        }
        if(type.find("ROUTE") != std::string::npos) {
            return ROUTE;
        }
        return UNKNOWN;
    }
}

Waybill WaybillBuilder::buildWaybill() {
    return Waybill(scheduler::TimeWindow(shift_start, shift_end), allocations);
}

Waybill WaybillBuilder::buildCollapsedWaybill() {
    Waybill::Allocations collapsed_allocations = allocations;
    for(auto& alloc : collapsed_allocations) {
        alloc.setStartTime(TimePoint::zero());
        alloc.setEndTime(TimePoint::zero());
    }
    return Waybill(scheduler::TimeWindow(shift_start, shift_end), collapsed_allocations);
}

router::DistanceMatrix WaybillBuilder::buildDistanceMatrix() {
    return *distance_matrix;
}

namespace {
    TimePoint parseTimePoint(const std::string& tp) {
        return TimePoint::zero();
    }

    TimeWindow parseTimeWindow(const std::string& tw) {
        return TimeWindow(TimePoint::zero(), TimePoint::zero());
    }

    TimeWindow parseTimeRange(const std::string& tr) {
        return TimeWindow(TimePoint::zero(), TimePoint::zero());
    }

    TimeInterval parseTimeInterval(const std::string& ti) {
        return TimeInterval::zero();
    }

    Location parseLocation(const std::string& l) {
        return Location::fromEncodedLatLon(0, 0);
    }
};

namespace {
    static constexpr const char* TYPE = "Type";
    static constexpr const char* ALLOCATED_TIME = "Allocated time";
    static constexpr const char* TIME_WINDOW = "Time window";
    static constexpr const char* LOCATION = "Location";
}

WaybillBuilder& WaybillBuilder::parse(const std::string &schedule) {

    MarkdownTableParser parser;
    parser.parseTable(schedule);

    std::unique_ptr<router::MockRouteProviderEngine> mock_engine = std::make_unique<router::MockRouteProviderEngine>();

    for(int i = 0; i < parser.getEntries().size(); ++i) {
        const auto& entry = parser.getEntries()[i];
        if(!entry.contains(TYPE)) {
            throw std::invalid_argument("Schedule entry doesn't contain Type");
        }
        WaybillEntryType type = parseType(entry.at(TYPE));

        switch(type) {
            case SHIFT_START: {
                shift_start = parseTimeRange(entry.at(ALLOCATED_TIME)).getStartTime();
                break;
            }
            case SHIFT_END: {
                shift_end = parseTimeRange(entry.at(ALLOCATED_TIME)).getEndTime();
                break;
            }
            case ALLOCATION: {
                TimeWindow allocated_time = parseTimeRange(entry.at(ALLOCATED_TIME));
                TimeWindow time_window = parseTimeWindow(entry.at(TIME_WINDOW));
                Location location = entry.contains(LOCATION) ? parseLocation(entry.at(LOCATION)) : Location::fromEncodedLatLon(i, i);
                allocations.push_back(WaybillAllocation(allocated_time.getStartTime(), allocated_time.getEndTime(), allocated_time.getWidth(), {time_window}, location));
                break;
            }
            case ROUTE: {
                Location prev_location = allocations.back().getLocation().getLocation();
                for(int j = i + 1; j < parser.getEntries().size(); ++j) {
                    const auto& next_entry = parser.getEntries()[j];
                    WaybillEntryType next_type = parseType(next_entry.at(TYPE));
                    if(next_type != ALLOCATION) continue;
                    Location next_location = next_entry.contains(LOCATION) ? parseLocation(next_entry.at(LOCATION)) : Location::fromEncodedLatLon(j, j);

                    TimeWindow route_time = parseTimeRange(entry.at(ALLOCATED_TIME));
                    mock_engine->addResponse(prev_location, next_location, router::RouteDetails(Distance::fromMeters(route_time.getWidth().toSeconds()), route_time.getWidth(), {}));
                    break;
                }
                break;
            }
        }
    }

    distance_matrix = std::make_unique<router::DistanceMatrix>(std::make_shared<router::DirectRequestDistanceMatrixEngine>(std::move(mock_engine), 0));

    return *this;
}