#include "Database.h"

using namespace ShooterCentral;

Database::Database() {

}
bool Database::addEvent(const ShootingEvent& event){
    const ShootingEventMetadata& info { event.getInfo() };

    if(events.contains(info))
       true; 


    if(events.try_emplace(info, event).second){
        addMetadataInfo(info);
        return true;
    }

    return false;
}
bool Database::addToStockpile     (const AmountOfAmmo& amountOfAmmo){
    const AmmoMetadata& info { amountOfAmmo.getAmmoInfo() };

    if(!stockpile.contains(info.cartridge)){
        if(!stockpile.try_emplace(info.cartridge).second)
            return false;
    }
    
    if(!stockpile.at(info.cartridge).try_emplace(info, StockpileAmmo{amountOfAmmo}).second)
        return false;

    // Add to amount per cartridge list
    if(!addAmountPerCartridge(info.cartridge, amountOfAmmo.getAmount() )){
        stockpile.at(info.cartridge).erase(info);
        return false;
    }

    addMetadataInfo(amountOfAmmo.getAmmoInfo());
    return true;

}
bool Database::addToStockpile(const AmmoMetadata& info) {
    return addToStockpile( AmountOfAmmo { info } );
}
bool Database::addToArmory(const ArmoryGun& gun) {
    const GunMetadata& info { gun.getGunInfo() };

    if(!armory.contains(info.cartridge)){
        if(!armory.try_emplace(info.cartridge).second)
            return false;
    }

    if(armory.at(info.cartridge).try_emplace(info, gun).second){
        addMetadataInfo(gun.getGunInfo());
        return true;
    }

    return false;
}
bool Database::addToArmory(const GunMetadata& info) {
    return addToArmory( ArmoryGun { info } );
}
ShootingEvent& Database::getEvent(const ShootingEventMetadata& info) {
    return events.at(info);
}
StockpileAmmo& Database::getAmmo   (const AmmoMetadata& info) {
    if(!stockpile.contains(info.cartridge))
        throw std::out_of_range{"test"};

    return stockpile.at(info.cartridge).at(info);
}
ArmoryGun& Database::getGun (const GunMetadata& info) {
    if(!armory.contains(info.cartridge))
       throw std::out_of_range{"test"};

    return armory.at(info.cartridge).at(info);
}
bool Database::addManufacturer  (const Manufacturer& add){
    return manufacturers.emplace(add).second;
}
bool Database::addCartridge(const Cartridge& add){
    return cartridges.emplace(add).second;
}
bool Database::addWeaponType  (const WeaponType& add){
    return weaponTypes.emplace(add).second;
}
bool Database::addLocation  (const Location& add){
    return locations.emplace(add).second;
}
bool Database::addEventType  (const ShootingEventType& add){
    return eventTypes.emplace(add).second;
}
bool Database::armoryContains(const GunMetadata& info) const {
    if(!armory.contains(info.cartridge))
        return false;

    return armory.at(info.cartridge).contains(info);
}
bool Database::stockpileContains (const AmmoMetadata& info) const {
    if(!stockpile.contains(info.cartridge))
        return false;

    return stockpile.at(info.cartridge).contains(info);
}
int Database::amountInStockpile   (const Cartridge& cartridge){
    if(!amountPerCartridge.contains(cartridge))
        return 0;

    return amountPerCartridge.at(cartridge);
}
int Database::amountInStockpile   (const AmmoMetadata& info){
    const Cartridge& cartridge { info.cartridge };

    if(!stockpile.contains(cartridge))
        return false;

    const auto& cartridgeMap { stockpile.at(cartridge) };

    return cartridgeMap.at(info).getAmountOnHand();
}
bool Database::addAmountPerCartridge(const Cartridge& cartridge, int addAmount) {
    if(!amountPerCartridge.contains(cartridge)){
        return amountPerCartridge.try_emplace(cartridge, addAmount).second;
    }

    amountPerCartridge.at(cartridge) += addAmount;

    return true;
}
void Database::addMetadataInfo(const GunMetadata& info){
   cartridges.emplace(info.cartridge);
   weaponTypes.emplace(info.weaponType);
}
void Database::addMetadataInfo(const AmmoMetadata& info){
    manufacturers.emplace(info.manufacturer);
    cartridges.emplace(info.cartridge);
}
void Database::addMetadataInfo(const ShootingEventMetadata& info){
    locations.emplace(info.location);
    eventTypes.emplace(info.eventType);
}


