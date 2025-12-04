#pragma once

#include "DatabaseItems.h"
#include "LAS/Logging.h"

#include <set>
#include <map>
#include <exception>
#include <format>

namespace ShooterCentral {

struct AddEventFlags {
    bool wasAdded           : 1;
    bool alreadyExists      : 1;

    bool locationEmpty      : 1;
    bool eventTypeEmpty     : 1;
    bool dateInvalid        : 1;

    bool gunsEmpty          : 1;    
    bool gunWasEmpty        : 1;   
    bool ammoWasInvalid     : 1;  // Ammo empty or no amount   

    bool shouldAdd() const;
};

class Database {
public:
    Database();
    ~Database() = default;

    const auto& getManufacturers()      const { return manufacturers; }
    const auto& getCartridges()         const { return cartridges; }
    const auto& getWeaponTypes()        const { return weaponTypes; }
    const auto& getLocations()          const { return locations; }
    const auto& getEventTypes()         const { return eventTypes; }
    const auto& getEvents()             const { return events; }
    const auto& getArmory()             const { return armory; }
    const auto& getStockpile()          const { return stockpile; }
    const auto& getAmountPerCartridge() const { return amountPerCartridge; }

    // Throws if cartridge not found
    const auto& getArmory(const Cartridge& cartridge) const {
        if(armory.contains(cartridge))
            return armory.at(cartridge);
        throw std::out_of_range { std::format("Database::getArmory(), no guns present with Cartridge '{}'", cartridge.getName() ) };
    }
    // Throws if cartridge not found
    const auto& getStockpile(const Cartridge& cartridge) const {
        if(stockpile.contains(cartridge))
            return stockpile.at(cartridge);
        throw std::out_of_range { std::format("Database::getStockpile(), no ammo present with Cartridge '{}'", cartridge.getName() ) };
    }    

   
    AddEventFlags addEvent  (const ShootingEvent& );  // True if event alread exists or created. False if could not emplace 
    bool addToStockpile     (const AmountOfAmmo& );
    bool addToStockpile     (const AmmoMetadata& );
    bool addToStockpile     (const StockpileAmmo& );
    bool addToArmory        (const ArmoryGun& );
    bool addToArmory        (const GunMetadata& ); 

    ShootingEvent& getEvent  (const ShootingEventMetadata& );    // Throws out_of_range if not present
    StockpileAmmo& getAmmo   (const AmmoMetadata& );             // Throws out_of_range if not present
    ArmoryGun&     getGun    (const GunMetadata& );              // Throws out_of_range if not present

    bool addManufacturer    (const Manufacturer& );
    bool addCartridge       (const Cartridge& );
    bool addWeaponType      (const WeaponType& );
    bool addLocation        (const Location& );
    bool addEventType       (const ShootingEventType& );

    bool armoryContains     (const GunMetadata& ) const;
    bool stockpileContains  (const AmmoMetadata& ) const;

    bool metadataContains   (const Manufacturer& ) const;
    bool metadataContains   (const Cartridge& ) const;
    bool metadataContains   (const WeaponType& ) const;
    bool metadataContains   (const Location& ) const;
    bool metadataContains   (const ShootingEventType& ) const;

    int amountInStockpile   (const Cartridge& ) const;
    int amountInStockpile   (const AmmoMetadata& ) const;


private:
    std::set<Manufacturer>      manufacturers   { };
    std::set<Cartridge>         cartridges      { };
    std::set<WeaponType>        weaponTypes     { };
    std::set<Location>          locations       { };
    std::set<ShootingEventType> eventTypes      { };

    std::map<ShootingEventMetadata, ShootingEvent>              events              { };

    std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>       armory              { };
    std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>> stockpile           { };
    std::map<Cartridge, int>                                    amountPerCartridge  { };

    bool addAmountPerCartridge  (const Cartridge& cartridge, int amount);
};


// Here add item by association for event, gun, ammo
void associateEvents    (Database& );

void addAllMetadataInfo (Database& );

// Adds all items into Database along with appropriate logging
void addMetadataInfo (Database&, const GunMetadata& );
void addMetadataInfo (Database&, const AmmoMetadata& );
void addMetadataInfo (Database&, const ShootingEventMetadata& );

}   // End SC namespace
