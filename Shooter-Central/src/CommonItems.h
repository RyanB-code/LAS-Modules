#pragma once

#include "Container Items/AssociatedItems.h"

namespace ShooterCentral {

    struct UnsavedChanges {
        bool guns   { false };
        bool ammo   { false };
        bool events { false };
    };
}
