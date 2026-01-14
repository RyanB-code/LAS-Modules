#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIData.h"
#include "UI/UIEvents.h" 
#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing
                    

namespace ShooterCentral::UI::Home {

    void main(
            const Database&, 
            ScreenData::Home&
        );
    void gunWindow(
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>&, 
            UI::ScreenData::Home::GunWindow& screenData 
        );
    void gunWindow_selectedGunInformation   (const ArmoryGun& );
    

    void eventsWindow(
            const std::map<ShootingEventMetadata, ShootingEvent>&, 
            UI::ScreenData::Home::EventsWindow& screenData 
        );
    void eventsWindow_selectedEventInformation(const ShootingEventMetadata& );

    void stockpileWindow (
            const std::map<Cartridge, int>&, 
            UI::ScreenData::Home::StockpileWindow& screenData 
        );

}   // End SC::UI::Home namespace
