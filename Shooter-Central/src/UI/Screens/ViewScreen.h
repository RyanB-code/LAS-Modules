#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing


namespace ShooterCentral::UI::ScreenData {

struct View {
    struct MainWindow{
        Category        category                { Category::NONE }; 
        std::string     categoryComboBoxText    { };
    };
    struct ArmoryWindow{
        GunMetadata             selectedGun;

        bool verticalLayout { false };

        static constexpr ImVec2 deselectButtonSize  { 100, 40 };
        static constexpr ImVec2 minWinSize          { 400, 600 };
        static constexpr ImVec2 minTableSize        { 400, 300 };
        static constexpr float  maxTableWidth       { 800 };     

        ImVec2 topWindowSize        { minWinSize };
        ImVec2 bottomWindowSize     { minWinSize };
        ImVec2 topTableSize         { minTableSize.x, 400 };
        ImVec2 bottomTableSize      { minTableSize.x, 400 };
    };

    struct EventsWindow {
        ShootingEventMetadata   selectedEvent   { };
        GunMetadata             selectedGun     { };

        bool verticalLayout { false };

        static constexpr ImVec2 deselectButtonSize  { 100, 40 };
        static constexpr ImVec2 minWinSize          { 400, 600 };
        static constexpr ImVec2 minTableSize        { 400, 300 };
        static constexpr float  maxTableWidth       { 600 };     


        ImVec2 topWindowSize        { minWinSize };
        ImVec2 bottomWindowSize     { minWinSize };
        ImVec2 topTableSize         { minTableSize.x, 400 };
        ImVec2 bottomTableSize      { minTableSize.x, 400 };
    };
    struct StockpileWindow{
        Cartridge       selectedCartridge   { };
        AmmoMetadata    selectedAmmo        { };
        
        bool selectedCartridgeValid { false };
        bool selectedAmmoValid      { false };

        bool verticalLayout { false };

        static constexpr ImVec2 deselectButtonSize  { 100, 40 };
        static constexpr ImVec2 minWinSize          { 400, 600 };
        static constexpr ImVec2 minTableSize        { 400, 300 };
        static constexpr float  maxTableWidth       { 600 };     


        ImVec2 windowSize   { minWinSize };
        ImVec2 tableSize    { minTableSize.x, 400 };
    };

    MainWindow      mainWindow      { };
    EventsWindow    eventsWindow    { };
    ArmoryWindow    armoryWindow    { };
    StockpileWindow stockpileWindow { };
};

}   // End SC::UI::ScreenData

namespace ShooterCentral::UI::View {

void main(const Database&, ScreenData::View& );

namespace ArmoryWindow {
    void main(
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
            ScreenData::View::ArmoryWindow& screenData 
        );
    void selectedGunInformation (const ArmoryGun& );
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
