#pragma once

#include "AssociatedItems.h"
#include "CommonItems.h"

#include <map>
#include <set>
#include <memory>
#include <exception>

namespace ShooterCentral {

    class Model {
    public:
        Model();
        ~Model();

        std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>::const_iterator   knownAmmo_cbegin() const;
        std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>::const_iterator   knownAmmo_cend() const;   
        
        std::map<GunMetadata, std::shared_ptr<GunMetadata>>::const_iterator     knownGuns_cbegin() const;
        std::map<GunMetadata, std::shared_ptr<GunMetadata>>::const_iterator     knownGuns_cend() const;

        std::map<AmmoMetadata,  AssociatedAmmo>::const_iterator                 ammoStockpile_cbegin() const;
        std::map<AmmoMetadata,  AssociatedAmmo>::const_iterator                 ammoStockpile_cend() const;

        std::map<GunMetadata,   AssociatedGun>::const_iterator                  gunsInArmory_cbegin() const;
        std::map<GunMetadata,   AssociatedGun>::const_iterator                  gunsInArmory_cend() const;

        std::map<Event, std::shared_ptr<Event>>::const_iterator                 events_cbegin() const;
        std::map<Event, std::shared_ptr<Event>>::const_iterator                 events_cend() const;


        std::set<Manufacturer>::const_iterator      manufacturers_cbegin() const;
        std::set<Manufacturer>::const_iterator      manufacturers_cend() const;

        std::set<Cartridge>::const_iterator         cartridges_cbegin() const;
        std::set<Cartridge>::const_iterator         cartridges_cend() const;

        std::set<Location>::const_iterator          locations_cbegin() const;
        std::set<Location>::const_iterator          locations_cend() const;

        std::set<EventType>::const_iterator         eventTypes_cbegin() const;
        std::set<EventType>::const_iterator         eventTypes_cend() const;

        std::set<WeaponType>::const_iterator        weaponTypes_cbegin() const;
        std::set<WeaponType>::const_iterator        weaponTypes_cend() const;


        bool knownAmmo_contains     (const AmmoMetadata& key) const;
        bool knownGuns_contains     (const GunMetadata& key) const;
        bool ammoStockpile_contains (const AmmoMetadata& key) const;
        bool gunsInArmory_contains  (const GunMetadata& key) const;
        bool events_contains        (const Event& key) const;

        bool manufacturers_contains (const Manufacturer& key) const;
        bool cartridges_contains    (const Cartridge& key) const;
        bool locations_contains     (const Location& key) const;
        bool eventTypes_contains    (const EventType& key) const;
        bool weaponTypes_contains   (const WeaponType& key) const;


        std::shared_ptr<AmmoMetadata>   knownAmmo_at    (const AmmoMetadata& key);
        std::shared_ptr<GunMetadata>    knownGuns_at    (const GunMetadata& key);
        std::shared_ptr<Event>          events_at       (const Event& key);
        AssociatedAmmo&                 ammoStockpile_at(const AmmoMetadata& key);
        AssociatedGun&                  gunsInArmory_at (const GunMetadata& key);


        bool knownAmmo_add      (std::shared_ptr<AmmoMetadata> add);
        bool knownGuns_add      (std::shared_ptr<GunMetadata> add);
        bool events_add         (std::shared_ptr<Event> add);
        bool ammoStockpile_add  (const AssociatedAmmo& add);
        bool gunsInArmory_add   (const AssociatedGun& add);

        bool manufacturers_add  (const Manufacturer& add);
        bool cartridges_add     (const Cartridge& add);
        bool eventTypes_add     (const EventType& add);
        bool weaponTypes_add    (const WeaponType& add);
        bool locations_add      (const Location& add);

    private:
        std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>   knownAmmo;
        std::map<GunMetadata, std::shared_ptr<GunMetadata>>     knownGuns;
        std::map<Event, std::shared_ptr<Event>>                 events;

        std::map<AmmoMetadata,  AssociatedAmmo> ammoStockpile;
        std::map<GunMetadata,   AssociatedGun>  gunsInArmory;

        std::set<Manufacturer>  manufacturers;
        std::set<Cartridge>     cartridges;
        std::set<Location>      locations;
        std::set<EventType>     eventTypes;
        std::set<WeaponType>    weaponTypes;
    };

}
