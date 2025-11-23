#pragma once

#include "Backend/DatabaseItems.h"

#include <LAS/json.h>
#include <LAS/HelperFunctions.h>

#include <sstream>
#include <chrono>
#include <fstream>

namespace ShooterCentral {

void to_json        (LAS::json& j, const ShootingEventMetadata& data);  // WROTE PROPERLY
void from_json      (const LAS::json& j, ShootingEventMetadata& data);

void to_json        (LAS::json& j, const AmmoMetadata& data);
void from_json      (const LAS::json& j, AmmoMetadata& data);

void to_json        (LAS::json& j, const GunMetadata& gun);

void to_json        (LAS::json& j, const AmountOfAmmo& data);


std::string makeFileName    (std::string directory, const GunMetadata& data);
std::string makeFileName    (std::string directory, const AmmoMetadata& data);
std::string makeFileName    (std::string directory, const ShootingEventMetadata& data);

bool write (std::string directory, const ShootingEventMetadata& data); // Testing one

/*
bool write (std::string directory, const GunMetadata& data);
bool write (std::string directory, const AmountOfAmmo& data);
bool write (std::string directory, const ShootingEvent& data);
*/




}

