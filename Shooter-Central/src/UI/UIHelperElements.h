#pragma once

#include "Backend/Database.h"
#include "UI/UIData.h"

#include <imgui/imgui.h>
#include <LAS/Logging.h>

namespace ShooterCentral::UI {

void centerNextItemX        (float x);
void centerNextItemY        (float y);
void centerNextComboBoxX    (const std::string& text, float comboBoxWidth);

void centerText             (const std::string& text);
void centerTextDisabled     (const std::string& text);
bool centerButton           (const std::string& text, ImVec2 size);

std::string categoryToString    (const Category& category,  const std::string& noneText=""); // noneText - Choose what to display when none is selected
std::string subItemToString     (const SubItem& item,       const std::string& noneText=""); // noneText - Choose what to display when none is selected
                                                                                             
void resetText (char* dest, size_t size, const char* replacementText=""); // Does nothing if paremeters are invalid for any reason

namespace Tables{
    namespace Selectable{
        void ammoMetadata(
                const std::map<AmmoMetadata, StockpileAmmo>& list, 
                AmmoMetadata& selected,
                ImVec2 size 
            );
        void ammoMetadata(
                const std::map<Cartridge, std::map<AmmoMetadata, StockpileAmmo>>& list,       
                AmmoMetadata& selected,
                ImVec2 size
            );
        void ammoAmountOnHand(
                const std::map<AmmoMetadata, StockpileAmmo>& list, 
                AmmoMetadata& selected,
                ImVec2 size 
            );
        void amountOfAmmo(
                const std::map<AmmoMetadata, AmountOfAmmo>& ammoUsed,
                AmmoMetadata& selected,
                ImVec2 size
                );
        void amountOfAmmo(
                const std::vector<AmountOfAmmo> ammoUsed,
                AmmoMetadata& selected,
                const ImVec2& size
                );
        void cartridgeAmountOnHand (
                const std::map<Cartridge, int>& cartridges, 
                Cartridge& selected,
                ImVec2 size
                );
        void gunMetadata(   
                const std::set<GunMetadata>& guns, 
                GunMetadata& selected,
                ImVec2 size
            );
        void gunMetadata(
                const std::vector<GunTrackingAmmoUsed>& guns,
                GunMetadata& selectedGun,
                ImVec2 size
            );
        void gunMetadataWithRoundCount(
                const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>&, 
                GunMetadata&,
                ImVec2  
            );
        void gunMetadataWithRoundCount(
                const std::vector<GunTrackingAmmoUsed>& list, 
                GunMetadata& gun,
                ImVec2 size     
            );
        void eventMetadata( 
                const std::set<ShootingEventMetadata>& events, 
                ShootingEventMetadata& selected,
                ImVec2 size
            );
        void eventsWithNumGunsUsed( 
                const std::map<ShootingEventMetadata, ShootingEvent>& events, 
                ShootingEventMetadata& selected,
                ImVec2 size
            );
    }   // End Selectable namespace
    void amountOfAmmo(
            const std::vector<AmountOfAmmo>& ammoUsed,
            ImVec2 size
        );

}
namespace ListBoxes{
    void cartridges     (const std::set<Cartridge>&,            ImVec2 size);
    void manufacturers  (const std::set<Manufacturer>&,         ImVec2 size);
    void eventLocations (const std::set<Location>&,             ImVec2 size);
    void eventTypes     (const std::set<ShootingEventType>&,    ImVec2 size); 
    void weaponTypes    (const std::set<WeaponType>&,           ImVec2 size);
}

namespace ComboBoxes{
    void category   (Category& selected);
    void subItem    (SubItem& selected);

    void locations  (const std::set<Location>& list, Location& selected);
    void eventTypes (const std::set<ShootingEventType>& list, ShootingEventType& selected);

}


class Popup {
public:
    Popup(const char* title);
    virtual ~Popup();

    virtual void show() = 0;
    void close();

    const char* getTitle() const;

    void buttons_Close();
    bool buttons_YesOrNo(); // Returns true if YES is chosen, false if not chosen yet
                                
    virtual std::unique_ptr<Popup> clone() const = 0;

    bool wasCloseCalled() const { return closeCalled; };
private:
    char title[32] = "";
    bool closeCalled { false };
};

class SimplePopup_Close : public Popup {
public: 
   SimplePopup_Close(const char* title, const char* text);
   ~SimplePopup_Close() = default;

   void show() override;
private:
   char text[256] = "";
};


}   // End UI namespace
