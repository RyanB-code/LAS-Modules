#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"

#include <map>
#include <memory>
#include <exception>

namespace ShooterCentral {

    class Event {
    private:
        static constexpr int MAX_NUM_GUNS { 5 };
    public:
        Event(Location setLocation, EventType setEventType, std::string setNotes, ymd setDate);
        ~Event();

        bool    addGun          (const GunAndAmmo& gun);        // Will not amend existing entries if a gun is already in the container
        bool    hasUsedGun      (const Gun& ammo) const;
        int     totalGunsUsed   () const;

        std::string getNotes()      const;
        EventType   getEventType()  const;
        Location    getLocation()   const;
        const ymd&  getDate()       const;
        timepoint   getTimepoint()  const;

        std::string printDate()     const;

        std::array<GunAndAmmo, MAX_NUM_GUNS>::const_iterator cbegin() const;
        std::array<GunAndAmmo, MAX_NUM_GUNS>::const_iterator cend() const;

        bool operator== (const Event& other) const;
        bool operator<  (const Event& other) const;
   private:
        EventMetadata eventMetadata;

        std::array<GunAndAmmo, MAX_NUM_GUNS> gunsUsedList;
        int nextIndex { 0 };
    };

    class AssociatedAmmo {
    public:
        AssociatedAmmo(std::shared_ptr<const AmountOfAmmo> setAmountofAmmo);
        ~AssociatedAmmo();

        bool addGun     (std::shared_ptr<const Gun> gun); // If key already exists, returns false since data is not overwritten
        bool removeGun  (const Gun& gun);
        bool hasGun     (const Gun& gun) const;

        int totalGuns   () const;

        const AmountOfAmmo& getAmountOfAmmo() const;

        std::map<Gun, std::shared_ptr<const Gun>>::const_iterator cbegin() const;     // Const itr to cbegin()
        std::map<Gun, std::shared_ptr<const Gun>>::const_iterator cend() const;       // Const itr to cend()
   private:
        std::shared_ptr<const AmountOfAmmo>       amountOfAmmo      { };
        std::map<Gun, std::shared_ptr<const Gun>> gunsAssociated    { };
    };


    class AssociatedGun {
    public:
        AssociatedGun(std::shared_ptr<const Gun> setGun);
        ~AssociatedGun();

        const Gun& getGun() const;
        operator bool() const;

        int getRoundCount() const;
        int totalEventsUsed () const;

        bool addAmmoUsed    (const AmountOfAmmo& ammo);
        bool removeAmmoUsed (const Ammo& ammo);
        bool hasUsedAmmo    (const Ammo& ammo) const;

        bool addEvent       (std::shared_ptr<const Event> event); // If key already exists, returns false since data is not overwritten
        bool removeEvent    (const Event& event); 
        bool wasUsedInEvent (const Event& event);

        std::map<Ammo, AmountOfAmmo>::const_iterator    ammoUsed_cbegin() const;
        std::map<Ammo, AmountOfAmmo>::const_iterator    ammoUsed_cend() const;       

        std::map<Event, std::shared_ptr<const Event>>::const_iterator   eventsUsed_cbegin() const;
        std::map<Event, std::shared_ptr<const Event>>::const_iterator   eventsUsed_cend() const;
    private:
        int totalRoundCount { 0 };

        std::shared_ptr<const Gun>                      gun           { };
        std::map<Ammo, AmountOfAmmo>                    ammoUsedList  { };    // Not shared_ptr because the amount is amount of rounds shot, not the amount in the stockpile 
        std::map<Event, std::shared_ptr<const Event>>   eventsUsed    { };

        void throwIfGunInvalid() const;
    };

}
