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
//  
//  4. Change window sizing for add_Event_GunsUsed tab since it /4 twice 
//  5. Fix popup close() command
//  6. fix changing Home::stockpileWindow() view more information command to change to view
//  7. make separaor text for event add gus screen

//  WIP
//  - Finalizing adding ammo to event, applying to stockpile and guns on the review screen
//      the review screen
//  - search for TODO to see notes about what to add
//  - 

/*
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

*/

}   // End UI namespace
