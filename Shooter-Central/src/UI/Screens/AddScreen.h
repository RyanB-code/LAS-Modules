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
    bool verifyEvent(const ShootingEvent& event);
}   // End EventWindow

}   // End SC::UI::Add


