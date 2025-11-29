#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"
#include "CommonItems.h"

#include "UI/UIHelperElements.h"
#include "UI/UIData.h"
#include "UI/UIControl.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <map>
#include <memory>

#include <iostream> // For testing
                    

// OPTIMIZE
//  1. Use Iterators instead of copying for selected items for list
//      - especially for GunTrackingAmmoUsed in View::eventsTab() items (lots of copying)
//  2. Make SelectableGunMetadata with reset(), operator== compare to empty, for ease of use with UI  
//  3. Prevent double adding of items for event. make buffer just be a shooting event
//  4. Change window sizing for add_Event_GunsUsed tab since it /4 twice 

//  WIP
//  - Finalizing adding ammo to event, applying to stockpile and guns on the review screen
//      the review screen
//

namespace ShooterCentral::UI {

namespace Home {
    void main                               (const Database&, ScreenData::Home&, const UnsavedChanges&);
    void gunWindow                          (const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>&, GunMetadata& selected);
    
    void gunWindow_selectedGunInformation   (const ArmoryGun& );
    void eventsWindow                       (const std::map<ShootingEventMetadata, ShootingEvent>&, ShootingEventMetadata& selected );
    void stockpileWindow                    (const std::map<Cartridge, int>&, Cartridge& selected);
}


namespace View {
    void main                               (const Database&, ScreenData::View& );
    void gunTab                             (const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>&, ScreenData::View::GunTab& data );
    void gunTab_selectedGunInformation      (const ArmoryGun& );
    void gunTab_ammoUsedWindow              (const ArmoryGun& );
    void gunTab_eventsWindow                (const ArmoryGun&, ShootingEventMetadata& selectedEvent);

    void eventsTab                          (const std::map<ShootingEventMetadata, ShootingEvent>&, ScreenData::View::EventTab& data);
    void eventsTab_selectedEventInformation (const ShootingEvent& event);
    void eventsTab_gunsUsed                 (const ShootingEvent& event, GunTrackingAmmoUsed& selectedGun );

    void stockpileTab(
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
            const std::map<Cartridge, int>& cartridgeList,
            ScreenData::View::StockpileTab& tabData
            );
    void stockpileTab_selectedAmmoInformation   (const StockpileAmmo& );
    void stockpileTab_selectedAmmoGunsUsed      (const std::set<GunMetadata>& );
}


namespace Add {
    void    main                    (const Database&, ScreenData::Add& );
    void    showExistingItemsWindow (const Database&, const SubItem& selected, ImVec2 size); 
    void    addItemWindow           (const Database&, ScreenData::Add& );

    void    add_Manufacturer        (char* textBuf, size_t size);
    void    add_Cartridge           (char* textBuf, size_t size);
    void    add_EventType           (char* textBuf, size_t size);
    void    add_WeaponType          (char* textBuf, size_t size);
    void    add_Location            (char* textBuf, size_t size);

    void    add_Event(  
                ScreenData::Add::EventBuffer& buffer, 
                size_t size,            
                const std::set<Location>&,
                const std::set<ShootingEventType>&,
                const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
                const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>&
            );
    void    add_Event_InformationTab(
                ScreenData::Add::EventBuffer& buffer, 
                size_t size,            
                const std::set<Location>&,
                const std::set<ShootingEventType>&
            );
    bool    add_Event_verifyInformation(const ScreenData::Add::EventBuffer& buffer);
    void    add_Event_GunsUsedTab(
                std::vector<GunTrackingAmmoUsed>& gunsUsed,
                std::vector<GunTrackingAmmoUsed>::iterator& selectedGun,
                ScreenData::Add::EventTab_AddItemsScreen& currentTab,
                const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&,
                const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>& 
            );
    // Returns true if an insertion happened so itrs can be updated in Add::EventBuffer
    bool    add_Event_Gun (
                const std::map<Cartridge, std::map<GunMetadata,  ArmoryGun>>&,
                std::vector<GunTrackingAmmoUsed>& gunsUsed
            );
    bool    add_Event_AmmoForGun (
                const std::map<AmmoMetadata,  StockpileAmmo>&,
                AmountOfAmmo& ammo 
            );

}

// Table drawing
namespace Tables{
    void selectable_Cartridges(
            const std::map<Cartridge, int>& cartridges, 
            Cartridge& selected,
            ImVec2 size
        );
    void selectable_SingleCartridgeAmmo(
            const std::map<AmmoMetadata, StockpileAmmo>& list, 
            AmmoMetadata& selected,
            ImVec2 size 
            );
    void selectable_AllAmmo(
            const std::map<Cartridge, std::map<AmmoMetadata, StockpileAmmo>>& list,       
            AmmoMetadata& selected,
            ImVec2 size
        );
    void selectable_Guns(
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>&, 
            GunMetadata&,
            ImVec2  
        );
    void selectable_Events( 
            const std::map<ShootingEventMetadata, ShootingEvent>& events, 
            ShootingEventMetadata& selected,
            ImVec2 size
        );
    void selectable_Events( 
            const std::set<ShootingEventMetadata>& events, 
            ShootingEventMetadata& selected,
            ImVec2 size
        );
    void selectable_EventGunsUsed(
            const std::vector<GunTrackingAmmoUsed>& list, 
            GunTrackingAmmoUsed& gun,
            ImVec2 size     
        );
    void selectable_gunMetadata(   
            const std::set<GunMetadata>& guns, 
            GunMetadata& selected,
            ImVec2 size
        );
    void selectable_gunMetadata(
            std::vector<GunTrackingAmmoUsed>& guns,
            std::vector<GunTrackingAmmoUsed>::iterator& selected,
            ImVec2 size
        );
    void selectable_AmmoMetadata(
            const std::map<AmmoMetadata, StockpileAmmo>& list, 
            AmmoMetadata& selected,
            ImVec2 size 
        );


    
    void amountOfAmmo   (const std::vector<AmountOfAmmo>& ammoUsed,                         ImVec2 size);
    void amountOfAmmo   (const std::map<AmmoMetadata, AmountOfAmmo>& ammoUsed,              ImVec2 size);
}
namespace ListBoxes{
    void cartridges     (const std::set<Cartridge>&,            ImVec2 size);
    void manufacturers  (const std::set<Manufacturer>&,         ImVec2 size);
    void eventLocations (const std::set<Location>&,             ImVec2 size);
    void eventTypes     (const std::set<ShootingEventType>&,    ImVec2 size); 
    void weaponTypes    (const std::set<WeaponType>&,           ImVec2 size);
}

namespace ComboBoxes{
    void  category    (Category& selected);
    void  subItem     (SubItem& selected);
}



}   // End UI namespace
