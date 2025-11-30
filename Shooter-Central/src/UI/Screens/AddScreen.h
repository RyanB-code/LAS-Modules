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

        MetadataWindow metadataWindow { };

        ShootingEvent event { };
        bool eventInfoVerified          { false };
        bool triedToVerifyEventInfo     { false };
        bool gunsVerified       { false };

        static constexpr ImVec2 verifyButtonSize { 150, 40 };
    };


    EventWindow     eventWindow     { };

    Category        category        { Category::NONE }; 
    SubItem         subItem         { SubItem::NONE };
    SubItemBuffers  subItemBuffers  { };

    bool verticalLayout { false };

    static constexpr ImVec2 deselectButtonSize  { 100, 40 };
    static constexpr ImVec2 minWinSize          { 400, 600 };
    static constexpr ImVec2 minTableSize        { 300, 400 };
    static constexpr float  maxTableWidth       { 800 };     

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
    void eventMetadata(
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
    void gunsAndAmmo(
            ShootingEvent& event,
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
            const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>& 
        );


    // Returns true if an insertion happened so itrs can be updated in Add::EventBuffer
    bool add_Event_Gun (
            const std::map<Cartridge, std::map<GunMetadata,  ArmoryGun>>&,
            std::vector<GunTrackingAmmoUsed>& gunsUsed
        );
    bool add_Event_AmmoForGun (
            const std::map<AmmoMetadata,  StockpileAmmo>&,
            AmountOfAmmo& ammo 
        );
}   // End EventWindow

}   // End SC::UI::Add


