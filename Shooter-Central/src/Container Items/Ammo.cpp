#include "Ammo.h"



void ShooterCentral::to_json (LAS::json& j, const AmmoMetadata& data){
    j = LAS::json {
        { "name",           data.name },
        { "manufacturer",   data.manufacturer.getName() },
        { "cartridge",      data.cartridge.getName() },
        { "grain",          data.grainWeight},
        { "isActive",       data.isActive}
    };
}



void ShooterCentral::to_json(LAS::json& j, const AmountOfAmmo& data){
    j = LAS::json {
        { "ammoInfo",   data.getAmmoInfo() },
        { "amount",     data.getAmount() }
    };
}
