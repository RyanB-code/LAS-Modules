#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing


namespace ShooterCentral::UI::ScreenData {

struct View {
    struct EventTab {
        ShootingEventMetadata selectedEvent;
        GunTrackingAmmoUsed   selectedGunAmmoUsed;
    };

    struct GunTab{
        GunMetadata             selectedGun;
        ShootingEventMetadata   selectedEvent;
    };
    struct StockpileTab{
        Cartridge       selectedCartridge { };
        AmmoMetadata    selectedAmmo;
    };

    Category        category                { Category::NONE }; 
    std::string     categoryComboBoxText    { };

    EventTab        eventTab                { };
    GunTab          gunTab                  { };
    StockpileTab    stockpileTab            { };
};

}   // End SC::UI::ScreenData

namespace ShooterCentral::UI::View {

void main                               (const Database&, ScreenData::View& );
void gunTab                             (const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>&, ScreenData::View::GunTab& data );
void gunTab_selectedGunInformation      (const ArmoryGun& );
void gunTab_ammoUsedWindow              (const ArmoryGun& );
void gunTab_eventsWindow                (const ArmoryGun&, ShootingEventMetadata& selectedEvent);

void eventsTab                          (const std::map<ShootingEventMetadata, ShootingEvent>&, ScreenData::View::EventTab& data);
void eventsTab_selectedEventInformation (const ShootingEvent& event);
void eventsTab_gunsUsed                 (const ShootingEvent& event, GunTrackingAmmoUsed& selectedGun );

void stockpileTab(
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
        const std::map<Cartridge, int>& cartridgeList,
        ScreenData::View::StockpileTab& tabData
        );
void stockpileTab_selectedAmmoInformation   (const StockpileAmmo& );
void stockpileTab_selectedAmmoGunsUsed      (const std::set<GunMetadata>& );


}   // End SC::UI::View namespace
