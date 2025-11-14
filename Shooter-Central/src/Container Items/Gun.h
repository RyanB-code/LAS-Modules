#pragma once

#include "Ammo.h" // Needs cartridge definition

#include <LAS/json.h>

#include <string>
#include <memory>
#include <stdexcept>

namespace ShooterCentral{

    

    

    void to_json(LAS::json& j, const GunMetadata& gun);

    

}
