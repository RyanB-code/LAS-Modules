#include "Database.h"

using namespace ShooterCentral;

Database::Database() {

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

