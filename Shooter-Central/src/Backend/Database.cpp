#include "Database.h"

namespace ShooterCentral {

VerifyEventFlags::VerifyEventFlags() : 
    locationInvalid     { false },
    eventTypeInvalid    { false },
    dateInvalid         { false },
    noGuns              { false },
    gunWasInvalid       { false },
    ammoWasInvalid      { false }
{

}
bool VerifyEventFlags::shouldAdd() const {
    if(locationInvalid)
        return false;
    if(eventTypeInvalid)
        return false;
    if(dateInvalid)
        return false;
    if(noGuns)
        return false;
    if(gunWasInvalid)
        return false;
    if(ammoWasInvalid)
        return false;

    return true;
}

VerifyAmountOfAmmoFlags::VerifyAmountOfAmmoFlags() :
    cartridgeInvalid    { false },
    manufacturerInvalid { false },
    grainWeightInvalid  { false },
    amountInvalid       { false }
{

}
bool VerifyAmountOfAmmoFlags::shouldAdd() const {
    if(cartridgeInvalid)
        return false;
    if(manufacturerInvalid)
        return false;
    if(grainWeightInvalid)
        return false;
    if(amountInvalid)
        return false;
    
    return true;
}
VerifyGunMetadataFlags::VerifyGunMetadataFlags() :
    cartridgeInvalid    { false },
    weaponTypeInvalid   { false },
    nameInvalid         { false }
{

}
bool VerifyGunMetadataFlags::shouldAdd() const {
    if(cartridgeInvalid)
        return false;
    if(weaponTypeInvalid)
        return false;
    if(nameInvalid)
        return false;
    
    return true;
}

AddEventFlags::AddEventFlags() :
    wasAdded        { false },
    alreadyExists   { false }
{

}
AddAmmoFlags::AddAmmoFlags () :
    wasAdded        { false },
    alreadyExists   { false }
{

}
AddGunFlags::AddGunFlags () :
    wasAdded        { false },
    alreadyExists   { false }
{

}

Database::Database() {

}
AddEventFlags Database::addEvent(const ShootingEvent& event){
    const ShootingEventMetadata& info { event.getInfo() }; 
    AddEventFlags flags{ };

    if(events.contains(info)){
        flags.alreadyExists = true;
        return flags;
    }
    
    flags.verifyFlags = verify(event);

    if(flags.verifyFlags.shouldAdd())
        flags.wasAdded = events.try_emplace(info, event).second;

    return flags;
}
AddAmmoFlags Database::addToStockpile     (const AmountOfAmmo& amountOfAmmo){
    return addToStockpile(amountOfAmmo);    
}
AddAmmoFlags Database::addToStockpile(const AmmoMetadata& info) {
    return addToStockpile( StockpileAmmo { info } );
}
AddAmmoFlags Database::addToStockpile(const StockpileAmmo& data){
    const AmmoMetadata& info { data.getAmmoInfo() };

    AddAmmoFlags flags { };

    if(!stockpile.contains(info.cartridge)){
        if(!stockpile.try_emplace(info.cartridge).second)
            return flags;
    }

    if(stockpile.at(info.cartridge).contains(info)){
        flags.alreadyExists = true;
        return flags;
    }

    flags.verifyFlags = verify(data.getAmountOfAmmo());
    
    if(!flags.verifyFlags.shouldAdd())
        return flags;
    
    flags.wasAdded = stockpile.at(info.cartridge).try_emplace(info, data).second;

    if(flags.wasAdded){
        // Add to amount per cartridge list
        if(!addAmountPerCartridge(info.cartridge, data.getAmountOnHand() )){
            stockpile.at(info.cartridge).erase(info);
            flags.wasAdded = false;
        }
    }

    return flags;
}

AddGunFlags Database::addToArmory(const ArmoryGun& gun) {
    const GunMetadata& info { gun.getGunInfo() };

    AddGunFlags flags { };

    if(!armory.contains(info.cartridge)){
        if(!armory.try_emplace(info.cartridge).second)
            return flags;
    }

    if(armory.at(info.cartridge).contains(info)){
        flags.alreadyExists = true;
        return flags;
    }

    flags.verifyFlags = verify(info);
    
    if(!flags.verifyFlags.shouldAdd())
        return flags;
    
    flags.wasAdded = armory.at(info.cartridge).try_emplace(info, gun).second;

    return flags;
}
AddGunFlags Database::addToArmory(const GunMetadata& info) {
    return addToArmory( ArmoryGun { info } );
}
bool Database::useAmmo (const AmountOfAmmo& amountOfAmmo){
    if(!stockpileContains(amountOfAmmo.getAmmoInfo()))
        return false;

    StockpileAmmo& target { getAmmo(amountOfAmmo.getAmmoInfo()) };

    if(target.getAmountOnHand() < amountOfAmmo.getAmount())
        return false;

    target.removeAmount(amountOfAmmo.getAmount());
    amountPerCartridge.at(amountOfAmmo.getAmmoInfo().cartridge) -= amountOfAmmo.getAmount();
    
    return true;
}

ShootingEvent& Database::getEvent(const ShootingEventMetadata& info) {
    return events.at(info);
}
StockpileAmmo& Database::getAmmo   (const AmmoMetadata& info) {
    if(!stockpile.contains(info.cartridge))
        throw std::out_of_range{std::format("Database::getAmmo(), no StockpileAmmo named [{}] found", info.name)};

    return stockpile.at(info.cartridge).at(info);
}
ArmoryGun& Database::getGun (const GunMetadata& info) {
    if(!armory.contains(info.cartridge))
       throw std::out_of_range{std::format("Database::getGun(), no ArmoryGun named [{}] found", info.name)};

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




VerifyEventFlags verify (const ShootingEvent& event){
    VerifyEventFlags flags{ };
    const ShootingEventMetadata& info { event.getInfo() }; 

    if(info.location == EMPTY_LOCATION)
       flags.locationInvalid = true; 
    if(info.eventType == EMPTY_EVENT_TYPE)
        flags.eventTypeInvalid = true;
    if(!info.date.ok() || info.date.year() < std::chrono::year{ShootingEventMetadata::MIN_YEAR})
        flags.dateInvalid = true;


    if(event.getGunsUsed().size() <= 0)
        flags.noGuns = true;
    else{
        for(const auto& gun : event.getGunsUsed()){
            const auto& gunInfo { gun.getGunInfo() };

            if(gunInfo == EMPTY_GUN_METADATA){
                flags.gunWasInvalid = true;
                break;
            }

            if(gun.getAmmoUsed().size() <= 0){
                flags.ammoWasInvalid = true; 
                break;
            }

            for(const auto& amountOfAmmo : gun.getAmmoUsed() ){
                const auto& ammoInfo { amountOfAmmo.getAmmoInfo() };

                if(ammoInfo == EMPTY_AMMO_METADATA || amountOfAmmo.getAmount() <= 0){
                    flags.ammoWasInvalid = true; 
                    break; 
                }
            }
        }
    }

    return flags;
}
VerifyAmountOfAmmoFlags verify  (const AmmoMetadata& info){
    VerifyAmountOfAmmoFlags flags { };
    if(info.cartridge == EMPTY_CARTRIDGE)
       flags.cartridgeInvalid = true; 
    if(info.manufacturer == EMPTY_MANUFACTURER)
        flags.manufacturerInvalid = true;
    if(info.grainWeight <= 0)
        flags.grainWeightInvalid = true;

    return flags;
}
VerifyAmountOfAmmoFlags verify  (const AmountOfAmmo& data){
    const AmmoMetadata& info { data.getAmmoInfo() };

    VerifyAmountOfAmmoFlags flags { };
    flags = verify(info);

    if(data.getAmount() < 0)
        flags.amountInvalid = true;

    return flags;
}
VerifyGunMetadataFlags  verify  (const GunMetadata& info){
    VerifyGunMetadataFlags flags { };
    if(info.cartridge == EMPTY_CARTRIDGE)
       flags.cartridgeInvalid = true; 
    if(info.weaponType == EMPTY_WEAPON_TYPE)
        flags.weaponTypeInvalid = true;
    if(info.name.empty())
        flags.nameInvalid = true;

    return flags;
}



void associateEvents(Database& db){
    for(const auto& [eventInfo, event] : db.getEvents()){
        addMetadataInfo(db, eventInfo);

        for(const auto& gunTrackingAmmo : event.getGunsUsed() ){
            const GunMetadata& gunInfo { gunTrackingAmmo.getGunInfo() };

            // Add gun to armory
            if(!db.armoryContains(gunInfo)){
                if(!db.addToArmory(gunInfo).wasAdded){
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
                        if(!db.addToStockpile(ammoInfo).wasAdded)
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
void addAllMetadataInfo (Database& db){
    for(const auto& [cartridge, map] : db.getArmory()){
        for(const auto& [info, armoryGun] : map)
            addMetadataInfo(db, armoryGun.getGunInfo());
    }

    for(const auto& [cartridge, map] : db.getStockpile()){
        for(const auto& [info, stockpileAmmo] : map)
            addMetadataInfo(db, stockpileAmmo.getAmmoInfo());
    }
   

    for(const auto& [info, event] : db.getEvents())
        addMetadataInfo(db, event.getInfo());
}




void addMetadataInfo(Database& db, const GunMetadata& info){
    if(!db.metadataContains(info.cartridge)){
        if(!db.addCartridge(info.cartridge))
            LAS::log_warn(std::format("addMetadataInfo() could not add Cartridge {}", info.cartridge.getName()) );
    }

    if(!db.metadataContains(info.weaponType)){
        if(!db.addWeaponType(info.weaponType))
            LAS::log_warn(std::format("addMetadataInfo() could not add WeaponType {}", info.weaponType.getName() ) );
    }
}
void addMetadataInfo(Database& db, const AmmoMetadata& info){
    if(!db.metadataContains(info.manufacturer)){
        if(!db.addManufacturer(info.manufacturer))
            LAS::log_warn(std::format("addMetadataInfo() could not add Manufacturer {}", info.manufacturer.getName()) );
    }

    if(!db.metadataContains(info.cartridge)){
        if(!db.addCartridge(info.cartridge))
            LAS::log_warn(std::format("addMetadataInfo() could not add Cartridge {}", info.cartridge.getName()) );
    }

}
void addMetadataInfo(Database& db, const ShootingEventMetadata& info){
    if(!db.metadataContains(info.location)){
        if(!db.addLocation(info.location))
            LAS::log_warn(std::format("addMetadataInfo() could not add Location {}", info.location.getName()) );
    }

    if(!db.metadataContains(info.eventType)){
        if(!db.addEventType(info.eventType))
            LAS::log_warn(std::format("addMetadataInfo() could not add EventType {}", info.eventType.getName()) );
    }
}



bool applyEvent(Database& db, const ShootingEvent& event, bool applyToArmory, bool applyToStockpile){
    const Database snapshot { db }; // If any errors are encounted, DB revert back to here

    if(applyToStockpile){
       for(const auto& gunTrackingAmmoUsed : event.getGunsUsed()){
            for(const auto& amountOfAmmo : gunTrackingAmmoUsed.getAmmoUsed()){
                const AmmoMetadata& ammoInfo { amountOfAmmo.getAmmoInfo() };

                // Apply to stockpile
                if(!db.stockpileContains(ammoInfo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Ammo '{}' was not found in Stockpile", ammoInfo.name)
                    };
                    return false;
                }

                if(!db.useAmmo(amountOfAmmo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Requested {} rounds of '{}', however only {} rounds in Stockpile", 
                                amountOfAmmo.getAmount(),
                                ammoInfo.name,
                                db.amountInStockpile(ammoInfo)
                            )
                    };
                    return false;
                }
            }
        }
    }


    if(applyToArmory){
       for(const auto& gunTrackingAmmoUsed : event.getGunsUsed()){
            const GunMetadata& gunInfo { gunTrackingAmmoUsed.getGunInfo() };

            if(!db.armoryContains(gunInfo)){
               db = snapshot;
               throw std::invalid_argument{
                    std::format("Gun '{}' was not found in the Armory", gunInfo.name)
                };
               return false;
            }
        
            ArmoryGun& gun { db.getGun(gunInfo) };
            
            // Add all ammo used
            for(const auto& amountOfAmmo : gunTrackingAmmoUsed.getAmmoUsed()){
                const AmmoMetadata& ammoInfo { amountOfAmmo.getAmmoInfo() };

                // Apply to the guns history
                if(!gun.addAmmoUsed(amountOfAmmo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to add used Ammo '{}' to Gun '{}'", ammoInfo.name, gunInfo.name)
                    };
                    return false;
                }

                // Check ammo is in stockpile
                if(!db.stockpileContains(ammoInfo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Ammo '{}' was not found in Stockpile", ammoInfo.name)
                    };                        
                    return false;
                }
                
                // Apply to the ammo's history
                StockpileAmmo& ammo { db.getAmmo(ammoInfo) };
                if(!ammo.hasGun(gunInfo)){
                    if(!ammo.addGun(gunInfo)){
                        db = snapshot;
                        throw std::invalid_argument{
                            std::format("Failed to add Gun '{}' to history for Ammo '{}'", gunInfo.name, ammoInfo.name)
                        };                        
                        return false;
                    }
                }
            }

            // Add event to gun history
            if(!gun.addEvent(event)){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to add Event to history for Gun'{}'", gunInfo.name)
                };
                return false;
            }
        }
    }   // End apply to armory
    
    const AddEventFlags addFlags { db.addEvent(event) };
        
    if(!addFlags.wasAdded){
        db = snapshot;
        throw addFlags;
        return false; // Will never be called IK
    }

    addAllMetadataInfo(db);
    return true;
}



}   // End SC namespace
