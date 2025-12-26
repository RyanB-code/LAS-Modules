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

        editItemWindow(data, database, data.subItem);
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
        ScreenData::Edit& screenData,
        const Database& database,
        SubItem selectedItem
    )
{

    ScreenData::Edit::ItemBuffers& data { screenData.itemBuffers };

    static SubItem lastItem { SubItem::NONE };
    static ScreenData::Edit::ItemBuffers lastBuffers { };

    bool selectedItemEmpty { true };

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

            if(data.eventType != EMPTY_EVENT_TYPE)
                selectedItemEmpty = false;

            break;
        case SubItem::LOCATION: 
            if(data.location != lastBuffers.location)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.location.getName());

            lastBuffers.location = data.location;

            if(data.location != EMPTY_LOCATION)
                selectedItemEmpty = false;

            break;
        case SubItem::AMMO:

            break;
        case SubItem::MANUFACTURER:
            if(data.manufacturer != lastBuffers.manufacturer)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.manufacturer.getName());

            lastBuffers.manufacturer = data.manufacturer;

            if(data.manufacturer != EMPTY_MANUFACTURER)
                selectedItemEmpty = false;

            break;
        case SubItem::CARTRIDGE:
            if(data.cartridge != lastBuffers.cartridge)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.cartridge.getName());

            lastBuffers.cartridge = data.cartridge;

            if(data.cartridge != EMPTY_CARTRIDGE)
                selectedItemEmpty = false;

            break;
        case SubItem::GUN:

            break;
        case SubItem::WEAPON_TYPE:
            if(data.weaponType != lastBuffers.weaponType)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.weaponType.getName());

            lastBuffers.weaponType = data.weaponType;

            if(data.weaponType != EMPTY_WEAPON_TYPE)
                selectedItemEmpty = false;

            break;
        default:

            break;
    }
 
    if(selectedItemEmpty){
        centerNextItemY(5);
        centerTextDisabled("Select an Item");
        return;
    }
    
    bool submitted { false };
    editMetadataItem(
            data.metadataItemOld, 
            data.metadataItemBuffer, 
            MAX_CHAR_METADATA_ITEM,
            submitted,
            screenData.buttonSize
        ); 

    if(!submitted)
        return;

    // Submitted, edit the information
    switch(selectedItem){
        case SubItem::EVENT:
            
            break;
        case SubItem::EVENT_TYPE:

            break;
        case SubItem::LOCATION: 

           break;
        case SubItem::AMMO:

            break;
        case SubItem::MANUFACTURER:
            {
                DatabaseEvents::Edit::Manufacturer edit { data.manufacturer, Manufacturer { data.metadataItemBuffer } };
                pushEvent(&edit);
            }
            break;
        case SubItem::CARTRIDGE:

            break;
        case SubItem::GUN:

            break;
        case SubItem::WEAPON_TYPE:

            break;
        default:

            break;
    }
}


void editMetadataItem (
        char* oldInfo, 
        char* textBuf, 
        size_t size,
        bool& submitted,
        const ImVec2& buttonSize
    )
{
    ImVec2 childSizes { ImGui::GetContentRegionAvail().x / 2 - 5, 100 };

    // HERE
    // for this, edit all occurrences should be default since only editing new,
    // the old metadata item will still be added since its present in old saved items.
    // Unless I add inactive bool to metadata items

    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("Edit information for the selected category");
    ImGui::BulletText("This will change all occurrences for EVERY use of the item");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made");
    ImGui::Unindent();

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    // View and change info below
    ImGui::Text("Current Name");
    ImGui::SameLine(150);
    ImGui::InputText("##Current Item", oldInfo, size, ImGuiInputTextFlags_ReadOnly);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Revised Name");
    ImGui::SameLine(150);
    ImGui::InputText("##Revised Item", textBuf, size);
    ImGui::Unindent();
    
    ImGui::Dummy( ImVec2 { 0, 50} );
    submitted = centerButton("Submit", buttonSize);    
}

}   // End Edit namespace

