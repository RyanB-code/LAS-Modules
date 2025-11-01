#pragma once

#include "Container Items/AssociatedItems.h"
#include "CommonItems.h"
#include "UI/UIHelperElements.h"
#include "UI/UIData.h"
#include "UI/UIControl.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <map>
#include <memory>

#include <iostream> // For testing

namespace ShooterCentral::UI {

namespace Home {
    void main                               (const Containers& containers, ScreenData::Home& data, const UnsavedChanges& changes);

    void gunWindow                          (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, std::weak_ptr<AssociatedGun>& selected );
    void gunWindow_selectedGunInformation   (const AssociatedGun& gun);
    void eventsWindow                       (const std::map<ShootingEventMetadata, std::shared_ptr<ShootingEvent>>& events, std::weak_ptr<ShootingEvent>& selected );
    void stockpileWindow                    (const std::map<Cartridge, int>& cartridgeList, Cartridge& selected);
}

namespace View {
    void main                               (const Containers& containers, ScreenData::View& data);

    void gunTab                             (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, ScreenData::View::GunTab& data );
    void gunTab_eventsWindow                (const AssociatedGun& gun, std::weak_ptr<ShootingEvent>& selectedEvent);
    void gunTab_selectedGunInformation      (const AssociatedGun& gun);
    void gunTab_ammoUsedWindow              (const AssociatedGun& gun);

    void eventsTab                          (const std::map<ShootingEventMetadata, std::shared_ptr<ShootingEvent>>& events, ScreenData::View::EventTab& data);
    void eventsTab_selectedEventInformation (const ShootingEvent& event);
    void eventsTab_gunsUsed                 (const ShootingEvent& event, std::reference_wrapper<const GunAndAmmo>& selectedGun );

    void stockpileTab(
            const std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>>& ammoList,
            const std::map<Cartridge, int>& cartridgeList,
            ScreenData::View::StockpileTab& selected
        );

}

namespace Add {
    void    main                    (const Containers& containers, ScreenData::Add& data);
    void    showExistingItemsWindow (const Containers& containers, const SubItem& selected, ImVec2 size); 
    void    addItemWindow           (const Containers& containers, ScreenData::Add& data);

    void    add_Manufacturer        (char* textBuf, size_t size);
    void    add_Cartridge           (char* textBuf, size_t size);
    void    add_EventType           (char* textBuf, size_t size);
    void    add_WeaponType          (char* textBuf, size_t size);
    void    add_Location            (char* textBuf, size_t size);

    void    add_Event(  
                ScreenData::Add::EventBuffer& buffer, 
                size_t size,            
                const std::map<Location,            std::shared_ptr<Location>>&                         locations,
                const std::map<ShootingEventType,   std::shared_ptr<ShootingEventType>>&                eventTypes,
                const std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>>&    stockpile,
                const std::map<Cartridge, std::map<GunMetadata,   std::shared_ptr<AssociatedGun>>>&     armory
            );

    void    add_Event_gunsUsed(
                std::vector<GunAndAmmo>& gunsUsedList,
                const std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>>&    stockpile,
                const std::map<Cartridge, std::map<GunMetadata,   std::shared_ptr<AssociatedGun>>>&     armory
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
            const std::map<AmmoMetadata, std::shared_ptr<AssociatedAmmo>>& list, 
            std::weak_ptr<AssociatedAmmo>& selected,
            ImVec2 size 
            );
    void selectable_KnownAmmo(
            const std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>& list,       
            std::weak_ptr<AmmoMetadata>& selected,
            ImVec2 size
        );
    void selectable_Guns(
            const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& list, 
            std::weak_ptr<AssociatedGun>& selected,
            ImVec2 size 
        );
    void selectable_Events( 
            const std::map<ShootingEventMetadata, std::shared_ptr<ShootingEvent>>& events, 
            std::weak_ptr<ShootingEvent>& selected,
            ImVec2 size
        );
    void selectable_EventGunsUsed(
            const std::vector<GunAndAmmo>& list, 
            std::reference_wrapper<const GunAndAmmo>& gun,
            ImVec2 size     
        );
    
    void amountOfAmmo   (const std::vector<AmountOfAmmo>& ammoUsed,                         ImVec2 size);
    void amountOfAmmo   (const std::map<AmmoMetadata, AmountOfAmmo>& ammoUsed,              ImVec2 size);

    void ammoGunsUsed   (const std::map<GunMetadata, std::shared_ptr<GunMetadata>>& list,   ImVec2 size);
}

namespace ListBoxes{
    void cartridges     (const std::map<Cartridge, std::shared_ptr<Cartridge>>& list,                   ImVec2 size);
    void manufacturers  (const std::map<Manufacturer, std::shared_ptr<Manufacturer>>& list,             ImVec2 size);
    void eventLocations (const std::map<Location, std::shared_ptr<Location>>& list,                     ImVec2 size);
    void eventTypes     (const std::map<ShootingEventType, std::shared_ptr<ShootingEventType>>& list,   ImVec2 size); 
    void weaponTypes    (const std::map<WeaponType, std::shared_ptr<WeaponType>>& list,                 ImVec2 size);
}
namespace ComboBoxes{
    void  category    (Category& selected);
    void  subItem     (SubItem& selected);
}


}   // End UI namespace
