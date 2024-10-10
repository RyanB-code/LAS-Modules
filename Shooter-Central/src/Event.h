#pragma once

#include "Gun.h"
#include "Ammo.h"
#include "Time.h"

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
        const EventType USPSA_MATCH     { "USPSA MATCH" };
        const EventType WORK_RANGE      { "WORK RANGE" };
        const EventType PERSONAL_RANGE  { "PERSONAL RANGE" };
        const EventType ET_NA           { "N/A" };
    }

    // MARK: MATCH INFO
    class Event final {
    public:
        Event(std::string setName, std::string setLocation, EventType setEventType, std::string setNotes, Datestamp setDate);
        ~Event();

        std::string         getName()           const;
        std::string         getNotes()          const;
        EventType           getEventType()      const;
        std::string         getLocation()       const;
        const Datestamp&    getDatestamp()      const;

        bool addGun(GunPtr gun, TrackedAmmo ammoUsed);

        bool operator==(const Event& other) const;
    private:
        std::string                                         name;
        std::string                                         location;
        EventType                                           eventType;
        std::array<std::pair<GunPtr, TrackedAmmo>, 5>       gunsUsed;
        std::string                                         notes;
        Datestamp                                           date;
    };
    using EventPtr = std::shared_ptr<Event>;
}

namespace std{
    template <>
    struct hash<ShooterCentral::Event> {
        size_t operator()(const ShooterCentral::Event& event) const{
            std::size_t seed { 0 };

            std::hash_combine(seed, event.getName());
            std::hash_combine(seed, event.getLocation());
            std::hash_combine(seed, event.getDatestamp().printDate());

            return seed;
        }
    };
}

namespace ShooterCentral{

    class EventTracker {
    public:
        EventTracker(LAS::Logging::LoggerPtr setLogger);
        ~EventTracker();

        bool        setDirectory    (std::string directory);
        std::string getDirectory    () const;

        bool addEvent (Event& event);

        void        getAllEvents  (std::vector<Event>& list)    const;  // Clears vector before adding elements

    private:
        std::unordered_map<Event, EventPtr>    events;

        std::string             saveDirectory;
        LAS::Logging::LoggerPtr logger;

        static constexpr std::string CARTRIDGES_FILENAME {"Cartridges.json"};
    };
    using EventTrackerPtr = std::shared_ptr<EventTracker>;

}