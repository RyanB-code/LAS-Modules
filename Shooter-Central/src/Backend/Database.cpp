#include "Database.h"

namespace ShooterCentral {

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
bool Database::addToStockpile(const StockpileAmmo& data){
    const AmmoMetadata& info { data.getAmmoInfo() };

    if(!stockpile.contains(info.cartridge)){
        if(!stockpile.try_emplace(info.cartridge).second)
            return false;
    }
    
    if(!stockpile.at(info.cartridge).try_emplace(info, data).second)
        return false;

    // Add to amount per cartridge list
    if(!addAmountPerCartridge(info.cartridge, data.getAmountOnHand() )){
        stockpile.at(info.cartridge).erase(info);
        return false;
    }

    return true;
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



void ShooterCentral::associateEvents(Database& db){
    for(const auto& [eventInfo, event] : db.getEvents()){
        addMetadataInfo(db, eventInfo);

        for(const auto& gunTrackingAmmo : event.getGunsUsed() ){
            const GunMetadata& gunInfo { gunTrackingAmmo.getGunInfo() };

            // Add gun to armory
            if(!db.armoryContains(gunInfo)){
                if(!db.addToArmory(gunInfo)){
                    LAS::log_error(std::format("associateEvents(), attempt to add gun '{}' failed", gunInfo.name)); 
                    continue;
                }
            }
            
            try{
                ArmoryGun& gun { db.getGun(gunInfo) };
                
                addMetadataInfo(db, gunInfo);

                if(!gun.addEvent(event))
                    LAS::log_error(std::format("associateEvents(), attempt to add Event on {} to gun '{}' failed", event.printDate(), gunInfo.name));

                // For every ammo type used, add metadata info, add to stockpile (w/ amount 0), and add Gun to that StockpileAmmo entry
                for(const auto& amountOfAmmo : gunTrackingAmmo.getAmmoUsed() ){
                    const AmmoMetadata& ammoInfo { amountOfAmmo.getAmmoInfo() };

                    addMetadataInfo(db, ammoInfo);

                    // Associate ammoInfo to gun used
                    if(!gun.addAmmoUsed(amountOfAmmo))
                        LAS::log_error(std::format("associateEvents(), attempt to add AmmountOfAmmo '{}' to gun '{}' failed", ammoInfo.name, gunInfo.name));

                    // Add ammoInfo to stockpile
                    if(!db.stockpileContains(ammoInfo)){
                        if(!db.addToStockpile(ammoInfo))
                            LAS::log_error(std::format("associateEvents(), attempt to add Ammo '{}' to stockpile failed", ammoInfo.name)); 
                            continue;
                    } 

                    // Associate stockpile ammo with gun
                    StockpileAmmo& stockpileAmmo { db.getAmmo(ammoInfo) };
                    if(stockpileAmmo.hasGun(gunInfo))
                        continue;
                    if(!stockpileAmmo.addGun(gunInfo))
                        LAS::log_error(std::format("associateEvents(), attempt to add Gun '{}' to Ammo '{}' failed", gunInfo.name, ammoInfo.name)); 

                }   // End adding of all Ammo for the Gun
            }   // End Gun selection
            catch(std::out_of_range& e){
                LAS::log_error(std::format("std::out_of_range from associateEvents(). What: {}", e.what()));
                continue;
            }

        }   // End adding of all Guns for the Event
    }   // End adding of all Events
}




void ShooterCentral::addMetadataInfo(Database& db, const GunMetadata& info){
    if(!db.metadataContains(info.cartridge)){
        if(!db.addCartridge(info.cartridge))
            LAS::log_warn(std::format("addMetadataInfo() could not add Cartridge {}", info.cartridge.getName()) );
    }

    if(!db.metadataContains(info.weaponType)){
        if(!db.addWeaponType(info.weaponType))
            LAS::log_warn(std::format("addMetadataInfo() could not add WeaponType {}", info.weaponType.getName() ) );
    }
}
void ShooterCentral::addMetadataInfo(Database& db, const AmmoMetadata& info){
    if(!db.metadataContains(info.manufacturer)){
        if(!db.addManufacturer(info.manufacturer))
            LAS::log_warn(std::format("addMetadataInfo() could not add Manufacturer {}", info.manufacturer.getName()) );
    }

    if(!db.metadataContains(info.cartridge)){
        if(!db.addCartridge(info.cartridge))
            LAS::log_warn(std::format("addMetadataInfo() could not add Cartridge {}", info.cartridge.getName()) );
    }

}
void ShooterCentral::addMetadataInfo(Database& db, const ShootingEventMetadata& info){
    if(!db.metadataContains(info.location)){
        if(!db.addLocation(info.location))
            LAS::log_warn(std::format("addMetadataInfo() could not add Location {}", info.location.getName()) );
    }

    if(!db.metadataContains(info.eventType)){
        if(!db.addEventType(info.eventType))
            LAS::log_warn(std::format("addMetadataInfo() could not add EventType {}", info.eventType.getName()) );
    }
}

}   // End SC namespace
