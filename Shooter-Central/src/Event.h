#pragma once

#include <chrono>

using ymd       = std::chrono::year_month_day;
using timepoint = std::chrono::system_clock::time_point;

namespace ShooterCentral{

    class Location{
    public:
        explicit Location(std::string setName="NULL");
        ~Location();

        std::string getName() const;
        operator std::string() const;
        bool operator==(const Location& other) const;

    private:
        std::string name;
    };

    class EventType{
    public:
        explicit EventType(std::string setName="NULL");
        ~EventType();

        std::string getName() const;
        operator std::string() const;
        bool operator==(const EventType& other) const;

    private:
        std::string name;
    };


    struct EventMetadata {
        Location    location    { };
        EventType   eventType   { };
        std::string notes       { };
        ymd         date        { std::chrono::sys_days{std::chrono::year_month_day{std::chrono::year{0}, std::chrono::month{0}, std::chrono::day{0}}}};

        bool operator== (const EventMetadata& other) const;
        bool operator<  (const EventMetadata& other) const;
   };
 
}
