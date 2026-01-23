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
    amountInvalid       { false },
    nameInvalid         { false }
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
    if(nameInvalid)
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
    return addToStockpile( StockpileAmmo {amountOfAmmo} );    
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
bool Database::addToExistingAmmo  (const AmountOfAmmo& amountOfAmmo){
    if(amountOfAmmo.getAmount() <= 0)
        return false;

    if(!stockpileContains(amountOfAmmo.getAmmoInfo()))
        return false;

    StockpileAmmo& target { getAmmo(amountOfAmmo.getAmmoInfo()) };

    target.addAmount(amountOfAmmo.getAmount());
    amountPerCartridge.at(amountOfAmmo.getAmmoInfo().cartridge) += amountOfAmmo.getAmount();
    
    return true;
}

void Database::deleteEvent(const ShootingEventMetadata& info){
    events.erase(info);
}

void Database::deleteFromStockpile (const AmmoMetadata& info){
    if(stockpile.contains(info.cartridge))
        stockpile.at(info.cartridge).erase(info);

    // Erase if Cartridge map has no entries
    if(stockpile.at(info.cartridge).empty()){
        stockpile.erase(info.cartridge);
        amountPerCartridge.erase(info.cartridge);
    }
}
void Database::deleteGun(const GunMetadata& info){
    if(armory.contains(info.cartridge))
        armory.at(info.cartridge).erase(info);

    // Erase if Cartridge map has no entries
    if(armory.at(info.cartridge).empty())
        armory.erase(info.cartridge);
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
const ShootingEvent& Database::getEvent(const ShootingEventMetadata& info) const {
    return events.at(info);
}
const StockpileAmmo& Database::getAmmo   (const AmmoMetadata& info) const {
    if(!stockpile.contains(info.cartridge))
        throw std::out_of_range{std::format("Database::getAmmo(), no StockpileAmmo named [{}] found", info.name)};

    return stockpile.at(info.cartridge).at(info);
}
const ArmoryGun& Database::getGun (const GunMetadata& info) const{
    if(!armory.contains(info.cartridge))
       throw std::out_of_range{std::format("Database::getGun(), no ArmoryGun named [{}] found", info.name)};

    return armory.at(info.cartridge).at(info);
}
bool Database::addMetadataItem  (const Manufacturer& add){
    return manufacturers.emplace(add).second;
}
bool Database::addMetadataItem(const Cartridge& add){
    return cartridges.emplace(add).second;
}
bool Database::addMetadataItem  (const WeaponType& add){
    return weaponTypes.emplace(add).second;
}
bool Database::addMetadataItem  (const Location& add){
    return locations.emplace(add).second;
}
bool Database::addMetadataItem  (const ShootingEventType& add){
    return eventTypes.emplace(add).second;
}
void Database::deleteMetadataItem  (const Manufacturer& item){
    manufacturers.erase(item);
}
void Database::deleteMetadataItem(const Cartridge& item){
    cartridges.erase(item);
}
void Database::deleteMetadataItem  (const WeaponType& item){
    weaponTypes.erase(item);
}
void Database::deleteMetadataItem  (const Location& item){
    locations.erase(item);
}
void Database::deleteMetadataItem  (const ShootingEventType& item){
    eventTypes.erase(item);
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
    if(info.name.empty())
        flags.nameInvalid = true;

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
        if(!db.addMetadataItem(info.cartridge))
            LAS::log_warn(std::format("addMetadataInfo() could not add Cartridge {}", info.cartridge.getName()) );
    }

    if(!db.metadataContains(info.weaponType)){
        if(!db.addMetadataItem(info.weaponType))
            LAS::log_warn(std::format("addMetadataInfo() could not add WeaponType {}", info.weaponType.getName() ) );
    }
}
void addMetadataInfo(Database& db, const AmmoMetadata& info){
    if(!db.metadataContains(info.manufacturer)){
        if(!db.addMetadataItem(info.manufacturer))
            LAS::log_warn(std::format("addMetadataInfo() could not add Manufacturer {}", info.manufacturer.getName()) );
    }

    if(!db.metadataContains(info.cartridge)){
        if(!db.addMetadataItem(info.cartridge))
            LAS::log_warn(std::format("addMetadataInfo() could not add Cartridge {}", info.cartridge.getName()) );
    }

}
void addMetadataInfo(Database& db, const ShootingEventMetadata& info){
    if(!db.metadataContains(info.location)){
        if(!db.addMetadataItem(info.location))
            LAS::log_warn(std::format("addMetadataInfo() could not add Location {}", info.location.getName()) );
    }

    if(!db.metadataContains(info.eventType)){
        if(!db.addMetadataItem(info.eventType))
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

bool changeAllOccurrences(Database& db, const Manufacturer& old, const Manufacturer& revised){
    const Database snapshot { db };

    db.deleteMetadataItem(old);
    if(!db.addMetadataItem(revised)){
        db = snapshot;
        throw std::invalid_argument{
            std::format("Failed to add Manufacturer '{}'", revised.getName())
        };
    }

    {   // Start toModify scope
        std::vector<AmmoMetadata> toModify { };

        // Mark all ammo needed to be changed
        for(const auto& [cartridge, map] : db.getStockpile()){
            for(const auto& [key, stockpileAmmo] : map){ 
                const AmmoMetadata& ammoInfo { stockpileAmmo.getAmmoInfo() };
                if(ammoInfo.manufacturer == old){
                    toModify.emplace_back(ammoInfo);
                }
            }

        }

        for(const auto& oldAmmoInfo : toModify){
            AmmoMetadata newAmmoInfo { oldAmmoInfo };
            newAmmoInfo.manufacturer = revised;

            // Attempt to make new one
            if(!db.addToStockpile(newAmmoInfo).wasAdded){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to revise AmmoMetadata for '{}'", oldAmmoInfo.name)
                };
            }

            StockpileAmmo& newAmmo { db.getAmmo(newAmmoInfo) };

            // Closed-scope so erase later wont cause UB
            {
                StockpileAmmo& oldAmmo { db.getAmmo(oldAmmoInfo) };

                // Copy all items
                newAmmo.addAmount(oldAmmo.getAmountOnHand() ); 
                newAmmo.setActive(oldAmmo.isActive() );

                for(const GunMetadata& gunInfo : oldAmmo.getGunsUsed()){
                    if(!newAmmo.addGun(gunInfo)){
                        db = snapshot;
                        throw std::invalid_argument{
                            std::format("Failed to add guns used to new StockpileAmmo '{}'", newAmmoInfo.name)
                        };
                    }
                }
            }

            // Erase the old one
            db.deleteFromStockpile(oldAmmoInfo);
        }
    }   // End toModify scope


    // Edit gun histories
    for(const auto& [cartridge, map] : db.getArmory()){
        for(const auto& [key, armoryGun] : map){ 
            const GunMetadata& info { armoryGun.getGunInfo() };

            std::vector<AmountOfAmmo> toModify { };
            
            // Add to list if changes need to be made
            for(const auto& [key, amountOfAmmo] : armoryGun.getAmmoUsed()){
                if(amountOfAmmo.getAmmoInfo().manufacturer == old){
                    toModify.emplace_back(amountOfAmmo);
                }
            }
            
            // Replace old info with new
            for(const auto& oldAmountOfAmmo : toModify){
                AmmoMetadata newAmmoInfo { oldAmountOfAmmo.getAmmoInfo() };
                newAmmoInfo.manufacturer = revised;
                AmountOfAmmo newAmountOfAmmo { newAmmoInfo, oldAmountOfAmmo.getAmount() };

                ArmoryGun& gun { db.getGun(info) };

                gun.removeAmmoUsed(oldAmountOfAmmo.getAmmoInfo());

                if(!gun.addAmmoUsed(newAmountOfAmmo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to add revised AmmoMetadata '{}' in Gun '{}'", newAmmoInfo.name, info.name)
                    };
                }

            }   // End going through toModify
             
        }   // End iterating over all guns for cartridge
    }   // End all cartridges


    // Edit event ammo used
    for (const auto& [key, event] : db.getEvents()){
        
        for(const auto& gunTrackingAmmoUsed : event.getGunsUsed() ){
            std::vector<AmountOfAmmo> toModify { };
            
            // Add to list if changes need to be made
            for(const auto& amountOfAmmo : gunTrackingAmmoUsed.getAmmoUsed()){
                if(amountOfAmmo.getAmmoInfo().manufacturer == old){
                    toModify.emplace_back(amountOfAmmo);
                }
            }
            
            // Replace old info with new
            for(const auto& oldAmountOfAmmo : toModify){
                AmmoMetadata newAmmoInfo { oldAmountOfAmmo.getAmmoInfo() };
                newAmmoInfo.manufacturer = revised;
                AmountOfAmmo newAmountOfAmmo { newAmmoInfo, oldAmountOfAmmo.getAmount() };

                ShootingEvent& eventToModify { db.getEvent(key) };
                eventToModify.getGun(gunTrackingAmmoUsed.getGunInfo()).removeAmmoUsed(oldAmountOfAmmo.getAmmoInfo());
                
                if(!eventToModify.getGun(gunTrackingAmmoUsed.getGunInfo()).addAmmoUsed(newAmountOfAmmo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to revise AmmoMetadata '{}' used in Event {}", newAmmoInfo.name, eventName(eventToModify.getInfo()))
                    };
                }

            }   // End going through toModify

        }   // End guns for event

    }   // End Events

    return true;
}
bool changeAllOccurrences(Database& db, const Location& old, const Location& revised){
    const Database snapshot { db };

    db.deleteMetadataItem(old);
    if(!db.addMetadataItem(revised)){
        db = snapshot;
        throw std::invalid_argument{
            std::format("Failed to add Location '{}'", revised.getName())
        };
    }

    std::vector<ShootingEventMetadata> toModify { };

    // Mark all events needed to be changed
    for(const auto& [key, event] : db.getEvents()){
        const ShootingEventMetadata& info { event.getInfo() };
        if(info.location == old){
            toModify.emplace_back(info);
        }
    }

    // Make changes for all keys in vector
    for(const auto& oldInfo : toModify){
        ShootingEventMetadata newInfo { oldInfo };
        newInfo.location = revised;
        ShootingEvent newEvent { newInfo };

        // Add guns to new event
        // Closed-scope so erase later wont cause UB
        {
            ShootingEvent& oldEvent { db.getEvent(oldInfo) };

            for(const GunTrackingAmmoUsed& gun : oldEvent.getGunsUsed()){
                if(!newEvent.addGun(gun)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed copying guns to new Event {}", eventName(newInfo))
                    };
                }
            }
        }

        if(!db.addEvent(newEvent).wasAdded){
            db = snapshot;
            throw std::invalid_argument{
                std::format("Failed to add revised Event {}", eventName(oldInfo))
            };
        }

        // Erase the old one
        db.deleteEvent(oldInfo);
    }

    // Edit gun histories
    for(const auto& [cartridge, map] : db.getArmory()){
        for(const auto& [key, armoryGun] : map){ 
            const GunMetadata& info { armoryGun.getGunInfo() };

            std::vector<ShootingEventMetadata> toModify { };
            
            // Add to list if changes need to be made
            for(const auto& eventInfo : armoryGun.getEventsUsed()){
                if(eventInfo.location == old){
                    toModify.emplace_back(eventInfo);
                }
            }
            
            // Replace old info with new
            for(const auto& oldInfo : toModify){
                ShootingEventMetadata newInfo { oldInfo };
                newInfo.location = revised;

                ArmoryGun& gun      { db.getGun(info) };

                gun.removeEvent(oldInfo);

                if(!gun.addEvent( db.getEvent(newInfo) )){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to add revised EventMetadata for Gun '{}'", info.name)
                    };
                }

            }   // End going through toModify
        }   // End iterating over all guns for cartridge
    }   // End all cartridges

    return true;
}
bool changeAllOccurrences(Database& db, const ShootingEventType& old, const ShootingEventType& revised){
    const Database snapshot { db };

    db.deleteMetadataItem(old);
    if(!db.addMetadataItem(revised)){
        db = snapshot;
        throw std::invalid_argument{
            std::format("Failed to add ShootingEventType '{}'", revised.getName())
        };
    }

    std::vector<ShootingEventMetadata> toModify { };

    // Mark all events needed to be changed
    for(const auto& [key, event] : db.getEvents()){
        const ShootingEventMetadata& info { event.getInfo() };
        if(info.eventType == old){
            toModify.emplace_back(info);
        }
    }

    // Make changes for all keys in vector
    for(const auto& oldInfo : toModify){
        ShootingEventMetadata newInfo { oldInfo };
        newInfo.eventType = revised;
        ShootingEvent newEvent { newInfo };

        // Add guns to new event
        // Closed-scope so erase later wont cause UB
        {
            ShootingEvent& oldEvent { db.getEvent(oldInfo) };

            for(const GunTrackingAmmoUsed& gun : oldEvent.getGunsUsed()){
                if(!newEvent.addGun(gun)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed copying guns to new Event {}", eventName(newInfo))
                    };
                }
            }
        }

        if(!db.addEvent(newEvent).wasAdded){
            db = snapshot;
            throw std::invalid_argument{
                std::format("Failed to add revised Event {}", eventName(oldInfo))
            };
        }

        // Erase the old one
        db.deleteEvent(oldInfo);
    }

    // Edit gun histories
    for(const auto& [cartridge, map] : db.getArmory()){
        for(const auto& [key, armoryGun] : map){ 
            const GunMetadata& info { armoryGun.getGunInfo() };

            std::vector<ShootingEventMetadata> toModify { };
            
            // Add to list if changes need to be made
            for(const auto& eventInfo : armoryGun.getEventsUsed()){
                if(eventInfo.eventType == old){
                    toModify.emplace_back(eventInfo);
                }
            }
            
            // Replace old info with new
            for(const auto& oldInfo : toModify){
                ShootingEventMetadata newInfo { oldInfo };
                newInfo.eventType = revised;

                ArmoryGun& gun      { db.getGun(info) };

                gun.removeEvent(oldInfo);

                if(!gun.addEvent( db.getEvent(newInfo) )){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to add revised EventMetadata for Gun '{}'", info.name)
                    };
                }

            }   // End going through toModify
        }   // End iterating over all guns for cartridge
    }   // End all cartridges

    return true;
}
bool changeAllOccurrences(Database& db, const WeaponType& old, const WeaponType& revised){
    const Database snapshot { db };

    db.deleteMetadataItem(old);
    if(!db.addMetadataItem(revised)){
        db = snapshot;
        throw std::invalid_argument{
            std::format("Failed to add WeaponType '{}'", revised.getName())
        };
    }


    // Edit events first since thid will be needed in adding Event to Gun history 
    for (const auto& [key, event] : db.getEvents()){
        std::vector<GunMetadata> toModify;
        
        // Mark if changes need to be made
        for(const auto& gunTrackingAmmoUsed : event.getGunsUsed() ){
            if(gunTrackingAmmoUsed.getGunInfo().weaponType == old){
                toModify.emplace_back(gunTrackingAmmoUsed.getGunInfo());
            }
        }

        // Select GunTrackingAmmoUsed to be changed, copy it, and apply to Event
        ShootingEvent& targetEvent { db.getEvent(key) };    
        for(const GunMetadata& oldInfo : toModify){
            GunTrackingAmmoUsed oldGunUsed { targetEvent.getGun(oldInfo) };

            GunMetadata newInfo { oldInfo };
            newInfo.weaponType = revised;

            GunTrackingAmmoUsed newGunUsed { newInfo };

            for(const auto& amountOfAmmo : oldGunUsed.getAmmoUsed() ){
                if(!newGunUsed.addAmmoUsed(amountOfAmmo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to copy used Ammo '{}' to revised Gun '{}' for Event {}",
                            amountOfAmmo.getAmmoInfo().name,
                            newInfo.name,
                            eventName(key)
                        )
                    };
                }
            }

            targetEvent.removeGun(oldInfo);
            
            if(!targetEvent.addGun(newGunUsed)){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to add revised GunTrackingAmmoUsed '{}' to Event {}",
                        newInfo.name,
                        eventName(key)
                    )
                };
            }
            
        }   // End modifying every GunUsed for the Event 

    }   // End going through Events


    std::vector<GunMetadata> toModify { };

    // Mark all guns needed to be changed
    for(const auto& [cartrdige, map] : db.getArmory()){
        for(const auto& [key, armoryGun] : map){
            const GunMetadata& info { armoryGun.getGunInfo() };
            if(info.weaponType == old){
                toModify.emplace_back(info);
            }
        }
    }

    // Make changes for all keys in vector
    for(const auto& oldInfo : toModify){
        GunMetadata newInfo { oldInfo };
        newInfo.weaponType = revised;

        if(!db.addToArmory(newInfo).wasAdded){
            db = snapshot;
            throw std::invalid_argument{
                std::format("Failed to add revised ArmoryGun '{}'", newInfo.name)
            };
        }

        ArmoryGun& newGun { db.getGun(newInfo) };

        // Closed-scope so erase later wont cause UB
        {
            ArmoryGun& oldGun { db.getGun(oldInfo) };

            newGun.setActive(oldGun.isActive());

            for(const ShootingEventMetadata& eventInfo : oldGun.getEventsUsed()){
                if(!newGun.addEvent( db.getEvent(eventInfo) )){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed copying EventMetadata to new Gun '{}'", newInfo.name)
                    };
                }
            }

            for(const auto& [key, amountOfAmmo] : oldGun.getAmmoUsed()){
                if(!newGun.addAmmoUsed(amountOfAmmo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed copying AmountOfAmmo to new Gun '{}'", newInfo.name)
                    };
                }
            }
       }

        // Erase the old one
        db.deleteGun(oldInfo);
    }


    // Edit Ammo histories
    for(const auto& [cartridge, map] : db.getStockpile()){
        for(const auto& [key, stockpileAmmo] : map){ 
            const AmmoMetadata& info { stockpileAmmo.getAmmoInfo() };

            std::vector<GunMetadata> toModify { };
            
            // Add to list if changes need to be made
            for(const auto& gunInfo : stockpileAmmo.getGunsUsed()){
                if(gunInfo.weaponType == old){
                    toModify.emplace_back(gunInfo);
                }
            }
            
            // Replace old info with new
            for(const auto& oldInfo : toModify){
                GunMetadata newInfo { oldInfo };
                newInfo.weaponType = revised;

                StockpileAmmo& ammo { db.getAmmo(info) };

                ammo.removeGun(oldInfo);

                if(!ammo.addGun( newInfo )){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to add revised GunMetadata to Ammo '{}'", info.name)
                    };
                }

            }   // End going through toModify
        }   // End iterating over all guns for cartridge
    }   // End all cartridges
    

    
    return true;
}
bool changeAllOccurrences(Database& db, const Cartridge& old, const Cartridge& revised){
    const Database snapshot { db };

    db.deleteMetadataItem(old);
    if(!db.addMetadataItem(revised)){
        db = snapshot;
        throw std::invalid_argument{
            std::format("Failed to add Cartridge '{}'", revised.getName())
        };
    }

    // Edit all Events
    for (const auto& [key, event] : db.getEvents()){
        std::vector<GunMetadata> toModify;

        // Mark if changes need to be made
        for(const auto& gunTrackingAmmoUsed : event.getGunsUsed() ){
            if(gunTrackingAmmoUsed.getGunInfo().cartridge == old){
                toModify.emplace_back(gunTrackingAmmoUsed.getGunInfo());
            }
        }

        // Select GunTrackingAmmoUsed to be changed, copy it, and apply to Event
        ShootingEvent& targetEvent { db.getEvent(key) };    
        for(const GunMetadata& oldInfo : toModify){
            GunTrackingAmmoUsed oldGunUsed { targetEvent.getGun(oldInfo) };

            GunMetadata newInfo { oldInfo };
            newInfo.cartridge = revised;

            GunTrackingAmmoUsed newGunUsed { newInfo };

            for(const auto& oldAmountOfAmmo : oldGunUsed.getAmmoUsed() ){
                // Edit the Ammo Used cartridge here
                AmmoMetadata newAmmoInfo { oldAmountOfAmmo.getAmmoInfo() };
                newAmmoInfo.cartridge = revised;

                AmountOfAmmo newAmountOfAmmo { newAmmoInfo, oldAmountOfAmmo.getAmount() };

                if(!newGunUsed.addAmmoUsed(newAmountOfAmmo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to copy revised Ammo '{}' to revised Gun '{}' for Event {}",
                                newAmountOfAmmo.getAmmoInfo().name,
                                newInfo.name,
                                eventName(key)
                            )
                    };
                }
            }

            targetEvent.removeGun(oldInfo);

            if(!targetEvent.addGun(newGunUsed)){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to add revised GunTrackingAmmoUsed '{}' to Event {}",
                            newInfo.name,
                            eventName(key)
                            )
                };
            }

        }   // End modifying every GunUsed for the Event 

    }   // End going through Events
    

    // Edit all Guns
    {
        std::vector<GunMetadata> toModify { };
        for(const auto& [cartridge, map] : db.getArmory()){
           if(cartridge == old){
                for(const auto& [key, armoryGun] : map){
                    toModify.emplace_back(armoryGun.getGunInfo());
                }
           }
        }

        for(const auto& oldGunInfo : toModify){
            GunMetadata newGunInfo { oldGunInfo };
            newGunInfo.cartridge = revised;

            ArmoryGun newGun { newGunInfo };

            ArmoryGun& oldGun { db.getGun(oldGunInfo) };

            newGun.setActive(oldGun.isActive());

            // Copy all Events
            for(const ShootingEventMetadata& eventInfo : oldGun.getEventsUsed()){
                if(!newGun.addEvent( db.getEvent(eventInfo) )){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed copying EventMetadata to revised Gun '{}'", newGunInfo.name)
                    };
                }
            }

            // Change Ammo Used Cartridges and copy to new gun
            for(const auto& [key, oldAmountOfAmmo] : oldGun.getAmmoUsed()){
                AmmoMetadata newAmmoInfo { oldAmountOfAmmo.getAmmoInfo() };
                newAmmoInfo.cartridge = revised;

                AmountOfAmmo newAmountOfAmmo { newAmmoInfo, oldAmountOfAmmo.getAmount() };

                if(!newGun.addAmmoUsed(newAmountOfAmmo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed adding revised AmountOfAmmo to revised Gun '{}'", newGunInfo.name)
                    };
                }
            }

            db.deleteGun(oldGunInfo);
            if(!db.addToArmory(newGun).wasAdded){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to add revised Gun '{}' to Armory", newGunInfo.name)
                };
            }
        }
    }   // GunMetadata toModify scope end

    // Edit all Ammo
    {
        std::vector<AmmoMetadata> toModify;
        for(const auto& [cartridge, map] : db.getStockpile()){
           if(cartridge == old){
                for(const auto& [key, stockpileAmmo] : map){
                    toModify.emplace_back(stockpileAmmo.getAmmoInfo());
                }
           }
        }

        for(const auto& oldAmmoInfo : toModify){
            AmmoMetadata newAmmoInfo { oldAmmoInfo };
            newAmmoInfo.cartridge = revised;

            StockpileAmmo newAmmo { newAmmoInfo };

            StockpileAmmo& oldAmmo { db.getAmmo(oldAmmoInfo) };

            newAmmo.addAmount(oldAmmo.getAmountOnHand() ); 
            newAmmo.setActive(oldAmmo.isActive() );

            for(const GunMetadata& oldGunInfo : oldAmmo.getGunsUsed()){
                GunMetadata newGunInfo { oldGunInfo };
                newGunInfo.cartridge = revised;

                if(!newAmmo.addGun(newGunInfo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to add revised Gun '{}' to revised StockpileAmmo '{}'", newGunInfo.name, newAmmoInfo.name)
                    };
                }
            }

            // Erase the old one
            db.deleteFromStockpile(oldAmmoInfo);
            if(!db.addToStockpile(newAmmo).wasAdded){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to add revised Ammo '{}' to Stockpile", newAmmoInfo.name)
                };
            }
        }
    }

    return true;
}
bool changeAllOccurrences(Database& db, const GunMetadata& old, const GunMetadata& revised){
    const Database snapshot { db };

    // Edit all Events
    for (const auto& [key, event] : db.getEvents()){
        if(!event.hasUsedGun(old))
            continue;

        ShootingEvent& targetEvent { db.getEvent(key) };    

        GunTrackingAmmoUsed oldGunUsed { targetEvent.getGun(old) };
        GunTrackingAmmoUsed newGunUsed { revised };

        for(const auto& amountOfAmmo : oldGunUsed.getAmmoUsed() ){
            if(!newGunUsed.addAmmoUsed(amountOfAmmo)){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to copy Ammo '{}' to revised Gun '{}' for Event {}",
                            amountOfAmmo.getAmmoInfo().name,
                            revised.name,
                            eventName(key)
                        )
                };
            }
        }

        targetEvent.removeGun(old);

        if(!targetEvent.addGun(newGunUsed)){
            db = snapshot;
            throw std::invalid_argument{
                std::format("Failed to add revised GunTrackingAmmoUsed '{}' to Event {}",
                        revised.name,
                        eventName(key)
                    )
            };
        }
    }   // End going through Events


    // Edit gun
    if(!db.addToArmory(revised).wasAdded){
        db = snapshot;
        throw std::invalid_argument{
            std::format("Failed to add revised Gun '{}' to Armory", revised.name)
        };
    }

    ArmoryGun& newGun { db.getGun(revised) };
    {
        ArmoryGun& oldGun { db.getGun(old) };

        newGun.setActive(oldGun.isActive());

        // Copy all Events
        for(const ShootingEventMetadata& eventInfo : oldGun.getEventsUsed()){
            if(!newGun.addEvent( db.getEvent(eventInfo) )){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed copying EventMetadata to revised Gun '{}'", revised.name)
                };
            }
        }


        // Copy all ammo used
        for(const auto& [key, amountOfAmmo] : oldGun.getAmmoUsed()){
            if(!newGun.addAmmoUsed(amountOfAmmo)){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed copying AmountOfAmmo to revised Gun '{}'", revised.name)
                };
            }
        }

        db.deleteGun(old);
    }
        
    // Edit all Ammo
    for(const auto& [cartridge, map] : db.getStockpile()){
        for(const auto& [key, stockpileAmmo] : map){

            // Edit guns that have used the Ammo
            if(stockpileAmmo.hasGun(old)){
                StockpileAmmo& item { db.getAmmo(stockpileAmmo.getAmmoInfo()) };
                item.removeGun(old);

                if(!item.addGun(revised)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to add revised Gun '{}' to StockpileAmmo '{}'", revised.name, stockpileAmmo.getAmmoInfo().name)
                    };
                }
            }                
        }
    }

    return true;
}
bool changeAllOccurrences(Database& db, const AmmoMetadata& old, const AmmoMetadata& revised){
    const Database snapshot { db };

    // Edit stockpile
    if(!db.addToStockpile(revised).wasAdded){
        db = snapshot;
        throw std::invalid_argument{
            std::format("Failed to add revised AmmoMetadata '{}' to Stockpile", revised.name)
        };
    }

    StockpileAmmo& newAmmo { db.getAmmo(revised) };

    {
        StockpileAmmo& oldAmmo { db.getAmmo(old) };
        newAmmo.setActive( oldAmmo.isActive() );
        newAmmo.addAmount( oldAmmo.getAmountOnHand() );

        for(const GunMetadata& gunInfo : oldAmmo.getGunsUsed()){
            if(!newAmmo.addGun(gunInfo)){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to copy Gun '{}' to revised StockpileAmmo '{}'", gunInfo.name, revised.name)
                };
            }
        }

        db.deleteFromStockpile(old);
    }
    

    // Edit all Events
    for (const auto& [key, event] : db.getEvents()){
        for(const auto& gunTrackingAmmoUsed : event.getGunsUsed() ){

            // Skip if cartridges dont match
            if(gunTrackingAmmoUsed.getGunInfo().cartridge != old.cartridge)
                continue;

            // Skip if the gun hasnt used the old ammo
            if(!gunTrackingAmmoUsed.hasUsedAmmo(old))
                continue;

            // Find the old ammo and copy
            AmountOfAmmo newAmountOfAmmo { revised };
            for(const auto& oldAmountOfAmmo : gunTrackingAmmoUsed.getAmmoUsed() ){
                if(oldAmountOfAmmo.getAmmoInfo() != old)
                    continue;
                
                newAmountOfAmmo.addAmount( oldAmountOfAmmo.getAmount() );
            }

            // Apply edit to the Gun for the Event
            ShootingEvent&          targetEvent { db.getEvent(event.getInfo()) };
            GunTrackingAmmoUsed&    targetGun   { targetEvent.getGun(gunTrackingAmmoUsed.getGunInfo()) };

            targetGun.removeAmmoUsed(old);
            if(!targetGun.addAmmoUsed(newAmountOfAmmo)){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to revise AmmoMetadata '{}' used in Event {}", revised.name, eventName(targetEvent.getInfo()))
                };
            }
        }   // End guns for event
    }   // End going through Events


    // Edit guns
    for(const auto& [cartridge, map] : db.getArmory()){
        if(cartridge != old.cartridge)
            continue;
            
        for(const auto& [key, armoryGun] : map){
            if(!armoryGun.hasUsedAmmo(old))
                continue;

            // Find the old ammo and copy
            AmountOfAmmo newAmountOfAmmo { revised };
            for(const auto& [key, oldAmountOfAmmo] : armoryGun.getAmmoUsed() ){
                if(oldAmountOfAmmo.getAmmoInfo() != old)
                    continue;
                
                newAmountOfAmmo.addAmount( oldAmountOfAmmo.getAmount() );
            }

            // Apply the change
            ArmoryGun& targetGun { db.getGun(armoryGun.getGunInfo()) };

            targetGun.removeAmmoUsed(old);
            if(!targetGun.addAmmoUsed(newAmountOfAmmo)){
                db = snapshot;
                throw std::invalid_argument{
                    std::format("Failed to revise AmmoMetadata '{}' in history for Gun '{}'", revised.name, targetGun.getGunInfo().name)
                };
            }
        }
    }

    return true;
}
bool changeAllOccurrences(Database& db, const ShootingEventMetadata& oldInfo, const ShootingEvent& revised){
    const Database snapshot { db };

    // Remove everything from old event
    {
        ShootingEvent& old { db.getEvent(oldInfo) };

        for(const auto& gunTrackingAmmoUsed : old.getGunsUsed()){
            const GunMetadata& gunInfo { gunTrackingAmmoUsed.getGunInfo() }; 

            db.getGun(gunInfo).removeEvent(oldInfo);

            for(const auto& amountOfAmmo: gunTrackingAmmoUsed.getAmmoUsed() ){
                const AmmoMetadata& ammoInfo { amountOfAmmo.getAmmoInfo() };

                if(!db.armoryContains(gunInfo)){
                    db = snapshot;
                    throw std::invalid_argument{
                        std::format("Failed to find Gun '{}' in Armory to edit", gunInfo.name)
                    };
                }

                ArmoryGun& gun { db.getGun(gunInfo) };

                gun.removeAmountOfAmmo(amountOfAmmo);

                // If amount used of that ammo type becomes <= 0, that entry is removed
                // So need to update ammo history if that gun used 0 of that ammo
                if(!gun.hasUsedAmmo(ammoInfo)){
                    if(!db.stockpileContains(ammoInfo)){
                        db = snapshot;
                        throw std::invalid_argument{
                            std::format("Failed to find Ammo '{}' in Stockpile to edit", ammoInfo.name)
                        };
                    }
                    
                    StockpileAmmo& ammo { db.getAmmo( amountOfAmmo.getAmmoInfo() ) };
                    ammo.removeGun(gunInfo);
                }
            }
        }
    }   // End scope for old event reference

    db.deleteEvent(oldInfo);

    // Will throw items
    // These should be caught in the command
    try {
        return applyEvent(db, revised, true, false);
    }
    catch(...){
        db = snapshot;
        throw;
    }
}


}   // End SC namespace
