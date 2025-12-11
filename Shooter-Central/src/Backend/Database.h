#pragma once

#include "DatabaseItems.h"
#include "LAS/Logging.h"

#include <set>
#include <map>
#include <exception>
#include <format>

namespace ShooterCentral {

struct VerifyEventFlags {
    VerifyEventFlags();

    bool locationInvalid    : 1;
    bool eventTypeInvalid   : 1;
    bool dateInvalid        : 1;

    bool noGuns             : 1;    
    bool gunWasInvalid      : 1;   
    bool ammoWasInvalid     : 1;  // Ammo empty or no amount   
    
    bool shouldAdd() const;
};

struct VerifyAmountOfAmmoFlags {
    VerifyAmountOfAmmoFlags();

    bool cartridgeInvalid       : 1;
    bool manufacturerInvalid    : 1;
    bool grainWeightInvalid     : 1;
    bool amountInvalid          : 1;

    bool shouldAdd() const;
};

struct VerifyGunMetadataFlags {
    VerifyGunMetadataFlags();

    bool cartridgeInvalid   : 1;
    bool weaponTypeInvalid  : 1;
    bool nameInvalid        : 1;

    bool shouldAdd() const;
};

struct AddEventFlags {
    AddEventFlags();

    bool wasAdded           : 1;
    bool alreadyExists      : 1;

    VerifyEventFlags verifyFlags { };
};
struct AddAmmoFlags {
    AddAmmoFlags ();

    bool wasAdded       : 1;
    bool alreadyExists  : 1;

    VerifyAmountOfAmmoFlags verifyFlags { };
};
struct AddGunFlags {
    AddGunFlags ();

    bool wasAdded       : 1;
    bool alreadyExists  : 1;

    VerifyGunMetadataFlags verifyFlags { };
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
   
    AddEventFlags   addEvent        (const ShootingEvent& );  // True if event alread exists or created. False if could not emplace 
    AddAmmoFlags    addToStockpile  (const AmountOfAmmo& );
    AddAmmoFlags    addToStockpile  (const AmmoMetadata& );
    AddAmmoFlags    addToStockpile  (const StockpileAmmo& );
    AddGunFlags     addToArmory     (const ArmoryGun& );
    AddGunFlags     addToArmory     (const GunMetadata& ); 

    bool useAmmo            (const AmountOfAmmo& );

    ShootingEvent& getEvent (const ShootingEventMetadata& );    // Throws out_of_range if not present
    StockpileAmmo& getAmmo  (const AmmoMetadata& );             // Throws out_of_range if not present
    ArmoryGun&     getGun   (const GunMetadata& );              // Throws out_of_range if not present

    int amountInStockpile   (const Cartridge& ) const;
    int amountInStockpile   (const AmmoMetadata& ) const;

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

VerifyEventFlags        verify  (const ShootingEvent& info);
VerifyAmountOfAmmoFlags verify  (const AmmoMetadata& info);
VerifyAmountOfAmmoFlags verify  (const AmountOfAmmo& info);
VerifyGunMetadataFlags  verify  (const GunMetadata& info);


// Adds all items into Database along with appropriate logging
void associateEvents    (Database& );
void addAllMetadataInfo (Database& );

void addMetadataInfo (Database&, const GunMetadata& );
void addMetadataInfo (Database&, const AmmoMetadata& );
void addMetadataInfo (Database&, const ShootingEventMetadata& );


bool applyEvent(Database& db, const ShootingEvent& event, bool applyToArmory, bool applyToStockpile); 


}   // End SC namespace
