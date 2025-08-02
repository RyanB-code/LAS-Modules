#pragma once

#include "AssociatedItems.h"
#include "CommonItems.h"

#include <map>
#include <set>
#include <memory>
#include <exception>

namespace ShooterCentral {

    class Containers {
    public:
        Containers();
        ~Containers();

        const std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>&    getKnownAmmo()    const;
        const std::map<GunMetadata, std::shared_ptr<GunMetadata>>&      getKnownGuns()    const;
        const std::map<EventMetadata, std::shared_ptr<Event>>&          getEvents()       const;

        const std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>>&    getAmmoStockpile()    const;
        const std::map<Cartridge, std::map<GunMetadata,   std::shared_ptr<AssociatedGun>>>&     getGunsInArmory()     const;

        const std::set<Manufacturer>&   getManufacturers()    const;
        const std::set<Cartridge>&      getCartridges()       const;
        const std::set<Location>&       getLocations()        const;
        const std::set<EventType>&      getEventTypes()       const;
        const std::set<WeaponType>&     getWeaponTypes()      const;


        bool knownAmmo_add      (std::shared_ptr<AmmoMetadata> add);
        bool knownGuns_add      (std::shared_ptr<GunMetadata> add);
        bool events_add         (std::shared_ptr<Event> add);      
        bool ammoStockpile_add  (std::shared_ptr<AssociatedAmmo> add);
        bool gunsInArmory_add   (std::shared_ptr<AssociatedGun> add);

        bool manufacturers_add  (const Manufacturer& add);
        bool cartridges_add     (const Cartridge& add);
        bool eventTypes_add     (const EventType& add);
        bool weaponTypes_add    (const WeaponType& add);
        bool locations_add      (const Location& add);

        std::shared_ptr<AmmoMetadata>   knownAmmo_at    (const AmmoMetadata& key);
        std::shared_ptr<GunMetadata>    knownGuns_at    (const GunMetadata& key);
        std::shared_ptr<Event>          events_at       (const Event& key);
        std::shared_ptr<AssociatedAmmo> ammoStockpile_at(const AmmoMetadata& key);
        std::shared_ptr<AssociatedGun>  gunsInArmory_at (const GunMetadata& key);

        bool ammoStockpile_contains (const AmmoMetadata& key) const;
        bool gunsInArmory_contains  (const GunMetadata& key) const;

    private:
        std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>   knownAmmo;
        std::map<GunMetadata, std::shared_ptr<GunMetadata>>     knownGuns;
        std::map<EventMetadata, std::shared_ptr<Event>>         events;

        std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>> ammoStockpile;
        std::map<Cartridge, std::map<GunMetadata,   std::shared_ptr<AssociatedGun>>>  gunsInArmory;

        std::set<Manufacturer>  manufacturers;
        std::set<Cartridge>     cartridges;
        std::set<Location>      locations;
        std::set<EventType>     eventTypes;
        std::set<WeaponType>    weaponTypes;
    };

}
