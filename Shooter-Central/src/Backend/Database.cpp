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
std::pair<EventIterator, bool> Database::addEvent(const ShootingEvent& event){
    if(events.contains(event.getInfo()))
        return std::pair{events.find(event.getInfo()), false};

    std::pair result { events.try_emplace(event.getInfo(), event) };
    return std::pair { result.first, result.second };
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
