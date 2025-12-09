#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIData.h"
#include "UI/UIEvents.h" 
#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing


namespace ShooterCentral::UI::View {

void main(const Database&, ScreenData::View& );

namespace ArmoryWindow {
    void main(
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
            ScreenData::View::ArmoryWindow& screenData 
        );
    void selectGunWindow(
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
            GunMetadata& selectedGun,
            ImVec2& tableSize,
            bool& isGunValid,
            bool& showInactive,
            float minTableWidth,
            float maxTableWidth,
            const ImVec2& windowSize,
            const ImVec2& buttonSize
        );
    void selectedGunInfoWindow(
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
            const GunMetadata& gunInfo,
            const bool& isGunValid,
            const ImVec2& windowSize
        );
    void selectedGunEventsWindow(
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
            const GunMetadata& selectedGun,
            ImVec2& tableSize,
            const bool isGunValid,
            float minTableWidth,
            float maxTableWidth,
            const ImVec2& windowSize
        );
    void selectedGunAmmoWindow(
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
            const GunMetadata& selectedGun,
            ImVec2& tableSize,
            const bool isGunValid,
            float minTableWidth,
            float maxTableWidth,
            const ImVec2& windowSize
        );
}

namespace EventsWindow {
    void main(
            const std::map<ShootingEventMetadata, ShootingEvent>&, 
            ScreenData::View::EventsWindow& data
        );
    void selectedEventInformation (const ShootingEvent& event);
}

namespace StockpileWindow {
    void main(
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
            const std::map<Cartridge, int>& cartridgeList,
            ScreenData::View::StockpileWindow& tabData
        );
    void selectedAmmoInformation   (const StockpileAmmo& );
}


}   // End SC::UI::View namespace
