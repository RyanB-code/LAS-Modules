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

    class ShootingEventType{
    public:
        explicit ShootingEventType(std::string setName="NULL");
        ~ShootingEventType();

        std::string getName() const;
        operator std::string() const;
        bool operator== (const ShootingEventType& other) const;
        bool operator<  (const ShootingEventType& other) const;

    private:
        std::string name;
    };


    struct ShootingEventMetadata {
        std::string notes       { };
        ymd         date        { std::chrono::sys_days{std::chrono::year_month_day{std::chrono::year{0}, std::chrono::month{0}, std::chrono::day{0}}}};

        const Location&    location;
        const ShootingEventType&   eventType;

        bool operator== (const ShootingEventMetadata& other) const;
        bool operator<  (const ShootingEventMetadata& other) const;
    };

    void to_json(LAS::json& j, const ShootingEventMetadata& data);
    std::chrono::system_clock::time_point stringToTimepoint(const std::string& timeString);

    class ShootingEvent {
    public:
        ShootingEvent(const ShootingEventMetadata& setInfo);
        ~ShootingEvent();

        const ShootingEventMetadata& getInfo() const;                   // Throws if eventMetadata is invalid

        bool    addGun          (const GunAndAmmo& gun);        // Will not amend existing entries if a gun is already in the container
        bool    hasUsedGun      (const GunMetadata& gun) const;
        int     totalGunsUsed   () const;

        std::string printDate()     const;

        const std::vector<GunAndAmmo>& getGunsUsed() const;
   private:
        ShootingEventMetadata eventMetadata;

        std::vector<GunAndAmmo> gunsUsedList;
    };
}
