#pragma once

#include "Backend/DatabaseItems.h"
#include "Backend/Database.h"

#include <LAS/HelperFunctions.h>
#include <LAS/json.h>

#include <sstream>
#include <chrono>
#include <fstream>

#include <LAS/Logging.h>    // testing
#include <iostream> // for testing

namespace ShooterCentral {

void to_json        (LAS::json&, const ShootingEventMetadata& ); 
void from_json      (const LAS::json&, ShootingEventMetadata& );

void to_json        (LAS::json&, const AmmoMetadata& );
void from_json      (const LAS::json&, AmmoMetadata& );

void to_json        (LAS::json&, const GunMetadata& );
void from_json      (const LAS::json&, GunMetadata& );

void to_json        (LAS::json&, const AmountOfAmmo& );
void from_json      (const LAS::json&, AmountOfAmmo& );

void to_json        (LAS::json&, const StockpileAmmo& );
void from_json      (const LAS::json&, StockpileAmmo& );

void to_json        (LAS::json&, const GunTrackingAmmoUsed& );
void from_json      (const LAS::json&, GunTrackingAmmoUsed& );

void to_json        (LAS::json&, const ArmoryGun& );
void from_json      (const LAS::json&, ArmoryGun& );

void to_json        (LAS::json&, const ShootingEvent& );
void from_json      (const LAS::json&, ShootingEvent& );

// Read/Write individual objects
bool write (std::string directory, const StockpileAmmo& ); 
bool write (std::string directory, const ArmoryGun& ); 
bool write (std::string directory, const ShootingEvent& ); 

bool read (std::ifstream&, ShootingEvent& );
bool read (std::ifstream&, ArmoryGun& );
bool read (std::ifstream&, StockpileAmmo& );

// Read/Write for directory/list
bool readEvents (Database&, const std::filesystem::path& directory); // Need to find better spot
                                                                     
// Overwrites existing guns/ammo. Must re-associate events afterward                                                                
bool readGuns   (Database&, const std::filesystem::path& directory); 
bool readAmmo   (Database&, const std::filesystem::path& directory);

std::string makeFileName    (std::string, const GunMetadata& );
std::string makeFileName    (std::string, const AmmoMetadata& );
std::string makeFileName    (std::string, const ShootingEventMetadata& );

/*
bool write (std::string directory, const GunMetadata& data);
bool write (std::string directory, const AmountOfAmmo& data);
bool write (std::string directory, const ShootingEvent& data);
*/




}

