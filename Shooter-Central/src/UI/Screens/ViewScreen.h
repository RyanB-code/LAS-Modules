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
        ShootingEventMetadata   selectedEvent;

        static constexpr ImVec2 MIN_WIN_SIZE        { 400, 600 };
        static constexpr ImVec2 MIN_TABLE_SIZE      { 400, 300 };
        static constexpr ImVec2 deselectButtonSize  { 100, 40 };
        static constexpr float  MAX_TOP_TABLE_WIDTH { 600 };     

        bool verticalLayout { false };

        ImVec2 topWindowSize        { MIN_WIN_SIZE };
        ImVec2 bottomWindowSize     { MIN_WIN_SIZE };
        ImVec2 topTableSize         { MIN_TABLE_SIZE.x, 400 };
        ImVec2 bottomTableSize      { MIN_TABLE_SIZE.x, 400 };
    };

    struct EventsWindow {
        ShootingEventMetadata selectedEvent;
        GunTrackingAmmoUsed   selectedGunAmmoUsed;
    };
    struct StockpileWindow{
        Cartridge       selectedCartridge   { };
        AmmoMetadata    selectedAmmo        { };
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
    void ammoUsedWindow         (const ArmoryGun& );
    void eventsWindow           (const ArmoryGun&, ShootingEventMetadata& selectedEvent);
}

void eventsTab(
        const std::map<ShootingEventMetadata, 
        ShootingEvent>&, 
        ScreenData::View::EventsWindow& data
    );
void eventsTab_selectedEventInformation (const ShootingEvent& event);
void eventsTab_gunsUsed                 (const ShootingEvent& event, GunTrackingAmmoUsed& selectedGun );

void stockpileTab(
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
        const std::map<Cartridge, int>& cartridgeList,
        ScreenData::View::StockpileWindow& tabData
    );
void stockpileTab_selectedAmmoInformation   (const StockpileAmmo& );
void stockpileTab_selectedAmmoGunsUsed      (const std::set<GunMetadata>& );


}   // End SC::UI::View namespace
