#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIData.h"
#include "UI/UIEvents.h" 
#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing
                    
                    
namespace ShooterCentral::UI::Edit {

    void main   (const Database&, ScreenData::Edit& );
    void selectExistingItemWindow (
            ScreenData::Edit::ItemBuffers& buffers,
            const Database& database, 
            const SubItem& selected, 
            const ImVec2& size
        );
    void editItemWindow(
            ScreenData::Edit& data,
            const Database& database,
            SubItem selectedItem
        );

    void editMetadataItem (
            char* oldInfo, 
            char* textBuf, 
            size_t size,
            bool& submitted,
            const ImVec2& buttonSize
        );
    void editGun(
            char* nameBuffer,
            size_t size,
            const GunMetadata& oldInfo,
            GunMetadata& newInfo,
            const std::set<WeaponType>& weaponTypes,
            bool& submitted,
            const ImVec2& buttonSize
        );
    void editAmmo(
            char* nameBuffer,
            size_t size,
            const AmmoMetadata& oldInfo,
            AmmoMetadata& newInfo,
            const std::set<Manufacturer>& manufacturers,
            bool& submitted,
            const ImVec2& buttonSize
        );
    void editEvent(
            ScreenData::Edit::ItemBuffers& data, 
            size_t notesSize,            
            const std::set<Location>& locations,
            const std::set<ShootingEventType>& eventTypes,
            const std::map<Cartridge, std::map<AmmoMetadata, StockpileAmmo>>& stockpile,
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory
        );
    void eventMetadataWindow(
            ScreenData::Edit::ItemBuffers::EventMetadataBuffers& data,
            const ShootingEventMetadata& oldInfo,
            size_t notesSize,
            const std::set<Location>&,
            const std::set<ShootingEventType>&
        );

}   // End SC::UI::ScreenData

