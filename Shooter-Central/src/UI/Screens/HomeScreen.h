#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"
#include "CommonItems.h"

#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing
                    
                    
namespace ShooterCentral::UI::ScreenData {

struct Home{
    bool showGuns       { true };
    bool showEvents     { true };
    bool showStockpile  { true };

    GunMetadata             selectedGun         { };
    ShootingEventMetadata   selectedEvent       { };
    Cartridge               selectedCartridge   { };
};

}   // End SC::UI::ScreenData

namespace ShooterCentral::UI::Home {

    void main                               (const Database&, ScreenData::Home&, const UnsavedChanges&);
    void gunWindow                          (const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>&, GunMetadata& selected);
    
    void gunWindow_selectedGunInformation   (const ArmoryGun& );
    void eventsWindow                       (const std::map<ShootingEventMetadata, ShootingEvent>&, ShootingEventMetadata& selected );
    void stockpileWindow                    (const std::map<Cartridge, int>&, Cartridge& selected);

}   // End SC::UI::Home namespace
