#include "Event.h"

using namespace ShooterCentral;


void ShooterCentral::to_json(LAS::json& j, const ShootingEventMetadata& data){
    std::ostringstream timeString;
    timeString << std::chrono::system_clock::time_point{std::chrono::sys_days{data.date}};

    j = LAS::json{
        { "location",       data.location    },
            { "eventType",      data.eventType   },
            { "notes",          data.notes       },
            { "date",           timeString.str() }
    };
}

