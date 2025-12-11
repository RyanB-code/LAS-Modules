#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIData.h"
#include "UI/UIEvents.h" 
#include "UI/UIHelperElements.h"


#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing


namespace ShooterCentral::UI::Add {
    
void main                    (const Database&, ScreenData::Add& );
void showExistingItemsWindow (const Database&, const SubItem& selected, ImVec2 size); 
void addItemWindow           (const Database&, ScreenData::Add& );

void add_Manufacturer        (char* textBuf, size_t size);
void add_Cartridge           (char* textBuf, size_t size);
void add_EventType           (char* textBuf, size_t size);
void add_WeaponType          (char* textBuf, size_t size);
void add_Location            (char* textBuf, size_t size);

namespace GunWindow {
    void main(
            ScreenData::Add::GunWindow& gunWindow, 
            const std::set<Cartridge>&  cartridges,
            const std::set<WeaponType>& weaponTypes
        );
    void gunMetadataWindow(
            ScreenData::Add::GunWindow& gunWindow, 
            const std::set<Cartridge>&  cartridges,
            const std::set<WeaponType>& weaponTypes,
            size_t nameSize
        );

}

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
            ScreenData::Add::EventWindow::MetadataWindow& data, 
            ShootingEvent& event,
            size_t notesSize,            
            const std::set<Location>&,
            const std::set<ShootingEventType>&
        );
    void infoWindow(
            const ShootingEventMetadata& event,
            bool&           submitted,
            ImVec2&         subWindowSize,
            const bool      attemptedVerify,
            const ImVec2&   minWinSize,
            const ImVec2&   windowSize,
            const ImVec2&   buttonSize
            );
    bool verifyMetadata(
            const Location& location,
            const ShootingEventType& et,
            const ymd& date
        );
    void gunsAndAmmoWindow(
            ScreenData::Add::EventWindow& data, 
            ShootingEvent& event,
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
            const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>& 
        );
    void viewGunsUsedWindow(
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
    void viewAmmoUsedWindow(
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
    void addGun(
            ScreenData::Add::EventWindow::GunsAndAmmoWindow::AddGunWindow& data, 
            ShootingEvent& event, 
            const std::map<Cartridge, std::map<GunMetadata,  ArmoryGun>>&,
            const ImVec2& tableSize
        );
    void addAmmoToGun (
            ScreenData::Add::EventWindow::GunsAndAmmoWindow::AddAmmoWindow& data, 
            GunTrackingAmmoUsed& selected,
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
            const ImVec2& tableSize
        );
    void review(
            ScreenData::Add::EventWindow::ReviewWindow& data,
            ShootingEvent& event
        );
    void reviewOptionsWindow(
            bool& submitEvent,
            bool& resetInputs,
            bool& applyToArmory,
            bool& applyToStockpile,
            const ImVec2& windowSize,
            const ImVec2& buttonSize
        );
    void reviewEventInfoWindow(
            const ShootingEventMetadata& info,
            const bool isEventValid,
            const ImVec2& windowSize
        );
}   // End EventWindow

}   // End SC::UI::Add


