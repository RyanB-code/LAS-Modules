#include "EditScreen.h"

namespace ShooterCentral::UI::Edit {

void main(const Database& database, ScreenData::Edit& data){
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    centerNextComboBoxX("   Select An Item", 200);
    ComboBoxes::subItem(data.subItem);

    ImGui::Spacing();
    ImGui::Spacing();

    const ImVec2 regionAvail { ImGui::GetContentRegionAvail() };

    // Sizes below the combo box to get proper Y coord after the box
    data.editItemWinSize        = ImVec2{ regionAvail.x / 4 * 3, regionAvail.y };
    data.existingItemWinSize    = ImVec2{ regionAvail.x / 4 - 10, regionAvail.y };

    if(data.existingItemWinSize.x < data.minWinSize.x ){
        data.verticalLayout = true;
        data.editItemWinSize = regionAvail;
        data.existingItemWinSize = data.editItemWinSize;
    }
    else
        data.verticalLayout = false;

    // Ensure minimum windows
    if(data.editItemWinSize.x < data.minWinSize.x)
        data.editItemWinSize.x = data.minWinSize.x;
    if(data.editItemWinSize.y < data.minWinSize.y)
        data.editItemWinSize.y = data.minWinSize.y;

    if(data.existingItemWinSize.x < data.minWinSize.x)
        data.existingItemWinSize.x = data.minWinSize.x;
    if(data.existingItemWinSize.y < data.minWinSize.y)
        data.existingItemWinSize.y = data.minWinSize.y;



    if(ImGui::BeginChild("Existing Items View", data.existingItemWinSize)){  
        data.existingItemTableSize.x = ImGui::GetContentRegionAvail().x-2;
        if(data.existingItemTableSize.x < data.minTableWidth)
            data.existingItemTableSize.x = data.minTableWidth;
        if(data.existingItemTableSize.x > data.maxTableWidth)
            data.existingItemTableSize.x = data.maxTableWidth;

        selectExistingItemWindow(database, data.subItem, data.existingItemTableSize);
    }
    ImGui::EndChild();

    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Edit Item Window", data.editItemWinSize)){ 
        ImGui::SeparatorText("Edit Item"); 
        ImGui::Spacing();
        ImGui::Spacing();

        //addItemWindow(database, data);
    }
    ImGui::EndChild();

}
void selectExistingItemWindow (const Database& database, const SubItem& selected, const ImVec2& size){
    static ShootingEventMetadata   selectedEvent    { };
    static GunMetadata             selectedGun      { };
    static AmmoMetadata            selectedAmmo     { };

    static Manufacturer selectedMan { };

    ImGui::SeparatorText("Select Item to Edit"); 
    ImGui::Spacing();
    ImGui::Spacing();

    centerNextItemX(size.x);
    ImGui::BeginGroup();
    switch(selected){
        case SubItem::EVENT:
            centerTextDisabled("All Events");
            ImGui::Spacing();
            ImGui::Spacing();

            Tables::Selectable::eventsWithNumGunsUsed(database.getEvents(), selectedEvent, size);

            if(selectedEvent != EMPTY_EVENT_METADATA){
                ScreenData::View viewWindow { };
                viewWindow.eventsWindow.selectedEvent = selectedEvent;
                viewWindow.category = Category::EVENTS;

                UIEvents::SetScreenData::View setData { viewWindow };
                UIEvents::SetScreen setScreen { Screen::VIEW };

                pushEvent(&setData);
                pushEvent(&setScreen);

                selectedEvent = EMPTY_EVENT_METADATA;
            }
            break;
        case SubItem::EVENT_TYPE:
            centerTextDisabled("All Event Types");
            ListBoxes::eventTypes(database.getEventTypes(), size);
            break;
        case SubItem::LOCATION:
            centerTextDisabled("All Event Locations");
            ListBoxes::eventLocations(database.getLocations(), size);
            break;
        case SubItem::AMMO:
            centerTextDisabled("All Ammo");
            ImGui::Spacing();
            ImGui::Spacing();

            Tables::Selectable::ammoMetadata(database.getStockpile(), selectedAmmo, size);

            if(selectedAmmo != EMPTY_AMMO_METADATA){
                ScreenData::View viewWindow { };
                viewWindow.stockpileWindow.selectedAmmo = selectedAmmo;
                viewWindow.stockpileWindow.selectedCartridge = selectedAmmo.cartridge;
                viewWindow.category = Category::STOCKPILE;

                UIEvents::SetScreenData::View setData { viewWindow };
                UIEvents::SetScreen setScreen { Screen::VIEW };

                pushEvent(&setData);
                pushEvent(&setScreen);
                selectedAmmo = EMPTY_AMMO_METADATA;
            }
            break;
        case SubItem::MANUFACTURER:
            centerTextDisabled("All Manufacturers");

            if(selectedMan != EMPTY_MANUFACTURER){
                LAS::log_debug("Selected man");
                selectedMan = EMPTY_MANUFACTURER;
            }

            ListBoxes::Selectable::manufacturers(database.getManufacturers(), selectedMan, size);

            break;
        case SubItem::CARTRIDGE:
            centerTextDisabled("All Cartridges");
            ListBoxes::cartridges(database.getCartridges(), size);
            break;
        case SubItem::GUN:
            centerTextDisabled("All Guns");
            ImGui::Spacing();
            ImGui::Spacing();

            Tables::Selectable::gunMetadataWithRoundCount(database.getArmory(), selectedGun, size);

            if(selectedGun != EMPTY_GUN_METADATA){
                ScreenData::View viewWindow { };
                viewWindow.armoryWindow.selectedGun = selectedGun;
                viewWindow.category = Category::GUNS;

                UIEvents::SetScreenData::View setData { viewWindow };
                UIEvents::SetScreen setScreen { Screen::VIEW };

                pushEvent(&setData);
                pushEvent(&setScreen);
                selectedGun = EMPTY_GUN_METADATA;
            }
            break;
        case SubItem::WEAPON_TYPE:
            centerTextDisabled("All Weapon Types");
            ListBoxes::weaponTypes(database.getWeaponTypes(), size);
            break;
        default:

            break;
    }
    ImGui::EndGroup();
}

}   // End Edit namespace

