#pragma once

#include "Gun.h"

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
        bool operator== (const Location& other) const;
        bool operator<  (const Location& other) const;

    private:
        std::string name;
    };

    class EventType{
    public:
        explicit EventType(std::string setName="NULL");
        ~EventType();

        std::string getName() const;
        operator std::string() const;
        bool operator== (const EventType& other) const;
        bool operator<  (const EventType& other) const;

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

    void to_json(LAS::json& j, const EventMetadata& data);
    void from_json(const LAS::json& j, EventMetadata& data);
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& timeString);

    class Event {
    public:
        static constexpr int MAX_NUM_GUNS { 5 };

        Event(Location setLocation, EventType setEventType, std::string setNotes, ymd setDate);
        Event(const EventMetadata& info);
        ~Event();

        bool    addGun          (const GunAndAmmo& gun);        // Will not amend existing entries if a gun is already in the container
        bool    hasUsedGun      (const GunMetadata& gun) const;
        int     totalGunsUsed   () const;

        const EventMetadata& getInfo() const;

        std::string getNotes()      const;
        EventType   getEventType()  const;
        Location    getLocation()   const;
        const ymd&  getDate()       const;

        std::string printDate()     const;

        const std::array<GunAndAmmo, MAX_NUM_GUNS>& getGunsUsed() const;
   private:
        EventMetadata eventMetadata;

        std::array<GunAndAmmo, MAX_NUM_GUNS> gunsUsedList;
        int nextIndex { 0 };
    };
}
