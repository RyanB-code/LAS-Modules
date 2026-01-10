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
            Tables::Selectable::eventsWithNumGunsUsed(database.getEvents(), buffers.selectedEventInfo, size);
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
            Tables::Selectable::ammoMetadata(database.getStockpile(), buffers.selectedAmmoInfo, size); 
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
            Tables::Selectable::gunMetadataWithRoundCount(database.getArmory(), buffers.selectedGunInfo, size); 
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
    bool submitted { false };

    if(selectedItem != lastItem) {
        lastBuffers = ScreenData::Edit::ItemBuffers { };
        data = ScreenData::Edit::ItemBuffers { };
    }
    
    lastItem = selectedItem;

    switch(selectedItem){
        case SubItem::EVENT:
            if(data.selectedEventInfo != lastBuffers.selectedEventInfo){
                data.eventBuffer.buffer = database.getEvents().at(data.selectedEventInfo);
                data.eventBuffer.gunsAndAmmoWindow = ScreenData::Edit::ItemBuffers::Event::GunsAndAmmoWindow { };

                resetText(data.eventBuffer.metadataWindow.notes, ShootingEventMetadata::MAX_CHAR_NOTES, data.selectedEventInfo.notes.c_str());
                data.eventBuffer.metadataWindow.location = data.selectedEventInfo.location;
                data.eventBuffer.metadataWindow.eventType = data.selectedEventInfo.eventType;
                data.eventBuffer.metadataWindow.day = static_cast<int>(static_cast<unsigned int>(data.selectedEventInfo.date.day()));
                data.eventBuffer.metadataWindow.month = static_cast<int>(static_cast<unsigned int>(data.selectedEventInfo.date.month()));
                data.eventBuffer.metadataWindow.year = static_cast<int>(data.selectedEventInfo.date.year());
            }

            lastBuffers.selectedEventInfo = data.selectedEventInfo;
            
            if(data.selectedEventInfo != EMPTY_EVENT_METADATA){
                selectedItemEmpty = false;

                editEvent(
                        data,
                        ShootingEventMetadata::MAX_CHAR_NOTES,
                        database.getLocations(),
                        database.getEventTypes(),
                        database.getStockpile(),
                        database.getArmory(),
                        submitted,
                        screenData.buttonSize
                    );
            }

            break;
        case SubItem::EVENT_TYPE:
            if(data.eventType != lastBuffers.eventType)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.eventType.getName());

            lastBuffers.eventType = data.eventType;

            if(data.eventType != EMPTY_EVENT_TYPE){
                selectedItemEmpty = false;

                editMetadataItem(
                        data.metadataItemOld, 
                        data.metadataItemBuffer, 
                        MAX_CHAR_METADATA_ITEM,
                        submitted,
                        screenData.buttonSize
                    ); 
            }

            break;
        case SubItem::LOCATION: 
            if(data.location != lastBuffers.location)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.location.getName());

            lastBuffers.location = data.location;

            if(data.location != EMPTY_LOCATION){
                selectedItemEmpty = false;

                editMetadataItem(
                        data.metadataItemOld, 
                        data.metadataItemBuffer, 
                        MAX_CHAR_METADATA_ITEM,
                        submitted,
                        screenData.buttonSize
                    ); 
            }

            break;
        case SubItem::AMMO:
            if(data.selectedAmmoInfo != lastBuffers.selectedAmmoInfo){
                resetText(data.nameBuffer, MAX_CHAR_METADATA_ITEM, data.selectedAmmoInfo.name.c_str());
                data.ammoInfoBuffer.manufacturer = data.selectedAmmoInfo.manufacturer;
                data.ammoInfoBuffer.grainWeight = data.selectedAmmoInfo.grainWeight;
            }

            lastBuffers.selectedAmmoInfo = data.selectedAmmoInfo;

            if(data.selectedAmmoInfo != EMPTY_AMMO_METADATA){
                selectedItemEmpty = false;
                editAmmo(
                        data.nameBuffer,
                        MAX_CHAR_METADATA_ITEM,
                        data.selectedAmmoInfo, 
                        data.ammoInfoBuffer, 
                        database.getManufacturers(),
                        submitted, 
                        screenData.buttonSize
                    );
            }
            break;
        case SubItem::MANUFACTURER:
            if(data.manufacturer != lastBuffers.manufacturer)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.manufacturer.getName());

            lastBuffers.manufacturer = data.manufacturer;

            if(data.manufacturer != EMPTY_MANUFACTURER){
                selectedItemEmpty = false;

                editMetadataItem(
                        data.metadataItemOld, 
                        data.metadataItemBuffer, 
                        MAX_CHAR_METADATA_ITEM,
                        submitted,
                        screenData.buttonSize
                    ); 
            }
            break;
        case SubItem::CARTRIDGE:
            if(data.cartridge != lastBuffers.cartridge)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.cartridge.getName());

            lastBuffers.cartridge = data.cartridge;

            if(data.cartridge != EMPTY_CARTRIDGE){
                selectedItemEmpty = false;

                editMetadataItem(
                        data.metadataItemOld, 
                        data.metadataItemBuffer, 
                        MAX_CHAR_METADATA_ITEM,
                        submitted,
                        screenData.buttonSize
                    ); 
            }

            break;
        case SubItem::GUN:
            if(data.selectedGunInfo != lastBuffers.selectedGunInfo){
                resetText(data.nameBuffer, MAX_CHAR_METADATA_ITEM, data.selectedGunInfo.name.c_str());
                data.gunInfoBuffer.weaponType = data.selectedGunInfo.weaponType;
            }

            lastBuffers.selectedGunInfo = data.selectedGunInfo;

            if(data.selectedGunInfo != EMPTY_GUN_METADATA){
                selectedItemEmpty = false;
                editGun(
                        data.nameBuffer,
                        MAX_CHAR_METADATA_ITEM,
                        data.selectedGunInfo, 
                        data.gunInfoBuffer, 
                        database.getWeaponTypes(),
                        submitted, 
                        screenData.buttonSize
                    );
            }
            break;
        case SubItem::WEAPON_TYPE:
            if(data.weaponType != lastBuffers.weaponType)
                resetText(data.metadataItemOld, MAX_CHAR_METADATA_ITEM, data.weaponType.getName());

            lastBuffers.weaponType = data.weaponType;

            if(data.weaponType != EMPTY_WEAPON_TYPE){
                selectedItemEmpty = false;

                editMetadataItem(
                        data.metadataItemOld, 
                        data.metadataItemBuffer, 
                        MAX_CHAR_METADATA_ITEM,
                        submitted,
                        screenData.buttonSize
                    ); 
            }

            break;
        default:

            break;
    }
 
    if(selectedItemEmpty){
        centerNextItemY(5);
        centerTextDisabled("Select an Item");
        return;
    }
    
    if(!submitted)
        return;

    // Submitted, edit the information
    switch(selectedItem){
        case SubItem::EVENT:
            {
                ShootingEventMetadata infoBuffer { 
                    std::string{data.eventBuffer.metadataWindow.notes},
                    data.eventBuffer.metadataWindow.location,
                    data.eventBuffer.metadataWindow.eventType,
                    ymd  { 
                            std::chrono::year    { data.eventBuffer.metadataWindow.year}, 
                            std::chrono::month   { static_cast<unsigned int>(data.eventBuffer.metadataWindow.month)}, 
                            std::chrono::day     { static_cast<unsigned int>(data.eventBuffer.metadataWindow.day)} 
                        }
                };

                data.eventBuffer.buffer.setInfo ( infoBuffer );

                DatabaseEvents::Edit::Event edit { data.selectedEventInfo, data.eventBuffer.buffer  };
                pushEvent(&edit);
            }
            break;
        case SubItem::EVENT_TYPE:
            {
                DatabaseEvents::Edit::EventType edit { data.eventType, ShootingEventType { data.metadataItemBuffer } };
                pushEvent(&edit);
            }
            break;
        case SubItem::LOCATION: 
            {
                DatabaseEvents::Edit::Location edit { data.location, Location { data.metadataItemBuffer } };
                pushEvent(&edit);
            }
           break;
        case SubItem::AMMO:
            data.ammoInfoBuffer.name        = data.nameBuffer;
            data.ammoInfoBuffer.cartridge   = data.selectedAmmoInfo.cartridge;
            {
                DatabaseEvents::Edit::AmmoMetadata edit { data.selectedAmmoInfo, data.ammoInfoBuffer };
                pushEvent(&edit);
            }

            break;
        case SubItem::MANUFACTURER:
            {
                DatabaseEvents::Edit::Manufacturer edit { data.manufacturer, Manufacturer { data.metadataItemBuffer } };
                pushEvent(&edit);
            }
            break;
        case SubItem::CARTRIDGE:
            {
                DatabaseEvents::Edit::Cartridge edit { data.cartridge, Cartridge { data.metadataItemBuffer } };
                pushEvent(&edit);
            }
            break;
        case SubItem::GUN:
            data.gunInfoBuffer.name         = data.nameBuffer;
            data.gunInfoBuffer.cartridge    = data.selectedGunInfo.cartridge;
            {
                DatabaseEvents::Edit::GunMetadata edit { data.selectedGunInfo, data.gunInfoBuffer };
                pushEvent(&edit);
            }

            break;
        case SubItem::WEAPON_TYPE:
            {
                DatabaseEvents::Edit::WeaponType edit { data.weaponType, WeaponType { data.metadataItemBuffer } };
                pushEvent(&edit);
            }
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
void editGun(
        char* nameBuffer,
        size_t size,
        const GunMetadata& oldInfo,
        GunMetadata& newInfo,
        const std::set<WeaponType>& weaponTypes,
        bool& submitted,
        const ImVec2& buttonSize
    )
{
    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("Edit information a Gun");
    ImGui::BulletText("You CANNOT edit the Cartridge for the Gun only. You must edit entire Cartridges themselves");
    ImGui::BulletText("This will change all occurrences for EVERY use of the item");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made");
    ImGui::Unindent();

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    // View and change info below
    ImGui::Indent(20);
    ImGui::BeginGroup();
    ImGui::TextDisabled("Name:            ");
    ImGui::SameLine();
    ImGui::Text("%s", oldInfo.name.c_str());

    ImGui::SameLine(250);
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("##New Name", nameBuffer, size);

    ImGui::TextDisabled("Weapon Type:     ");
    ImGui::SameLine();
    ImGui::Text("%s", oldInfo.weaponType.getName()); 

    ImGui::SameLine(250);
    ImGui::SetNextItemWidth(200);
    ComboBoxes::weaponTypes(weaponTypes, newInfo.weaponType);

    ImGui::TextDisabled("Cartridge:       ");
    ImGui::SameLine();
    ImGui::Text("%s", oldInfo.cartridge.getName());

    ImGui::SameLine(250);
    ImGui::TextDisabled("(Cannot Change Cartridge)");
    ImGui::EndGroup();
        
    ImGui::Dummy( ImVec2 { 0, 50} );
    submitted = centerButton("Submit", buttonSize);    

}
void editAmmo(
        char* nameBuffer,
        size_t size,
        const AmmoMetadata& oldInfo,
        AmmoMetadata& newInfo,
        const std::set<Manufacturer>& manufacturers,
        bool& submitted,
        const ImVec2& buttonSize
    )
{
    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("Edit information an Ammo Type");
    ImGui::BulletText("You CANNOT edit the Cartridge for the Ammo only. You must edit entire Cartridges themselves");
    ImGui::BulletText("This will change all occurrences for EVERY use of the item");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made");
    ImGui::Unindent();

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    // View and change info below
    ImGui::Indent(20);
    ImGui::BeginGroup();

    ImGui::TextDisabled("Name:            ");
    ImGui::SameLine();
    ImGui::Text("%s", oldInfo.name.c_str());
    ImGui::SameLine(250);
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("##New Name", nameBuffer, size);

    ImGui::TextDisabled("Manufacturer:    ");
    ImGui::SameLine();
    ImGui::Text("%s", oldInfo.manufacturer.getName()); 
    ImGui::SameLine(250);
    ImGui::SetNextItemWidth(200);
    ComboBoxes::manufacturers(manufacturers, newInfo.manufacturer);

    ImGui::TextDisabled("Grain Weight:    "); 
    ImGui::SameLine();
    ImGui::Text("%d", oldInfo.grainWeight); 
    ImGui::SameLine(250);
    ImGui::SetNextItemWidth(200);
    ImGui::InputInt("##Grain Weight", &newInfo.grainWeight, 1, 5);

    ImGui::TextDisabled("Cartridge:       ");
    ImGui::SameLine();
    ImGui::Text("%s", oldInfo.cartridge.getName());
    ImGui::SameLine(250);
    ImGui::TextDisabled("(Cannot Change Cartridge)");

    ImGui::EndGroup();
        
    ImGui::Dummy( ImVec2 { 0, 50} );
    submitted = centerButton("Submit", buttonSize);    

}
void editEvent(
        ScreenData::Edit::ItemBuffers& data, 
        size_t notesSize,            
        const std::set<Location>& locations,
        const std::set<ShootingEventType>& eventTypes,
        const std::map<Cartridge, std::map<AmmoMetadata, StockpileAmmo>>& stockpile,
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
        bool& submitted,
        const ImVec2& buttonSize
    )
{ 
    ImGui::Spacing();
    ImGui::Spacing();

    submitted = centerButton("Submit Changes", buttonSize);

    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginTabBar("Edit Event Tabs")){
        if(ImGui::BeginTabItem("Event Information")){
            editEvent_metadataWindow(
                    data.eventBuffer.metadataWindow, 
                    data.selectedEventInfo, 
                    notesSize, 
                    locations, 
                    eventTypes
                ); 
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Guns and Ammo")){
            editEvent_gunsAndAmmoWindow(
                    data.eventBuffer, 
                    data.eventBuffer.buffer, 
                    stockpile, 
                    armory
                ); 
            ImGui::EndTabItem();
        } 
        ImGui::EndTabBar();
    }
}
void editEvent_metadataWindow(
        ScreenData::Edit::ItemBuffers::Event::MetadataWindow& data,
        const ShootingEventMetadata& oldInfo,
        size_t notesSize,
        const std::set<Location>& locations,
        const std::set<ShootingEventType>& eventTypes
    ) 
{
    using namespace std::chrono;

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Indent(20);
    ImGui::Text("Location:");
    ImGui::SameLine(150);
    ImGui::TextDisabled("%s", oldInfo.location.getName());
    ImGui::SameLine(400);
    ImGui::SetNextItemWidth(200);
    ComboBoxes::locations(locations, data.location);

    ImGui::Text("Event Type:");
    ImGui::SameLine(150);
    ImGui::TextDisabled("%s", oldInfo.eventType.getName());
    ImGui::SameLine(400);
    ImGui::SetNextItemWidth(200);
    ComboBoxes::eventTypes(eventTypes, data.eventType);

    ImGui::Text("Date:");
    ImGui::SameLine(150);
    ImGui::TextDisabled("%s", printDate(oldInfo.date).c_str());
    ImGui::SameLine(400);
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Input Day", &data.day);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Input Month", &data.month);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Input Year", &data.year);

    // Button to auto fill today's date
    ImGui::SameLine(0, 30);
    if(ImGui::Button("Today", ImVec2(75, 20))){
        const std::chrono::zoned_time now {std::chrono::current_zone(), std::chrono::system_clock::now( ) };

        const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now.get_local_time())};
        data.day = static_cast<unsigned>(ymd.day());
        data.month = static_cast<unsigned>(ymd.month());
        data.year = static_cast<int>(ymd.year());
    }
    // Grayed out details, approximate centers of text box did the trick
    ImGui::SetCursorPosX(400);
    ImGui::TextDisabled("(Day)");
    ImGui::SameLine(0, 70);
    ImGui::TextDisabled("(Month)");
    ImGui::SameLine(0, 70);
    ImGui::TextDisabled("(Year)");

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Notes:");
    ImGui::SameLine(400);
    ImGui::TextDisabled("(May be left blank)");

    if(ImGui::BeginChild("Old Notes", ImVec2{350, 100}, ImGuiChildFlags_Border)){
        ImGui::TextWrapped("%s", oldInfo.notes.c_str());
    }
    ImGui::EndChild();
 
    ImGui::SameLine(400);
    ImGui::InputTextMultiline(
            "##Revised Notes", 
            data.notes, 
            notesSize, 
            ImVec2{600, 100}, 
            ImGuiInputTextFlags_CtrlEnterForNewLine
        );

    ImGui::Unindent();
}
void editEvent_gunsAndAmmoWindow(
        ScreenData::Edit::ItemBuffers::Event& eventWindow, 
        ShootingEvent& event,
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&    stockpile,
        const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>&     armory
    )
{
    ScreenData::Edit::ItemBuffers::Event::GunsAndAmmoWindow& data { eventWindow.gunsAndAmmoWindow };

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent(20);
    ImGui::Text("NOTE: Editing ammo amounts will NOT affect amount-on-hand in Stockpile, but will apply to Gun history");
    ImGui::Unindent();
 
    ImGui::Dummy( ImVec2 {0, 50} );

    static ImVec2 regionAvail { ImGui::GetContentRegionAvail() };

    // Do this after directions since that will be unchanging
    regionAvail = ImGui::GetContentRegionAvail();
    data.mainWindowSize = ImVec2 { (regionAvail.x / 4 ) * 2 - 30, regionAvail.y }; // Minus 30 for offset stuff
    data.viewWindowSize = ImVec2 { (regionAvail.x / 4 ), regionAvail.y };

    if(data.viewWindowSize.x < data.minWinSize.x){
        data.verticalLayout = true;
        data.mainWindowSize = regionAvail;
        data.viewWindowSize = data.mainWindowSize;
    }
    else
        data.verticalLayout = false;

    // Ensure minimum windows
    if(data.mainWindowSize.x < data.minWinSize.x)
        data.mainWindowSize.x = data.minWinSize.x;
    if(data.mainWindowSize.y < data.minWinSize.y)
        data.mainWindowSize.y = data.minWinSize.y;

    if(data.viewWindowSize.x < data.minWinSize.x)
        data.viewWindowSize.x = data.minWinSize.x;
    if(data.viewWindowSize.y < data.minWinSize.y)
        data.viewWindowSize.y = data.minWinSize.y;



    bool removeGunButton { false };
    editEvent_viewGunsUsedWindow(
            event,
            data.selectedGun,
            data.selectedGunValid,
            removeGunButton,
            data.viewTableSize,
            data.minTableWidth,
            data.maxTableWidth,
            data.viewWindowSize,
            data.buttonSize
        );

    if(removeGunButton){
        event.removeGun(data.selectedGun);
        data.selectedGun    = EMPTY_GUN_METADATA;
        data.selectedAmmo   = EMPTY_AMMO_METADATA; 
        data.selectedGunValid   = false;
        data.selectedAmmoValid  = false;
    }
    
    if(!data.verticalLayout)
        ImGui::SameLine();

    bool removeAmmo { false };
    editEvent_viewAmmoUsedWindow(
            event,
            data.selectedGun,
            data.selectedAmmo,
            data.selectedAmmoValid,
            removeAmmo,
            data.selectedGunValid,
            data.viewTableSize,
            data.viewWindowSize,
            data.buttonSize
        );
    if(removeAmmo){
        event.getGun(data.selectedGun).removeAmmoUsed(data.selectedAmmo);

        data.selectedAmmo = EMPTY_AMMO_METADATA;
        data.selectedAmmoValid = false;
    }


    
    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Add Area", data.mainWindowSize)){
        data.mainTableSize.x = ImGui::GetContentRegionAvail().x-2;
        if(data.mainTableSize.x < data.minTableWidth)
            data.mainTableSize.x = data.minTableWidth;
        if(data.mainTableSize.x > data.maxTableWidth)
            data.mainTableSize.x = data.maxTableWidth;

        if(ImGui::BeginTabBar("Add Area Tabs")){
            if(ImGui::BeginTabItem("Add Gun")){
                editEvent_addGun(data.addGunWindow, event, armory, data.mainTableSize);
                ImGui::EndTabItem();
            }

            if(!data.selectedGunValid)
                ImGui::BeginDisabled();

            if(ImGui::BeginTabItem("Add Ammo")){
                if(!event.hasUsedGun(data.selectedGun)){
                    centerNextItemY(5);
                    centerTextDisabled("Select A Gun To Add Ammo");
                }
                else 
                    editEvent_addAmmoToGun(data.addAmmoWindow, event.getGun(data.selectedGun), stockpile, data.mainTableSize);

                ImGui::EndTabItem();
            }

            if(!data.selectedGunValid)
                ImGui::EndDisabled();

            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
}
void editEvent_viewGunsUsedWindow(
        const ShootingEvent& event,
        GunMetadata&    selectedGun,
        bool&           isGunValid,
        bool&           removeGunPressed,
        ImVec2&         tableSize,
        const float     minTableWidth,
        const float     maxTableWidth,
        const ImVec2&   windowSize,
        const ImVec2&   buttonSize
    )
{
    if(ImGui::BeginChild("Guns Used", windowSize)){
        ImGui::SeparatorText("Guns Used");
        ImGui::Spacing();
        ImGui::Spacing();

        if(!isGunValid)
            ImGui::BeginDisabled();

        if(centerButton("Remove Gun", buttonSize))
           removeGunPressed = true; 

        if(!isGunValid)
            ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Spacing();

        tableSize.x = ImGui::GetContentRegionAvail().x-2;
        if(tableSize.x < minTableWidth)
            tableSize.x = minTableWidth;
        if(tableSize.x > maxTableWidth)
            tableSize.x = maxTableWidth;

        centerNextItemX(tableSize.x);
        Tables::Selectable::gunMetadataWithRoundCount(event.getGunsUsed(), selectedGun, tableSize);
        isGunValid = selectedGun != EMPTY_GUN_METADATA;
    }
    ImGui::EndChild();

}
void editEvent_viewAmmoUsedWindow(
        const ShootingEvent&    event,
        const GunMetadata&      selectedGun,
        AmmoMetadata&           selectedAmmo,
        bool&                   isAmmoValid,
        bool&                   removeAmmo,
        const bool              isGunValid,
        const ImVec2&           tableSize,
        const ImVec2&           windowSize,
        const ImVec2&           buttonSize

    )
{
    if(ImGui::BeginChild("Ammo Used", windowSize)){
        ImGui::SeparatorText("Ammo Used"); 
        ImGui::Spacing();
        ImGui::Spacing();

        if(!isGunValid){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Ammo Used");
        }
        else{
            if(!isAmmoValid)
                ImGui::BeginDisabled();

            removeAmmo = centerButton("Remove Ammo", buttonSize);

            if(!isAmmoValid)
                ImGui::EndDisabled();

            ImGui::Spacing();
            ImGui::Spacing();

            centerNextItemX(tableSize.x);
            Tables::Selectable::amountOfAmmo(
                    event.getGun(selectedGun).getAmmoUsed(), 
                    selectedAmmo,
                    tableSize
                );
            isAmmoValid = selectedAmmo != EMPTY_AMMO_METADATA;
        }
    }
    ImGui::EndChild();

}
void editEvent_addGun (
        ScreenData::Edit::ItemBuffers::Event::GunsAndAmmoWindow::AddGunWindow& data, 
        ShootingEvent& event, 
        const std::map<Cartridge, std::map<GunMetadata,  ArmoryGun>>& armory,
        const ImVec2& tableSize
    )
{
    ImGui::Spacing();
    ImGui::Spacing();

    data.selectedGunValid = data.selectedGun != EMPTY_GUN_METADATA;

    if(!data.selectedGunValid)
        ImGui::BeginDisabled();

    if(centerButton("Add Gun", data.buttonSize)){
        if(event.hasUsedGun(data.selectedGun)){
            SimpleClosePopup popup { 
                "Gun Already Added",
                "This Gun is already added to the Event"
            };
            UIEvents::PushPopup event { &popup };
            pushEvent(&event);
        }
        else{
            if(event.addGun(GunTrackingAmmoUsed{data.selectedGun}))
                data.selectedGun = EMPTY_GUN_METADATA;
            else{
                SimpleClosePopup popup { 
                    "Failed to Add Gun",
                    "This Gun could not be added to the Event"
                };
                UIEvents::PushPopup event { &popup };
                pushEvent(&event);
            }
        }
    }

    if(!data.selectedGunValid)
        ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Spacing();

    centerText("All Guns");

    ImGui::Spacing();
    ImGui::Spacing();
    
    centerNextItemX(tableSize.x);
    Tables::Selectable::gunMetadataWithRoundCount(armory, data.selectedGun, tableSize);
}
void editEvent_addAmmoToGun (
        ScreenData::Edit::ItemBuffers::Event::GunsAndAmmoWindow::AddAmmoWindow& data, 
        GunTrackingAmmoUsed& selected,
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& stockpile,
        const ImVec2& tableSize
    )
{
    const Cartridge& gunCartridge { selected.getGunInfo().cartridge };

    if(!stockpile.contains(gunCartridge)){
        centerNextItemY(5);
        centerTextDisabled(std::format("Stockpile contains no Ammo for cartridge '{}'", gunCartridge.getName())); 
        return;
    }
    
    ImGui::Spacing();
    ImGui::Spacing();

    data.isAmountOfAmmoValid = (data.selectedAmmo != EMPTY_AMMO_METADATA && data.amountBuffer > 0);

    if(!data.isAmountOfAmmoValid)
        ImGui::BeginDisabled();

    if(centerButton("Add Ammo", data.buttonSize)){
        selected.addAmmoUsed( AmountOfAmmo { data.selectedAmmo, data.amountBuffer });
        data.selectedAmmo = EMPTY_AMMO_METADATA;
        data.amountBuffer = 0;
    }

    if(!data.isAmountOfAmmoValid)
        ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Spacing();

    // amount of ammo here
    centerNextItemX(300);
    ImGui::BeginGroup();
    ImGui::Text("Amount"); 
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    ImGui::InputInt("##Amount Of Ammo", &data.amountBuffer, 1, 50);
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Spacing();

    centerText(std::format("All {} ammo", gunCartridge.getName()));

    ImGui::Spacing();
    ImGui::Spacing();
    
    centerNextItemX(tableSize.x);
    Tables::Selectable::ammoAmountOnHand(stockpile.at(gunCartridge), data.selectedAmmo, tableSize);
}

}   // End Edit namespace

