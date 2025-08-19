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

        bool addGun     (std::shared_ptr<const GunMetadata> gun); // If key already exists, returns false since data is not overwritten
        bool removeGun  (const GunMetadata& gun);
        bool hasGun     (const GunMetadata& gun) const;

        int totalGuns   () const;

        const std::map<GunMetadata, std::shared_ptr<const GunMetadata>>& getGunsUsed() const;
   private:
        AmountOfAmmo                                                amountOfAmmo;    
        std::map<GunMetadata, std::shared_ptr<const GunMetadata>>   gunsAssociated    { };
    };


    class AssociatedGun {
    public:
        AssociatedGun(const GunMetadata& setGun);
        ~AssociatedGun();

        const GunMetadata& getGunInfo() const;

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
        const std::map<EventMetadata, std::shared_ptr<const Event>>&    getEventsUsed() const;
    private:
        int totalRoundCount { 0 };

        const GunMetadata&                                      gun;
        std::map<AmmoMetadata, AmountOfAmmo>                    ammoUsedList  { };    // Not shared_ptr because the amount is amount of rounds shot, not the amount in the stockpile 
        std::map<EventMetadata, std::shared_ptr<const Event>>   eventsUsed    { };
    };

}
