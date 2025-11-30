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
    
    struct MainWindow {
        static constexpr ImVec2 MIN_WIN_SIZE  { 500, 500 };

        ImVec2  oneThirdWindowSize  { };        // 3 window panes for the 3 sections.
        ImVec2  childWindowSize     { };
        bool    verticalLayout      { false };
    };
    struct GunWindow {
        ImVec2      tableSize        { 0, 300 };
        GunMetadata selectedGun      { };
    };
    struct EventsWindow {
        ImVec2 tableSize        { 0, 300 };
        ShootingEventMetadata selectedEvent { };
    };
    struct StockpileWindow{
        ImVec2      tableSize        { 0, 300 };
        Cartridge   selectedCartridge { };
    };


    MainWindow      mainWindow      { };
    EventsWindow    eventsWindow    { };
    GunWindow       gunWindow       { };
    StockpileWindow stockpileWindow { };
};

}   // End SC::UI::ScreenData

namespace ShooterCentral::UI::Home {

    void main(
            const Database&, 
            ScreenData::Home&, 
            const UnsavedChanges&
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
    void eventsWindow_selectedEventInformation(const ShootingEvent& event);

    void stockpileWindow (
            const std::map<Cartridge, int>&, 
            UI::ScreenData::Home::StockpileWindow& screenData 
        );

}   // End SC::UI::Home namespace
