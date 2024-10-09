#pragma once

#include "Gun.h"
#include "Ammo.h"

#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <array>

namespace ShooterCentral{

    class EventType {
    public:
        EventType(std::string setName);
        ~EventType();

        std::string getName() const;

        operator std::string() const;
        bool operator==(const EventType& other) const;
        friend std::ostream& operator<<(std::ostream& stream, const EventType& event);

    private:
        std::string name;
    };

    namespace EventTypes{
        const EventType MATCH           { "MATCH" };
        const EventType WORK_RANGE      { "WORK RANGE" };
        const EventType PERSONAL_RANGE  { "PERSONAL RANGE" };
        const EventType ET_NA           { "N/A" };
    }

    struct EventLocation{
        std::string location;
    };

    // MARK: MATCH INFO
    class Event final {
    public:
        Event(std::string setName, EventLocation setLocation, EventType setEventType, std::string setNotes);
        ~Event();

        std::string     getName()           const;
        std::string     getNotes()          const;
        EventType       getEventType()      const;
        EventLocation   getEventLocation()  const;

       // bool            addGun(GunPtr gun, AmmoPtr ammo, uint64_t rounds);

    private:
        std::string                                         name;
        EventLocation                                       location;
        EventType                                           eventType;
       // std::vector<std::tuple<GunPtr, AmmoPtr, uint64_t>>  gunsUsed;
        std::string                                         notes;
        
    };

    using EventPtr = std::shared_ptr<Event>;
}