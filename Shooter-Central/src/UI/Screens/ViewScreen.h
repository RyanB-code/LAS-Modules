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
    void selectEventWindow(
            const std::map<ShootingEventMetadata, ShootingEvent>& events,
            ShootingEventMetadata& selectedEvent,
            GunMetadata& selectedGun,
            ImVec2& tableSize,
            bool& isEventValid,
            float minTableWidth,
            float maxTableWidth,
            const ImVec2& windowSize,
            const ImVec2& buttonSize
        );
    void selectedEventInfoWindow(
            const ShootingEventMetadata& selectedEvent,
            const bool& isEventValid,
            const ImVec2& windowSize
        );
    void selectedEventGunsUsedWindow(
            const std::map<ShootingEventMetadata, ShootingEvent>& events,
            const ShootingEventMetadata& selectedEvent,
            GunMetadata& selectedGun,
            ImVec2& tableSize,
            const bool isEventValid,
            float minTableWidth,
            float maxTableWidth,
            const ImVec2& windowSize
        );
}

namespace StockpileWindow {
    void main(
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
            const std::map<Cartridge, int>& cartridgeList,
            ScreenData::View::StockpileWindow& tabData
        );
    void selectCartridgeWindow(
            const std::map<Cartridge, int>& cartridgeList,
            Cartridge&      selectedCartridge,
            AmmoMetadata&   selectedAmmo,
            ImVec2&         tableSize,
            bool&           isCartridgeValid,
            bool&           isAmmoValid,
            float           minTableWidth,
            float           maxTableWidth,
            const ImVec2&   windowSize,
            const ImVec2&   buttonSize
        );
    void selectAmmoWindow(
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
            AmmoMetadata&       selectedAmmo,
            const Cartridge&    selectedCartridge,
            const ImVec2&       tableSize,
            bool&               isAmmoValid,
            bool&               showInactive,
            const bool          isCartridgeValid,
            const ImVec2&       windowSize,
            const ImVec2&       buttonSize
        );
    void selectedAmmoInfoWindow(
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
            const AmmoMetadata& selectedAmmo,
            const bool& isAmmoValid,
            const ImVec2& windowSize
        );
    void selectedAmmoGunsUsed(
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
            const AmmoMetadata& selectedAmmo,
            const ImVec2&       tableSize,
            const bool&         isAmmoValid,
            const ImVec2&       windowSize
        );
}


}   // End SC::UI::View namespace
