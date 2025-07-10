#pragma once

#include "AssociatedItems.h"

namespace ShooterCentral {

    struct ContainerItrs {
        std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>::const_iterator   knownAmmo_cbegin;
        std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>::const_iterator   knownAmmo_cend;   
        
        std::map<GunMetadata, std::shared_ptr<GunMetadata>>::const_iterator     knownGuns_cbegin;
        std::map<GunMetadata, std::shared_ptr<GunMetadata>>::const_iterator     knownGuns_cend;

        std::map<AmmoMetadata,  AssociatedAmmo>::const_iterator                 ammoStockpile_cbegin;
        std::map<AmmoMetadata,  AssociatedAmmo>::const_iterator                 ammoStockpile_cend;

        std::map<GunMetadata,   AssociatedGun>::const_iterator                  gunsInArmory_cbegin;
        std::map<GunMetadata,   AssociatedGun>::const_iterator                  gunsInArmory_cend;

        std::map<Event, std::shared_ptr<Event>>::const_iterator                 events_cbegin;
        std::map<Event, std::shared_ptr<Event>>::const_iterator                 events_cend;
    };

    struct UnsavedChanges {
        bool guns   { false };
        bool ammo   { false };
        bool events { false };
    };
}
