#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIData.h"
#include "UI/UIEvents.h" 
#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

                    
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
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
            bool& submitted,
            const ImVec2& buttonSize
        );
    void editEvent_metadataWindow(
            ScreenData::Edit::ItemBuffers::Event::MetadataWindow& data,
            const ShootingEventMetadata& oldInfo,
            size_t notesSize,
            const std::set<Location>&,
            const std::set<ShootingEventType>&
        );
    void editEvent_gunsAndAmmoWindow(
            ScreenData::Edit::ItemBuffers::Event& data, 
            ShootingEvent& event,
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
            const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>& 
        );
    void editEvent_viewGunsUsedWindow(
            const ShootingEvent& event,
            GunMetadata&    selectedGun,
            bool&           isGunValid,
            bool&           removeGunPressed,
            ImVec2&         tableSize,
            const float     minTableWidth,
            const float     maxTableWidth,
            const ImVec2&   windowSize,
            const ImVec2&   buttonSize
        );
    void editEvent_viewAmmoUsedWindow(
            const ShootingEvent&    event,
            const GunMetadata&      selectedGun,
            AmmoMetadata&           selectedAmmo,
            bool&                   isAmmoValid,
            bool&                   removeAmmo,
            const bool              isGunValid,
            const ImVec2&           tableSize,
            const ImVec2&           windowSize,
            const ImVec2&           buttonSize
        );
    void editEvent_addGun(
            ScreenData::Edit::ItemBuffers::Event::GunsAndAmmoWindow::AddGunWindow& data, 
            ShootingEvent& event, 
            const std::map<Cartridge, std::map<GunMetadata,  ArmoryGun>>&,
            const ImVec2& tableSize
        );
    void editEvent_addAmmoToGun (
            ScreenData::Edit::ItemBuffers::Event::GunsAndAmmoWindow::AddAmmoWindow& data, 
            GunTrackingAmmoUsed& selected,
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
            const ImVec2& tableSize
        );

}   // End SC::UI::ScreenData

