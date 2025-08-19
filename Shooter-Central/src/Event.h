#pragma once

#include "Gun.h"

#include <vector>
#include <chrono>
#include <memory>

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
        std::string notes       { };
        ymd         date        { std::chrono::sys_days{std::chrono::year_month_day{std::chrono::year{0}, std::chrono::month{0}, std::chrono::day{0}}}};

        const Location&    location;
        const EventType&   eventType;

        bool operator== (const EventMetadata& other) const;
        bool operator<  (const EventMetadata& other) const;
    };

    void to_json(LAS::json& j, const EventMetadata& data);
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& timeString);

    class Event {
    public:
        Event(const EventMetadata& setInfo);
        ~Event();

        const EventMetadata& getInfo() const;                   // Throws if eventMetadata is invalid

        bool    addGun          (const GunAndAmmo& gun);        // Will not amend existing entries if a gun is already in the container
        bool    hasUsedGun      (const GunMetadata& gun) const;
        int     totalGunsUsed   () const;

        std::string printDate()     const;

        const std::vector<GunAndAmmo>& getGunsUsed() const;
   private:
        EventMetadata eventMetadata;

        std::vector<GunAndAmmo> gunsUsedList;
    };
}
