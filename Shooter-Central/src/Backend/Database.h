#pragma once

#include "DatabaseItems.h"

#include <set>
#include <map>
#include <exception>
#include <format>

namespace ShooterCentral {

class Database {
public:
    Database();
    ~Database() = default;

    const auto& getManufacturers()  const { return manufacturers; }
    const auto& getCartridges()     const { return cartridges; }
    const auto& getWeaponTypes()    const { return weaponTypes; }
    const auto& getLocations()      const { return locations; }
    const auto& getEventTypes()     const { return eventTypes; }
    const auto& getEvents()         const { return events; }

    const auto& getArmory()         const { return armory; }
    const auto& getStockpile()      const { return stockpile; }

    // Throws if cartridge not found
    const auto& getArmory(const Cartridge& cartridge) const {
        if(armory.contains(cartridge))
            return armory.at(cartridge);
        throw std::invalid_argument { std::format("Database::getArmory(), no guns present with Cartridge '{}'", cartridge.getName() ) };
    }
    // Throws if cartridge not found
    const auto& getStockpile(const Cartridge& cartridge) const {
        if(stockpile.contains(cartridge))
            return stockpile.at(cartridge);
        throw std::invalid_argument { std::format("Database::getStockpile(), no ammo present with Cartridge '{}'", cartridge.getName() ) };
    }    

    // getters and adders for events, armory, stockpile
    bool addEvent (const ShootingEvent& event);  // True if event alread exists, or created. False if could not emplace 
   
    bool addToStockpile     (const AmountOfAmmo& );
    bool addToStockpile     (const AmmoMetadata& );
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

    bool armoryContains     (const GunMetadata& info) const;
    bool stockpileContains  (const AmmoMetadata& info) const;

    int amountInStockpile   (const Cartridge& );
    int amountInStockpile   (const AmmoMetadata& );

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
    void addMetadataInfo        (const GunMetadata& );
    void addMetadataInfo        (const AmmoMetadata& );
    void addMetadataInfo        (const ShootingEventMetadata& );

};

}   // End SC namespace
