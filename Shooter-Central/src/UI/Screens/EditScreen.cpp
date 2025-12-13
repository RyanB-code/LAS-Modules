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

        selectExistingItemWindow(data.itemBuffers, database, data.subItem, data.existingItemTableSize);
    }
    ImGui::EndChild();

    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Edit Item Window", data.editItemWinSize)){ 
        ImGui::SeparatorText("Edit Item"); 
        ImGui::Spacing();
        ImGui::Spacing();

        editItemWindow(data.itemBuffers, database, data.subItem);
    }
    ImGui::EndChild();

}
void selectExistingItemWindow (
        ScreenData::Edit::ItemBuffers& buffers,
        const Database& database, 
        const SubItem& selected, 
        const ImVec2& size
    )
{
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
            Tables::Selectable::eventsWithNumGunsUsed(database.getEvents(), buffers.eventInfo, size);
            break;
        case SubItem::EVENT_TYPE:
            centerTextDisabled("All Event Types");
            ListBoxes::Selectable::eventTypes(database.getEventTypes(), buffers.eventType, size);
            break;
        case SubItem::LOCATION:
            centerTextDisabled("All Event Locations");
            ListBoxes::Selectable::eventLocations(database.getLocations(), buffers.location, size);
            break;
        case SubItem::AMMO:
            centerTextDisabled("All Ammo");
            ImGui::Spacing();
            ImGui::Spacing();
            Tables::Selectable::ammoMetadata(database.getStockpile(), buffers.ammoInfo, size); 
            break;
        case SubItem::MANUFACTURER:
            centerTextDisabled("All Manufacturers");

            if(buffers.manufacturer != EMPTY_MANUFACTURER){
                LAS::log_debug("Selected man");
                buffers.manufacturer = EMPTY_MANUFACTURER;
            }
            ListBoxes::Selectable::manufacturers(database.getManufacturers(), buffers.manufacturer, size);
            break;
        case SubItem::CARTRIDGE:
            centerTextDisabled("All Cartridges");
            ListBoxes::Selectable::cartridges(database.getCartridges(), buffers.cartridge, size);
            break;
        case SubItem::GUN:
            centerTextDisabled("All Guns");
            ImGui::Spacing();
            ImGui::Spacing();
            Tables::Selectable::gunMetadataWithRoundCount(database.getArmory(), buffers.gunInfo, size); 
            break;
        case SubItem::WEAPON_TYPE:
            centerTextDisabled("All Weapon Types");
            ListBoxes::Selectable::weaponTypes(database.getWeaponTypes(), buffers.weaponType, size);
            break;
        default:

            break;
    }
    ImGui::EndGroup();
}
void editItemWindow(
        ScreenData::Edit::ItemBuffers& data,
        const Database& database,
        SubItem selectedItem
    )
{
    static SubItem lastItem { SubItem::NONE };
    static ScreenData::Edit::ItemBuffers lastBuffers { };

    if(selectedItem != lastItem)
        resetText(data.metadataItemBuffer, MAX_CHAR_METADATA_ITEM);
    
    lastItem = selectedItem;

    switch(selectedItem){
        case SubItem::EVENT:
            
            break;
        case SubItem::EVENT_TYPE:
            if(data.eventType != lastBuffers.eventType)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.eventType.getName());

            lastBuffers.eventType = data.eventType;

            if(data.eventType == EMPTY_EVENT_TYPE){
                centerNextItemY(5);
                centerTextDisabled("Select an Item");
            }
            else
                editEventType(data.metadataItemOld, data.metadataItemBuffer, MAX_CHAR_METADATA_ITEM);

            break;
        case SubItem::LOCATION: 
            //editLocation(data.eventType, MAX_CHAR_METADATA_ITEM);
            break;
        case SubItem::AMMO:

            break;
        case SubItem::MANUFACTURER:
            //Add::add_Manufacturer(data.subItemBuffers.manufacturer, MAX_CHAR_METADATA_ITEM);
            break;
        case SubItem::CARTRIDGE:
            //Add::add_Cartridge(data.subItemBuffers.cartridge, MAX_CHAR_METADATA_ITEM);
            break;
        case SubItem::GUN:

            break;
        case SubItem::WEAPON_TYPE:
            //Add::add_WeaponType(data.subItemBuffers.weaponType, MAX_CHAR_METADATA_ITEM);
            break;
        default:
            centerNextItemY(5);
            centerTextDisabled("Select an Item");
            break;
    }
 
}


void editEventType (char* oldInfo, char* textBuf, size_t size){
    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("Edit information for the selected Event Type");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Text("Current Info");
    ImGui::SameLine(150);
    ImGui::InputText("##Current EventType", oldInfo, size, ImGuiInputTextFlags_ReadOnly);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Event Type");
    ImGui::SameLine(150);
    ImGui::InputText("##Edit EventType", textBuf, size);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Button", ImVec2{200,50})){
        LAS::log_debug("Submitted");
    }

}

}   // End Edit namespace

