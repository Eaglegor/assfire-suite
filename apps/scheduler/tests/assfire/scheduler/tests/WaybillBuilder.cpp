#include "WaybillBuilder.hpp"

#include <regex>
#include <stdexcept>

#include "io/markdown/MarkdownTableParser.hpp"
#include "assfire/router/tests/MockRouteProviderEngine.hpp"
#include "assfire/router/api/distance_matrix_engines/DirectRequestDistanceMatrixEngine.hpp"

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

    WaybillEntryType parseType(const std::string &type) {
        if (type.find("SHIFT_START") != std::string::npos) {
            return SHIFT_START;
        }
        if (type.find("SHIFT_END") != std::string::npos) {
            return SHIFT_END;
        }
        if (type.find("ALLOCATION") != std::string::npos) {
            return ALLOCATION;
        }
        if (type.find("ROUTE") != std::string::npos) {
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
    for (auto &alloc : collapsed_allocations) {
        alloc.setStartTime(TimePoint::zero());
        alloc.setEndTime(TimePoint::zero());
        alloc.setNextRouteInfo(router::RouteInfo::zero());
    }
    return Waybill(scheduler::TimeWindow(shift_start, shift_end), collapsed_allocations);
}

router::DistanceMatrix WaybillBuilder::buildDistanceMatrix() {
    return *distance_matrix;
}

namespace {

    static const std::regex TIME_POINT_REGEX("(\\d+):?"); // 1:00:34
    static const std::regex TIME_INTERVAL_REGEX = TIME_POINT_REGEX;
    static const std::regex TIME_WINDOW_REGEX("\\[\\s*((?:\\d+:?)+)\\s*-\\s*((?:\\d+:?)+)\\]"); // [0:15:00 - 20:35:23:01]
    static const std::regex TIME_RANGE_REGEX("\\s*((?:\\d+:?)+)(?:\\s*-\\s*((?:\\d+:?)+))?"); // 0:15:00 - 20:35:23:01
    static const std::regex LOCATION_REGEX("\\(\\s*([\\d.]+)\\s*,\\s*([\\d.]+)\\s*\\)"); // (34.5, 56)

    TimePoint parseTimePoint(const std::string &tp) {
        std::vector<std::string> tokens;

        auto matches_begin =
                std::sregex_iterator(tp.begin(), tp.end(), TIME_POINT_REGEX);
        auto matches_end = std::sregex_iterator();

        for (auto iter = matches_begin; iter != matches_end; ++iter) {
            tokens.push_back(iter->str(1));
        }

        long seconds = 0;
        for (int i = 0; i < tokens.size(); ++i) {
            const std::string &token = tokens[tokens.size() - 1 - i];
            long value = std::stol(token);
            switch (i) {
                case 0: // seconds
                    break;
                case 1: // minutes
                    value *= 60;
                    break;
                case 2: // hours
                    value *= 3600;
                    break;
                case 3: // days
                    value *= 86400;
                    break;
                default:
                    throw std::runtime_error("Unsupported time point format");
            }
            seconds += value;
        }

        return TimePoint::fromEpochSeconds(seconds);
    }

    TimeInterval parseTimeInterval(const std::string &ti) {
        std::vector<std::string> tokens;

        auto matches_begin =
                std::sregex_iterator(ti.begin(), ti.end(), TIME_INTERVAL_REGEX);
        auto matches_end = std::sregex_iterator();

        for (auto iter = matches_begin; iter != matches_end; ++iter) {
            tokens.push_back(iter->str(1));
        }

        long seconds = 0;
        for (int i = 0; i < tokens.size(); ++i) {
            const std::string &token = tokens[tokens.size() - 1 - i];
            long value = std::stol(token);
            switch (i) {
                case 0: // seconds
                    break;
                case 1: // minutes
                    value *= 60;
                    break;
                case 2: // hours
                    value *= 3600;
                    break;
                case 3: // days
                    value *= 86400;
                    break;
                default:
                    throw std::runtime_error("Unsupported time interval format");
            }
            seconds += value;
        }

        return TimeInterval::fromSeconds(seconds);
    }

    TimeWindow parseTimeWindow(const std::string &tw) {
        std::vector<std::string> tokens;

        auto matches_begin =
                std::sregex_iterator(tw.begin(), tw.end(), TIME_WINDOW_REGEX);
        auto matches_end = std::sregex_iterator();

        for (auto iter = matches_begin; iter != matches_end; ++iter) {
            for(auto it2 = iter->begin() + 1; it2 != iter->end(); ++it2) {
                if(it2->matched) {
                    tokens.push_back(it2->str());
                }
            }
        }

        if (tokens.empty()) return TimeWindow::infinity();
        if (tokens.size() != 2) throw std::runtime_error("Invalid time window format");

        return TimeWindow(parseTimePoint(tokens[0]), parseTimePoint(tokens[1]));
    }

    TimeWindow parseTimeRange(const std::string &tr) {
        std::vector<std::string> tokens;

        auto matches_begin =
                std::sregex_iterator(tr.begin(), tr.end(), TIME_RANGE_REGEX);
        auto matches_end = std::sregex_iterator();

        for (auto iter = matches_begin; iter != matches_end; ++iter) {
            for(auto it2 = iter->begin() + 1; it2 != iter->end(); ++it2) {
                if(it2->matched) {
                    tokens.push_back(it2->str());
                }
            }
        }

        if (tokens.size() > 2) throw std::runtime_error("Invalid time range format");

        if (tokens.size() == 1) {
            return TimeWindow(parseTimePoint(tokens[0]), parseTimePoint(tokens[0]));
        } else {
            return TimeWindow(parseTimePoint(tokens[0]), parseTimePoint(tokens[1]));
        }
    }

    WaybillAllocation::Location parseLocation(const std::string &l) {
        std::vector<std::string> tokens;

        auto matches_begin =
                std::sregex_iterator(l.begin(), l.end(), TIME_WINDOW_REGEX);
        auto matches_end = std::sregex_iterator();

        for (auto iter = matches_begin; iter != matches_end; ++iter) {
            for(auto it2 = iter->begin() + 1; it2 != iter->end(); ++it2) {
                if(it2->matched) {
                    tokens.push_back(it2->str());
                }
            }
        }

        if (tokens.size() != 2) throw std::runtime_error("Invalid location format");

        return WaybillAllocation::Location::fromEncodedLatLon(std::stoi(tokens[0]), std::stoi(tokens[1]));
    }
};

namespace {
    static constexpr const char *TYPE = "Type";
    static constexpr const char *ALLOCATED_TIME = "Allocated time";
    static constexpr const char *TIME_WINDOW = "Time window";
    static constexpr const char *LOCATION = "Location";
}

WaybillBuilder &WaybillBuilder::parse(const std::string &schedule) {

    MarkdownTableParser parser;
    parser.parseTable(schedule);

    std::unique_ptr<router::MockRouteProviderEngine> mock_engine = std::make_unique<router::MockRouteProviderEngine>();

    for (int i = 0; i < parser.getEntries().size(); ++i) {
        const auto &entry = parser.getEntries()[i];
        if (!entry.contains(TYPE)) continue;
        WaybillEntryType type = parseType(entry.at(TYPE));

        switch (type) {
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
                TimeWindow time_window = entry.contains(TIME_WINDOW) ? parseTimeWindow(entry.at(TIME_WINDOW)) : TimeWindow::infinity();
                WaybillAllocation::Location location;
                if(entry.contains(LOCATION)) location = parseLocation(entry.at(LOCATION));
                else if(i == 0) location = WaybillAllocation::Location::fromEncodedLatLon(i, i);
                else {
                    WaybillEntryType prev_type = UNKNOWN;
                    for(int j = i - 1; j >= 0 && prev_type == UNKNOWN; --j) {
                        prev_type = parseType(parser.getEntries()[j].at(TYPE));
                    }
                    if(prev_type == ALLOCATION) {
                        location = allocations.back().getLocation().getRawLocation();
                    } else {
                        location = WaybillAllocation::Location::fromEncodedLatLon(i, i);
                    }
                }
                allocations.push_back(WaybillAllocation(allocated_time.getStartTime(), allocated_time.getEndTime(), allocated_time.getWidth(), {time_window}, location));
                break;
            }
            case ROUTE: {
                WaybillAllocation::Location prev_location = allocations.back().getLocation().getRawLocation();
                for (int j = i + 1; j < parser.getEntries().size(); ++j) {
                    const auto &next_entry = parser.getEntries()[j];
                    WaybillEntryType next_type = parseType(next_entry.at(TYPE));
                    if (next_type != ALLOCATION) continue;
                    WaybillAllocation::Location next_location = next_entry.contains(LOCATION) ? parseLocation(next_entry.at(LOCATION)) : WaybillAllocation::Location::fromEncodedLatLon(j, j);

                    TimeWindow route_time = parseTimeRange(entry.at(ALLOCATED_TIME));
                    router::RouteDetails details(Distance::fromMeters(route_time.getWidth().toSeconds()), route_time.getWidth(), {});
                    mock_engine->addResponse(prev_location, next_location, details);
                    allocations.back().setNextRouteInfo(details.getSummary());
                    break;
                }
                break;
            }
        }
    }

    distance_matrix = std::make_unique<router::DistanceMatrix>(std::make_shared<router::DirectRequestDistanceMatrixEngine>(std::move(mock_engine), 0));

    return *this;
}