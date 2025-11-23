#pragma once

#include "Backend/DatabaseItems.h"

#include <LAS/HelperFunctions.h>
#include <LAS/json.h>

#include <sstream>
#include <chrono>
#include <fstream>

#include <LAS/Logging.h>    // testing
#include <iostream> // for testing

namespace ShooterCentral {

void to_json        (LAS::json& j, const ShootingEventMetadata& data); 
void from_json      (const LAS::json& j, ShootingEventMetadata& data);

void to_json        (LAS::json& j, const AmmoMetadata& data);
void from_json      (const LAS::json& j, AmmoMetadata& data);

void to_json        (LAS::json& j, const GunMetadata& data);
void from_json      (const LAS::json& j, GunMetadata& data);

void to_json        (LAS::json& j, const AmountOfAmmo& data);
void from_json      (const LAS::json& j, AmountOfAmmo& data);


bool readDir_Events(const std::string& dir);

bool write (std::string directory, const StockpileAmmo& data); 
bool write (std::string directory, const ArmoryGun& data); 
bool write (std::string directory, const ShootingEvent& data); 


std::string makeFileName    (std::string directory, const GunMetadata& data);
std::string makeFileName    (std::string directory, const AmmoMetadata& data);
std::string makeFileName    (std::string directory, const ShootingEventMetadata& data);

/*
bool write (std::string directory, const GunMetadata& data);
bool write (std::string directory, const AmountOfAmmo& data);
bool write (std::string directory, const ShootingEvent& data);
*/




}

