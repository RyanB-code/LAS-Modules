#include "Containers.h"

using namespace ShooterCentral;

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
const std::map<EventMetadata, std::shared_ptr<Event>>& Containers::getEvents()       const{
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
const std::set<Manufacturer>& Containers::getManufacturers()    const{
    return manufacturers;
}
const std::set<Cartridge>& Containers::getCartridges()       const{
    return cartridges;
}
const std::set<Location>& Containers::getLocations()        const{
    return locations;
}
const std::set<EventType>& Containers::getEventTypes()       const{
    return eventTypes;
}
const std::set<WeaponType>& Containers::getWeaponTypes()      const{
    return weaponTypes;
}
bool Containers::knownAmmo_add      (std::shared_ptr<AmmoMetadata> add){
    if(!add)
        return false;

    return knownAmmo.try_emplace(*add, add).second;
}
bool Containers::knownGuns_add      (std::shared_ptr<GunMetadata> add){
    if(!add)
        return false;

    return knownGuns.try_emplace(*add, add).second;
}
bool Containers::events_add(std::shared_ptr<Event> add){
    if(!add)
        return false;

    return events.try_emplace(add->getInfo(), add).second;
}       
bool Containers::ammoStockpile_add  (std::shared_ptr<AssociatedAmmo> add){
    if(!add)
        return false;

    if(!*add)           // Checks to see if Assoc item is pointing to a valid object
        return false;

    const AmmoMetadata& info { add->getAmountOfAmmo().getAmmo() };

    if(!ammoStockpile.contains(info.cartridge)){
        if(!ammoStockpile.try_emplace(info.cartridge).second)
            return false;
    }
    
    if(!ammoStockpile.at(info.cartridge).try_emplace(info, add).second)
        return false;

    // Add to amount per cartridge list
    if(!addAmountPerCartridge(info.cartridge, add->getAmountOfAmmo().getAmount() )){
        ammoStockpile.at(info.cartridge).erase(info);
        return false;
    }
    
    return true;
}
bool Containers::gunsInArmory_add   (std::shared_ptr<AssociatedGun> add){
    if(!add)
        return false;

    if(!*add)           // Checks to see if Assoc item is pointing to a valid objects
        return false;

    const GunMetadata& info { add->getGun() };

    if(gunsInArmory.contains(info.cartridge))
        return gunsInArmory.at(info.cartridge).try_emplace(info, add).second;

    if(gunsInArmory.try_emplace(info.cartridge).second)
        return gunsInArmory.at(info.cartridge).try_emplace(info, add).second;

    return false;
}
bool Containers::manufacturers_add  (const Manufacturer& add){
    return manufacturers.emplace(add).second;
}
bool Containers::cartridges_add     (const Cartridge& add){
    return cartridges.emplace(add).second;
}
bool Containers::eventTypes_add     (const EventType& add){
    return eventTypes.emplace(add).second;
}
bool Containers::weaponTypes_add    (const WeaponType& add){
    return weaponTypes.emplace(add).second;
}
bool Containers::locations_add      (const Location& add){
    return locations.emplace(add).second;
}
std::shared_ptr<AmmoMetadata> Containers::knownAmmo_at    (const AmmoMetadata& key){
    return knownAmmo.at(key);
}
std::shared_ptr<GunMetadata> Containers::knownGuns_at    (const GunMetadata& key){
    return knownGuns.at(key);
}
std::shared_ptr<Event> Containers::events_at       (const Event& key){
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
