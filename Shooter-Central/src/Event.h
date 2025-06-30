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


    class EventMetadata {
    public:
        EventMetadata();
        EventMetadata(Location setLocation, EventType setEventType, std::string setNotes, ymd setDate);
        ~EventMetadata();

        std::string getNotes()      const;
        EventType   getEventType()  const;
        Location    getLocation()   const;
        const ymd&  getDate()       const;
        timepoint   getTimepoint()  const;

    private:
        Location    location;
        EventType   eventType;
        std::string notes;
        ymd         date;
    };
 
}
