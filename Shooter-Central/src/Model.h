#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"

#include <map>
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


        bool knownAmmo_contains     (const AmmoMetadata& key) const;
        bool knownGuns_contains     (const GunMetadata& key) const;
        bool ammoStockpile_contains (const AmmoMetadata& key) const;
        bool gunsInArmory_contains  (const GunMetadata& key) const;
        bool events_contains        (const Event& key) const;

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

    private:
        std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>   knownAmmo;
        std::map<GunMetadata, std::shared_ptr<GunMetadata>>     knownGuns;
        std::map<Event, std::shared_ptr<Event>>                 events;

        std::map<AmmoMetadata,  AssociatedAmmo> ammoStockpile;
        std::map<GunMetadata,   AssociatedGun>  gunsInArmory;
    };

}
