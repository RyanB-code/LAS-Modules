#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing


namespace ShooterCentral::UI::ScreenData {

struct Add {
    static constexpr int MAX_CHAR_INPUT { 64 };

    struct SubItemBuffers {
        char weaponType[MAX_CHAR_INPUT]     = "";
        char manufacturer[MAX_CHAR_INPUT]   = "";
        char cartridge[MAX_CHAR_INPUT]      = "";
        char location[MAX_CHAR_INPUT]       = "";
        char eventType[MAX_CHAR_INPUT]      = "";
    }; 
    struct EventWindow {
        struct MetadataWindow {
            ShootingEventType   selectedET          { };
            Location            selectedLocation    { };

            char notes[MAX_CHAR_INPUT] = "";
            int day     { 0 };
            int month   { 0 };
            int year    { 0 };
        };
        struct GunsAndAmmoWindow {
            struct AddGunWindow {
                GunMetadata selectedGun { };

                bool selectedGunValid { false };
                static constexpr ImVec2 buttonSize { 100, 40 };
            };

            static constexpr ImVec2 buttonSize          { 100, 40 };
            static constexpr ImVec2 minWinSize          { 400, 600 };
            static constexpr ImVec2 minTableSize        { 300, 400 };
            static constexpr float  maxTableWidth       { 800 };     
            static constexpr float  optionsWinHeight    { 100 };

            GunMetadata    selectedGun     { };
            AmmoMetadata   selectedAmmo    { };

            AddGunWindow addGunWindow { };

            bool verticalLayout     { false };
            bool selectedGunValid   { false };
           
            ImVec2 optionsWinSize   { minWinSize.x, optionsWinHeight };
            ImVec2 mainWindowSize   { minWinSize };
            ImVec2 viewWindowSize   { minWinSize };
            
            ImVec2 mainTableSize    { minTableSize };
            ImVec2 viewTableSize    { minTableSize };
        };

        MetadataWindow      metadataWindow      { };
        GunsAndAmmoWindow   gunsAndAmmoWindow   { };

        ShootingEvent event { };
        bool eventInfoVerified          { false };
        bool triedToVerifyEventInfo     { false };
        bool gunsVerified       { false };

        static constexpr ImVec2 verifyButtonSize { 150, 40 };
    };

    static constexpr ImVec2 deselectButtonSize  { 100, 40 };
    static constexpr ImVec2 minWinSize          { 400, 600 };
    static constexpr ImVec2 minTableSize        { 300, 400 };
    static constexpr float  maxTableWidth       { 800 };     

    EventWindow     eventWindow     { };

    Category        category        { Category::NONE }; 
    SubItem         subItem         { SubItem::NONE };
    SubItemBuffers  subItemBuffers  { };

    bool verticalLayout { false };

    ImVec2 mainWindowSize   { minWinSize };
    ImVec2 infoWindowSize   { minWinSize };
    ImVec2 infoTableSize    { minTableSize };
 

};

}   // End Sc::UI::ScreenData

namespace ShooterCentral::UI::Add {
    
void main                    (const Database&, ScreenData::Add& );
void showExistingItemsWindow (const Database&, const SubItem& selected, ImVec2 size); 
void addItemWindow           (const Database&, ScreenData::Add& );

void add_Manufacturer        (char* textBuf, size_t size);
void add_Cartridge           (char* textBuf, size_t size);
void add_EventType           (char* textBuf, size_t size);
void add_WeaponType          (char* textBuf, size_t size);
void add_Location            (char* textBuf, size_t size);

namespace EventWindow {

    void main(  
            ScreenData::Add::EventWindow& eventWindow, 
            size_t notesSize,            
            const std::set<Location>&,
            const std::set<ShootingEventType>&,
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
            const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>&
        );
    void eventMetadataWindow(
            ScreenData::Add::EventWindow& buffer, 
            size_t notesSize,            
            const std::set<Location>&,
            const std::set<ShootingEventType>&
        );
    bool verifyMetadata(
            Location& location,
            ShootingEventType& et,
            ymd date
        );
    void gunsAndAmmoWindow(
            ScreenData::Add::EventWindow& data, 
            ShootingEvent& event,
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
            const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>& 
        );
    void addGun(
            ScreenData::Add::EventWindow::GunsAndAmmoWindow::AddGunWindow& data, 
            ShootingEvent& event, 
            const std::map<Cartridge, std::map<GunMetadata,  ArmoryGun>>&,
            const ImVec2& tableSize
        );
    void addAmmoToGun (
            GunMetadata& selected,
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&
        );
}   // End EventWindow

}   // End SC::UI::Add


