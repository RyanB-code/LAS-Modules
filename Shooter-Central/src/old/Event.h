#pragma once

#include "Gun.h"
#include "Ammo.h"

#include <stdint.h>
#include <string>
#include <memory>
#include <map>
#include <array>
#include <chrono>
#include <tuple>

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

    std::ostream& operator<<(std::ostream& os, const Location& location);

    using LocationList = std::vector<Location>;

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

    std::ostream& operator<<(std::ostream& os, const EventType& eventType);

    using EventTypeList = std::vector<EventType>;




    class Event final {
    public:
        Event();
        Event(Location setLocation, EventType setEventType, std::string setNotes, ymd setDate);
        ~Event();

        std::string getName()       const;
        std::string getNotes()      const;
        EventType   getEventType()  const;
        Location    getLocation()   const;
        const ymd&  getDate()       const;
        timepoint   getTimepoint()  const;
        
        std::string printDate()     const;

        bool    addGun          (Gun gun, TrackedAmmo ammoUsed);
        void    getAllGunsUsed  (std::vector<std::pair<ConstGunPtr, ConstTrackedAmmoPtr>>& list) const; // Clears vector before adding elements 
        int     getNumGunsUsed  () const;

        bool operator== (const Event& other) const;
        bool operator<  (const Event& other) const;


    private:
        static constexpr int MAX_GUNS_PER_EVENT  { 5 };

        Location    location;
        EventType   eventType;
        std::string notes;
        ymd         date;

        std::array<std::pair<Gun, TrackedAmmo>, MAX_GUNS_PER_EVENT>  gunsUsed;
    };
    using EventPtr = std::shared_ptr<Event>;

    void to_json    (LAS::json& j, const Event& event);
    void from_json  (const LAS::json& j, Event& event); 


    
    class EventTracker {
    public:
        EventTracker();
        ~EventTracker();

        bool        setDirectory    (std::string directory);
        std::string getDirectory    () const;

        bool addEvent           (Event& event);
        bool addEventType       (const EventType& type);
        bool addLocation        (const Location& location);

        bool contains           (const Event& event) const;

        void getAllEvents       (std::vector<EventPtr>& list)       const;  // Clears vector before adding elements
        void getAllEventTypes   (EventTypeList& list)               const;  // Clears vector before adding elements
        void getAllLocations    (LocationList& list)                const;  // Clears vector before adding elements
        int  getTotalEvents     ()                                  const;


        bool writeAllEvents     () const;
        bool writeAllEventTypes () const;
        bool writeAllLocations  () const;

        bool readEvents         ();
        bool readEventTypes     ();
        bool readLocations      ();

    private:
        std::map<Event, EventPtr> events;
        std::map<std::string, EventType>  eventTypes;
        std::map<std::string, Location>  locations;

        std::string             saveDirectory;

        static constexpr std::string_view EVENT_TYPES_FILENAME  {"EventTypes.json"};
        static constexpr std::string_view LOCATIONS_FILENAME    {"Locations.json"};

    };
    
    using EventTrackerPtr = std::shared_ptr<EventTracker>;

    namespace EventHelper{
        std::string makeFileName     (std::string directory, const Event& event);           // Makes proper file name for an Event

        bool    writeEvent          (std::string directory, const Event&            event);
        bool    writeAllLocations   (std::string path,      const LocationList&     list);
        bool    writeAllEventTypes  (std::string path,      const EventTypeList&    list);

        Event   readEvent           (const std::string& path);
        bool    readEventTypes      (std::string path, EventTypeList&   list);               // Clears vector before adding elements
        bool    readLocations       (std::string path, LocationList&    list);               // Clears vector before adding elements

        timepoint stringToTimepoint(const std::string& timeString);
    }

}
