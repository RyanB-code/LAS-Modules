#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"

#include <map>
#include <memory>

namespace ShooterCentral {
    // Forward declarations
    class Event;

    class AssociatedAmmo {
    public:
        AssociatedAmmo(std::shared_ptr<AmountOfAmmo> setAmountofAmmo);
        ~AssociatedAmmo();

        bool addGun     (std::shared_ptr<Gun> gun); // If key already exists, returns false since data is not overwritten
        bool removeGun  (const Gun& gun);
        bool hasGun     (const Gun& gun) const;

        int totalGuns   () const;

        const AmountOfAmmo& getAmountOfAmmo() const;

        std::map<Gun, std::shared_ptr<Gun>>::const_iterator begin() const;     // Const itr to cbegin()
        std::map<Gun, std::shared_ptr<Gun>>::const_iterator end() const;       // Const itr to cend()
   private:
        std::shared_ptr<AmountOfAmmo>       amountOfAmmo      { };
        std::map<Gun, std::shared_ptr<Gun>> gunsAssociated    { };
    };


    class AssociatedGun {
    public:
        AssociatedGun(std::shared_ptr<Gun> setGun);
        ~AssociatedGun();

        int getRoundCount() const;

        bool addAmmoUsed    (const AmountOfAmmo& ammo);
        bool removeAmmoUsed (const Ammo& ammo);
        bool hasUsedAmmo    (const Ammo& ammo) const;

        bool addEvent       (std::shared_ptr<Event> event);
        //bool removeEvent    (const Event& event); 
        //bool wasUsedInEvent (const Event& event);


    private:
        int totalRoundCount { 0 };

        std::shared_ptr<Gun>                    gun                 { };
        std::map<Ammo, AmountOfAmmo>            ammoUsedList        { };    // Not shared_ptr because the amount is amount of rounds shot, not in stockpile 
        std::map<Event, std::shared_ptr<Event>> eventsAssociated    { };
    };

}
