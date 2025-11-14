#include "Gun.h"

using namespace ShooterCentral;


void ShooterCentral::to_json(LAS::json& j, const GunMetadata& gun){
    using LAS::json;

    j = LAS::json{
        { "name",           gun.name        },
        { "weaponType",     gun.weaponType  },
        { "cartridge",      gun.cartridge   },
        { "isActive",       gun.isActive    }
    };
}




