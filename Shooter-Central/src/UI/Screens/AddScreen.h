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
    enum class EventTab {
        INFO,
        GUNS_AND_AMMO,
        REVIEW_AND_SUBMIT
    };
    enum class EventTab_AddItemsScreen {
        GUN,
        AMMO
    };
    struct EventBuffer {
        Location            location        { "" };
        ShootingEventType   eventType       { "" };

        std::vector<GunTrackingAmmoUsed>            gunsUsed { };
        std::vector<GunTrackingAmmoUsed>::iterator  selectedGun { gunsUsed.end() };

        char notes[MAX_CHAR_INPUT] = "";
        int day     { 0 };
        int month   { 0 };
        int year    { 0 };

        bool applyToStockpile   { true };
        bool applyToArmory      { true };

        bool eventInfoVerified  { false };
        bool gunsVerified       { false };
        EventTab currentTab                         { EventTab::INFO };
        EventTab_AddItemsScreen addItemsCurrentTab  { EventTab_AddItemsScreen::GUN };
    };

    Category        category        { Category::NONE }; 
    SubItem         subItem         { SubItem::NONE };
    SubItemBuffers  subItemBuffers  { };
    EventBuffer     eventBuffer     { };
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

void add_Event(  
        ScreenData::Add::EventBuffer& buffer, 
        size_t size,            
        const std::set<Location>&,
        const std::set<ShootingEventType>&,
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
        const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>&
    );
void add_Event_InformationTab(
        ScreenData::Add::EventBuffer& buffer, 
        size_t size,            
        const std::set<Location>&,
        const std::set<ShootingEventType>&
    );
bool add_Event_verifyInformation(const ScreenData::Add::EventBuffer& buffer);
void add_Event_GunsUsedTab(
        std::vector<GunTrackingAmmoUsed>& gunsUsed,
        std::vector<GunTrackingAmmoUsed>::iterator& selectedGun,
        ScreenData::Add::EventTab_AddItemsScreen& currentTab,
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


}   // End SC::UI::Add


