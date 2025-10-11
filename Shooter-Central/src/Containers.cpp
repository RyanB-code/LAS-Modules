#include "Containers.h"

namespace ShooterCentral {

Containers::Containers(){

}
Containers::~Containers(){

}

const std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>& Containers::getKnownAmmo()    const{
    return knownAmmo;
}
const std::map<GunMetadata, std::shared_ptr<GunMetadata>>& Containers::getKnownGuns()    const{
    return knownGuns;
}
const std::map<ShootingEventMetadata, std::shared_ptr<ShootingEvent>>& Containers::getEvents()       const{
    return events;
}
const std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>>& Containers::getAmmoStockpile()    const{
    return ammoStockpile;
}
const std::map<Cartridge, std::map<GunMetadata,   std::shared_ptr<AssociatedGun>>>& Containers::getGunsInArmory()     const{
    return gunsInArmory;
}
const std::map<Cartridge, int>& Containers::getAmountPerCartridge() const{
    return amountPerCartridge;
}
const std::map<Manufacturer,  std::shared_ptr<Manufacturer>>& Containers::getManufacturers()    const{
    return manufacturers;
}
const std::map<Cartridge,     std::shared_ptr<Cartridge>>& Containers::getCartridges()       const{
    return cartridges;
}
const std::map<Location,      std::shared_ptr<Location>>& Containers::getLocations()        const{
    return locations;
}
const std::map<ShootingEventType,     std::shared_ptr<ShootingEventType>>& Containers::getEventTypes()       const{
    return eventTypes;
}
const std::map<WeaponType,    std::shared_ptr<WeaponType>>& Containers::getWeaponTypes()      const{
    return weaponTypes;
}
std::pair<std::shared_ptr<AmmoMetadata>, bool> Containers::knownAmmo_create      (const ObjectBuffers::AmmoMetadata& data){
    const Manufacturer manufacturer { data.manufacturer };
    const Cartridge cartridge       { data.cartridge };

    bool newManufacturer { false };

    if(!manufacturers.contains(manufacturer)) {
        if(!manufacturers_add(manufacturer))
            return std::pair{nullptr, false};
        else
            newManufacturer = true;
    }

    if(!cartridges.contains(cartridge)) {
        if(!cartridges_add(cartridge)){

            // Strong rollback guarantee erasure if  operator fails
            if(newManufacturer)
                manufacturers.erase(manufacturer);

            return std::pair{nullptr, false};
        }
    }

    const AmmoMetadata buffer { 
        .name           = data.name, 
        .grainWeight    = data.grainWeight,
        .isActive       = data.isActive,
        .manufacturer   = *manufacturers.at(manufacturer),
        .cartridge      = *cartridges.at(cartridge)
    };

    if(knownAmmo.contains(buffer))
        return std::pair{knownAmmo.at(buffer), true};

    std::pair result { knownAmmo.try_emplace(buffer, std::make_shared<AmmoMetadata>(buffer)) };
    return std::pair{ result.first->second, result.second };

}
std::pair<std::shared_ptr<GunMetadata>, bool> Containers::knownGuns_create      (const ObjectBuffers::GunMetadata& data){
    const WeaponType weaponType { data.weaponType };
    const Cartridge cartridge { data.cartridge };

    bool newWeaponType { false };

    if(!weaponTypes.contains(weaponType)) {
        if(!weaponTypes_add(weaponType))
            return std::pair{nullptr, false};
        else
            newWeaponType = true;
    }

    if(!cartridges.contains(cartridge)) {
        if(!cartridges_add(cartridge)){

            // Providing the strong rollback guarantee if operation fails
            if(newWeaponType)
                weaponTypes.erase(weaponType);

            return std::pair{nullptr, false};
        }
    }

    const GunMetadata buffer { 
        .name           = data.name, 
        .isActive       = data.isActive,
        .cartridge      = *cartridges.at(cartridge),
        .weaponType     = *weaponTypes.at(weaponType)
    };

    if(knownGuns.contains(buffer))
        return std::pair{knownGuns.at(buffer), true};

    std::pair result { knownGuns.try_emplace(buffer, std::make_shared<GunMetadata>(buffer)) };
    return std::pair { result.first->second, result.second };
}
std::pair<std::shared_ptr<ShootingEvent>, bool> Containers::events_create(const ObjectBuffers::ShootingEventMetadata& data){
    const Location  location      { data.location };
    const ShootingEventType eventType     { data.eventType };

    bool newItem { false };

    if(!locations.contains(location)) {
        if(!locations_add(location))
            return std::pair{nullptr, false};
        else
            newItem = true;
    }

    if(!eventTypes.contains(eventType)) {
        if(!eventTypes_add(eventType)){

            // Providing the strong rollback guarantee if operation fails
            if(newItem)
                locations.erase(location);

            return std::pair{nullptr, false};
        }
    }

    const ShootingEventMetadata buffer { 
        .notes          = data.notes,
        .date           = std::chrono::floor<std::chrono::days>(stringToTimepoint(data.date)),
        .location       = *locations.at(location), 
        .eventType      = *eventTypes.at(eventType)
    };

    if(events.contains(buffer))
        return std::pair{events.at(buffer), true};

    std::pair result { events.try_emplace(buffer, std::make_shared<ShootingEvent>(buffer)) };
    return std::pair { result.first->second, result.second };
}
bool Containers::ammoStockpile_add  (const AssociatedAmmo& add){
    const AmmoMetadata& info { add.getAmountOfAmmo().getAmmoInfo() };

    if(!ammoStockpile.contains(info.cartridge)){
        if(!ammoStockpile.try_emplace(info.cartridge).second)
            return false;
    }
    
    if(!ammoStockpile.at(info.cartridge).try_emplace(info, std::make_shared<AssociatedAmmo>(add)).second)
        return false;

    // Add to amount per cartridge list
    if(!addAmountPerCartridge(info.cartridge, add.getAmountOfAmmo().getAmount() )){
        ammoStockpile.at(info.cartridge).erase(info);
        return false;
    }
    
    return true;
}
bool Containers::gunsInArmory_add   (const AssociatedGun& add){
    const GunMetadata& info { add.getGunInfo() };

    if(gunsInArmory.contains(info.cartridge))
        return gunsInArmory.at(info.cartridge).try_emplace(info, std::make_shared<AssociatedGun>(add)).second;

    if(gunsInArmory.try_emplace(info.cartridge).second)
        return gunsInArmory.at(info.cartridge).try_emplace(info, std::make_shared<AssociatedGun>(add)).second;

    return false;
}
bool Containers::manufacturers_add  (const Manufacturer& add){
    return manufacturers.try_emplace(add, std::make_shared<Manufacturer>(add)).second;
}
bool Containers::cartridges_add     (const Cartridge& add){
    return cartridges.try_emplace(add, std::make_shared<Cartridge>(add)).second;
}
bool Containers::eventTypes_add     (const ShootingEventType& add){
    return eventTypes.try_emplace(add, std::make_shared<ShootingEventType>(add)).second;
}
bool Containers::weaponTypes_add    (const WeaponType& add){
    return weaponTypes.try_emplace(add, std::make_shared<WeaponType>(add)).second;
}
bool Containers::locations_add      (const Location& add){
    return locations.try_emplace(add, std::make_shared<Location>(add)).second;
}
std::shared_ptr<AmmoMetadata> Containers::knownAmmo_at    (const AmmoMetadata& key){
    return knownAmmo.at(key);
}
std::shared_ptr<GunMetadata> Containers::knownGuns_at    (const GunMetadata& key){
    return knownGuns.at(key);
}
std::shared_ptr<ShootingEvent> Containers::events_at       (const ShootingEvent& key){
    return events.at(key.getInfo());
}
std::shared_ptr<AssociatedAmmo> Containers::ammoStockpile_at(const AmmoMetadata& key){
    return ammoStockpile.at(key.cartridge).at(key);
}
std::shared_ptr<AssociatedGun> Containers::gunsInArmory_at (const GunMetadata& key) {
    return gunsInArmory.at(key.cartridge).at(key);
}
bool Containers::ammoStockpile_contains (const AmmoMetadata& key) const{
    if(!ammoStockpile.contains(key.cartridge))
        return false;

    return ammoStockpile.at(key.cartridge).contains(key);
}
bool Containers::gunsInArmory_contains  (const GunMetadata& key) const{
    if(!gunsInArmory.contains(key.cartridge))
        return false;

    return gunsInArmory.at(key.cartridge).contains(key);

}
int Containers::getAmountOfCartridge(const Cartridge& cartridge) const {
    if(!amountPerCartridge.contains(cartridge))
        return -1;

    return amountPerCartridge.at(cartridge);
}
bool Containers::addAmountPerCartridge(const Cartridge& cartridge, int addAmount) {
    if(!amountPerCartridge.contains(cartridge)){
        return amountPerCartridge.try_emplace(cartridge, addAmount).second;
    }

    amountPerCartridge.at(cartridge) += addAmount;

    return true;
}

namespace ModelEvents {

    AddGunMetadata::AddGunMetadata(const ObjectBuffers::GunMetadata& set) : info { set } {

    }
    Status AddGunMetadata::execute (Containers& container) {
        if(container.knownGuns_create(info).second)
            return Status { true };
        else
            return Status { false, "knownGuns_create() failed" };
    }

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
}   // End ModeEvents namespace

}   // End SC namespace
