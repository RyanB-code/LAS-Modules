#include "DatabaseEvents.h"

namespace ShooterCentral {

namespace ModelEvents {

/* / Full objects
CreateGun::CreateGun(const ObjectBuffers::GunMetadata& set) : info { set } {

}
Status CreateGun::execute (Containers& container) {
    if(container.knownGuns_create(info).second)
        return Status { true };

    return Status { false, "Failed to create Gun object and add to database" };
}
CreateAmmo::CreateAmmo(const ObjectBuffers::AmmoMetadata& set) : info { set } {

}
Status CreateAmmo::execute (Containers& container) {
    if(container.knownAmmo_create(info).second)
        return Status { true };

    return Status { false, "Failed to create Ammo object and add to database" };
}
CreateEvent::CreateEvent(const ObjectBuffers::ShootingEventMetadata& set) : info { set } {

}
Status CreateEvent::execute (Containers& container) {
    if(container.events_create(info).second)
        return Status { true };

    return Status { false, "Failed to create Event object and add to database" };
}

// Metadata items
AddManufacturer::AddManufacturer(const Manufacturer& m) : manufacturer { m } {

}
Status AddManufacturer::execute (Containers& container) {
    if(container.manufacturers_add(manufacturer))
        return Status{true};

    return Status{false, "Failed to add Manufacturer" };
}
AddCartridge::AddCartridge(const Cartridge& c) : cartridge { c } {

}
Status AddCartridge::execute (Containers& container) {
    if(container.cartridges_add(cartridge))
        return Status{true};

    return Status{false, "Failed to add Cartridge" };
}
AddEventType::AddEventType(const ShootingEventType& et) : eventType { et } {

}
Status AddEventType::execute (Containers& container) {
    if(container.eventTypes_add(eventType))
        return Status{true};

    return Status{false, "Failed to add ShootingEventType" };
}
AddWeaponType::AddWeaponType(const WeaponType& wt) : weaponType { wt } {
    
}
Status AddWeaponType::execute (Containers& container) {
    if(container.weaponTypes_add(weaponType))
        return Status{true};

    return Status{false, "Failed to add WeaponType" };
}

AddLocation::AddLocation(const Location& loc) : location { loc } {
    
}
Status AddLocation::execute (Containers& container) {
    if(container.locations_add(location))
        return Status{true};

    return Status{false, "Failed to add Location" };
}

*/

}   // End ModeEvents namespace



}   // End SC namespace
