#pragma once

#include "Gun.h"
#include "Ammo.h"

#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <array>
#include <chrono>

using ymd       = std::chrono::year_month_day;
using timepoint = std::chrono::system_clock::time_point;

namespace ShooterCentral{
    class Event final {
    public:
        Event();
        Event(std::string setName, std::string setLocation, std::string setEventType, std::string setNotes, ymd setDate);
        ~Event();

        std::string getName()       const;
        std::string getNotes()      const;
        std::string getEventType()  const;
        std::string getLocation()   const;
        const ymd&  getDate()       const;
        timepoint   getTimepoint()  const;
        
        std::string printDate()     const;

        bool    addGun          (Gun gun, TrackedAmmo ammoUsed);
        void    getAllGunsUsed  (std::vector<std::pair<Gun, TrackedAmmo>>& list) const;
        uint8_t getNumGunsUsed  () const;

        bool operator==(const Event& other) const;
    private:
        static constexpr int MAX_GUNS_PER_EVENT  { 5 };

        std::string name;
        std::string location;
        std::string eventType;
        std::string notes;
        ymd         date;

        std::array<std::pair<Gun, TrackedAmmo>, MAX_GUNS_PER_EVENT>  gunsUsed;
    };
    using EventPtr = std::shared_ptr<Event>;

    void to_json    (LAS::json& j, const Event& event);
    void from_json  (const LAS::json& j, Event& event); 
}

namespace std{
    template <>
    struct hash<ShooterCentral::Event> {
        size_t operator()(const ShooterCentral::Event& event) const{
            std::size_t seed { 0 };

            std::hash_combine(seed, event.getName());
            std::hash_combine(seed, event.getLocation());
            std::hash_combine(seed, event.printDate());

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

        uint64_t    getTotalEvents  () const;

        bool addEvent       (Event& event);
        void getAllEvents   (std::vector<Event>& list)    const;  // Clears vector before adding elements

        bool writeAllEvents () const;
        bool readEvents     ();

        void getAllEventTypes    (StringVector& lsit) const;   // Clears vector before adding elements
        bool addEventType        (const std::string& type);

        void getAllLocations     (StringVector& list) const;   // Clears vector before adding elements
        bool addLocation         (const std::string& location);

        bool writeAllEventTypes () const;
        bool readEventTypes     ();

        bool writeAllLocations  () const;
        bool readLocations      ();

    private:
        std::unordered_map<Event, EventPtr> events;
        std::map<std::string, std::string>  eventTypes;
        std::map<std::string, std::string>  locations;

        std::string             saveDirectory;
        LAS::Logging::LoggerPtr logger;

        static constexpr std::string_view EVENT_TYPES_FILENAME  {"EventTypes.json"};
        static constexpr std::string_view LOCATIONS_FILENAME    {"Locations.json"};

    };
    
    using EventTrackerPtr = std::shared_ptr<EventTracker>;

    namespace EventHelper{
        bool    writeEvent    (std::string directory, const Event& event);
        Event   readEvent     (const std::string& path);

        bool    writeAllEventTypes  (std::string path, const StringVector& eventTypes);
        bool    readEventTypes      (std::string path, StringVector& eventTypes);               // Clears vector before adding elements

        bool    writeAllLocations   (std::string path, const StringVector& locations);
        bool    readLocations       (std::string path, StringVector& locations);               // Clears vector before adding elements

        timepoint stringToTimepoint(const std::string& timeString);
    }

}