#pragma once

#include "Gun.h"

#include <vector>
#include <memory>


namespace ShooterCentral{ 

    void to_json(LAS::json& j, const ShootingEventMetadata& data);

    
}
