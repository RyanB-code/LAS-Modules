#pragma once

#include "Backend/Database.h"

#include <iostream>
#include <format>

namespace ShooterCentral {

void printAmmoMetadata  (const AmmoMetadata& info);
void printGunMetadata   (const GunMetadata& info);
void printEventMetadata (const ShootingEventMetadata& info);
void printStockpileAmmo (const StockpileAmmo& ammo);
void printArmoryGun     (const ArmoryGun& gun);
void printAmountOfAmmo  (const AmountOfAmmo& ammo);
void printEvent         (const ShootingEvent& event);

}
