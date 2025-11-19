#include "Database.h"

using namespace ShooterCentral;

Database::Database() {

}
bool Database::addEvent(const ShootingEvent& event){
    const ShootingEventMetadata& info { event.getInfo() };

    if(events.contains(info))
       return true; 

    return events.try_emplace(info, event).second;
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

    return armory.at(info.cartridge).try_emplace(info, gun).second;
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
int Database::amountInStockpile   (const Cartridge& cartridge) const{
    if(!amountPerCartridge.contains(cartridge))
        return 0;

    return amountPerCartridge.at(cartridge);
}
bool Database::metadataContains   (const Manufacturer& man) const{
    return manufacturers.contains(man);
}
bool Database::metadataContains   (const Cartridge& cart) const{
    return cartridges.contains(cart);
}
bool Database::metadataContains   (const WeaponType& wt) const{
    return weaponTypes.contains(wt);
}
bool Database::metadataContains   (const Location& loc) const{
    return locations.contains(loc);
}
bool Database::metadataContains   (const ShootingEventType& et) const{
    return eventTypes.contains(et);
}


int Database::amountInStockpile   (const AmmoMetadata& info) const{
    const Cartridge& cartridge { info.cartridge };

    if(!stockpile.contains(cartridge))
        return 0;

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





void addMetadataInfo(Database& database, const GunMetadata& info){
    if(!database.addCartridge(info.cartridge))
        LAS::log_warn(std::format("addMetadataInfo() could not add Cartridge {}", info.cartridge.getName()) );

    if(!database.addWeaponType(info.weaponType))
        LAS::log_warn(std::format("addMetadataInfo() could not add WeaponType {}", info.weaponType.getName() ) );
}
void addMetadataInfo(Database& database, const AmmoMetadata& info){
     if(!database.addManufacturer(info.manufacturer))
        LAS::log_warn(std::format("addMetadataInfo() could not add Manufacturer {}", info.manufacturer.getName()) );

    if(!database.addCartridge(info.cartridge))
        LAS::log_warn(std::format("addMetadataInfo() could not add Cartridge {}", info.cartridge.getName()) );
}
void addMetadataInfo(Database& database, const ShootingEventMetadata& info){
    if(!database.addLocation(info.location))
        LAS::log_warn(std::format("addMetadataInfo() could not add Location {}", info.location.getName()) );
    if(!database.addEventType(info.eventType))
        LAS::log_warn(std::format("addMetadataInfo() could not add EventType {}", info.eventType.getName()) );
}


