#pragma once

#include "Container Items/AssociatedItems.h"
#include "CommonItems.h"
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
    void    addItemWindow           (const SubItem& selected, ScreenData::Add::TextBuffers& textBuffers);

    void    add_WeaponType          (char* textBuf, size_t size);
}


// Helper functions
void centerNextItemX        (float x);
void centerNextItemY        (float y);
void centerNextComboBoxX    (const std::string& text, float comboBoxWidth);

void centerText             (const std::string& text);
void centerTextDisabled     (const std::string& text);
bool centerButton           (const std::string& text, ImVec2 size);

std::string categoryToString    (const Category& category,  const std::string& noneText=""); // noneText - Choose what to display when none is selected
std::string subItemToString     (const SubItem& item,       const std::string& noneText=""); // noneText - Choose what to display when none is selected
                                                                                             
void resetText (char* dest, size_t size, const char* replacementText=""); // Does nothing if paremeters are invalid for any reason


class Popup {
public:
    Popup(const char* title);
    virtual ~Popup();

    virtual void show() = 0;
    void close() const;

    const char* getTitle() const;

    void buttons_Close()   const;
    bool buttons_YesOrNo() const; // Returns true if YES is chosen, false if not chosen yet

private:
    char title[32] = "";
};
                                                                                        

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
