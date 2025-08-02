#pragma once

#include "AssociatedItems.h"

namespace ShooterCentral {

    struct UnsavedChanges {
        bool guns   { false };
        bool ammo   { false };
        bool events { false };
    };
}
