#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"

#include <map>
#include <memory>
#include <exception>

namespace ShooterCentral {

    
    class AssociatedAmmo {
    public:
        AssociatedAmmo(AmountOfAmmo setAmountofAmmo);
        ~AssociatedAmmo();

        const AmountOfAmmo& getAmountOfAmmo() const;
        operator bool() const;

        bool addGun     (std::shared_ptr<const GunMetadata> gun); // If key already exists, returns false since data is not overwritten
        bool removeGun  (const GunMetadata& gun);
        bool hasGun     (const GunMetadata& gun) const;

        int totalGuns   () const;

        std::map<GunMetadata, std::shared_ptr<const GunMetadata>>::const_iterator cbegin() const;     // Const itr to cbegin()
        std::map<GunMetadata, std::shared_ptr<const GunMetadata>>::const_iterator cend() const;       // Const itr to cend()
   private:
        AmountOfAmmo                                                amountOfAmmo      { };
        std::map<GunMetadata, std::shared_ptr<const GunMetadata>>   gunsAssociated    { };

        void throwIfInvalid() const;
    };


    class AssociatedGun {
    public:
        AssociatedGun(std::shared_ptr<const GunMetadata> setGun);
        ~AssociatedGun();

        const GunMetadata& getGun() const;
        operator bool() const;

        int getRoundCount()         const;
        int totalEventsUsed()       const;
        int totalAmmoTypesUsed()    const;

        bool addAmmoUsed    (const AmountOfAmmo& ammo);
        bool removeAmmoUsed (const AmmoMetadata& ammo);
        bool hasUsedAmmo    (const AmmoMetadata& ammo) const;

        bool addEvent       (std::shared_ptr<Event> event); // If key already exists, returns false since data is not overwritten. Iterates over the event and adds all ammo to the gun
        bool removeEvent    (const EventMetadata& data); 
        bool wasUsedInEvent (const EventMetadata& data);

        const std::map<AmmoMetadata, AmountOfAmmo>&                     getAmmoUsed()   const;
        const std::map<EventMetadata, std::shared_ptr<Event>>&    getEventsUsed() const;
    private:
        int totalRoundCount { 0 };

        std::shared_ptr<const GunMetadata>                      gun           { };
        std::map<AmmoMetadata, AmountOfAmmo>                    ammoUsedList  { };    // Not shared_ptr because the amount is amount of rounds shot, not the amount in the stockpile 
        std::map<EventMetadata, std::shared_ptr<Event>>   eventsUsed    { };

        void throwIfGunInvalid() const;
    };

}
