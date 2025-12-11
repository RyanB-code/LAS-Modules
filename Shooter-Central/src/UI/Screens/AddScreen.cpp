#include "AddScreen.h"

namespace ShooterCentral::UI::Add {

void main(const Database& database, ScreenData::Add& data){

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    centerNextComboBoxX("   Select An Item", 200);
    ComboBoxes::subItem(data.subItem);

    ImGui::Spacing();
    ImGui::Spacing();

    const ImVec2 regionAvail { ImGui::GetContentRegionAvail() };

    // Sizes below the combo box to get proper Y coord after the box
    data.mainWindowSize = ImVec2{ regionAvail.x / 4 * 3, regionAvail.y };
    data.infoWindowSize = ImVec2{ regionAvail.x / 4 - 10, regionAvail.y };

    if(data.infoWindowSize.x < data.minWinSize.x ){
        data.verticalLayout = true;
        data.mainWindowSize = regionAvail;
        data.infoWindowSize = data.mainWindowSize;
    }
    else
        data.verticalLayout = false;

    // Ensure minimum windows
    if(data.mainWindowSize.x < data.minWinSize.x)
        data.mainWindowSize.x = data.minWinSize.x;
    if(data.mainWindowSize.y < data.minWinSize.y)
        data.mainWindowSize.y = data.minWinSize.y;

    if(data.infoWindowSize.x < data.minWinSize.x)
        data.infoWindowSize.x = data.minWinSize.x;
    if(data.infoWindowSize.y < data.minWinSize.y)
        data.infoWindowSize.y = data.minWinSize.y;


    if(ImGui::BeginChild("Add Item Window", data.mainWindowSize)){ 
        ImGui::SeparatorText("Add Item"); 
        ImGui::Spacing();
        ImGui::Spacing();

        addItemWindow(database, data);
    }
    ImGui::EndChild();

    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Existing Items View", data.infoWindowSize)){ 
        ImGui::SeparatorText("View Existing Items"); 
        ImGui::Spacing();
        ImGui::Spacing();

        data.infoTableSize.x = ImGui::GetContentRegionAvail().x-2;
        if(data.infoTableSize.x < data.minTableWidth)
            data.infoTableSize.x = data.minTableWidth;
        if(data.infoTableSize.x > data.maxTableWidth)
            data.infoTableSize.x = data.maxTableWidth;

        centerNextItemX(data.infoTableSize.x);
        showExistingItemsWindow(database, data.subItem, data.infoTableSize);
    }
    ImGui::EndChild();


}
void showExistingItemsWindow (const Database& database, const SubItem& selected, ImVec2 size){
    static ShootingEventMetadata   selectedEvent    { };
    static GunMetadata             selectedGun      { };
    static AmmoMetadata            selectedAmmo     { };

    ImGui::BeginGroup();
    switch(selected){
        case SubItem::EVENT:
            centerTextDisabled("All Events");
            centerTextDisabled("Select an Event to View Detailed Information");
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
            centerTextDisabled("Select Ammo to View Detailed Information");
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
            ListBoxes::manufacturers(database.getManufacturers(), size);
            break;
        case SubItem::CARTRIDGE:
            centerTextDisabled("All Cartridges");
            ListBoxes::cartridges(database.getCartridges(), size);
            break;
        case SubItem::GUN:
            centerTextDisabled("All Guns");
            centerTextDisabled("Select a Gun to View Detailed Information");
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
            centerTextDisabled("Select an Item");
            break;
    }
    ImGui::EndGroup();
}
void addItemWindow(const Database& database, ScreenData::Add& data){
    switch(data.subItem){
        case SubItem::EVENT:
            EventWindow::main(
                    data.eventWindow, 
                    ShootingEventMetadata::MAX_CHAR_NOTES, 
                    database.getLocations(), 
                    database.getEventTypes(),
                    database.getStockpile(),
                    database.getArmory()
                );
            break;
        case SubItem::EVENT_TYPE:
            Add::add_EventType(data.subItemBuffers.eventType, MAX_CHAR_METADATA_ITEM);
            break;
        case SubItem::LOCATION: 
            Add::add_Location(data.subItemBuffers.location, MAX_CHAR_METADATA_ITEM);
            break;
        case SubItem::AMMO:
       
            break;
        case SubItem::MANUFACTURER:
            Add::add_Manufacturer(data.subItemBuffers.manufacturer, MAX_CHAR_METADATA_ITEM);
            break;
        case SubItem::CARTRIDGE:
            Add::add_Cartridge(data.subItemBuffers.cartridge, MAX_CHAR_METADATA_ITEM);
            break;
        case SubItem::GUN:
            Add::GunWindow::main(data.gunWindow, database.getCartridges(), database.getWeaponTypes());
            break;
        case SubItem::WEAPON_TYPE:
            Add::add_WeaponType(data.subItemBuffers.weaponType, MAX_CHAR_METADATA_ITEM);
            break;
        default:
            centerTextDisabled("Select an Item");
            break;
    }
 
}
void add_Manufacturer(char* textBuf, size_t size){
    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Manufacturer to choose from when creating Ammo.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Text("Manufacturer");
    ImGui::SameLine(150);
    ImGui::InputText("##Input Manufacturer", textBuf, size);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Manufacturer", ImVec2{200,50})){
        DatabaseEvents::Add::Manufacturer e { Manufacturer { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
}
void add_Cartridge       (char* textBuf, size_t size){
    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Cartridge to choose from when creating Ammo and Guns.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Text("Cartridge");
    ImGui::SameLine(150);
    ImGui::InputText("##Input Cartridge", textBuf, size);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Cartrige", ImVec2{200,50})){
        DatabaseEvents::Add::Cartridge e { Cartridge { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
}
void add_EventType       (char* textBuf, size_t size){
    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Event Type to choose from when creating an Event");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Text("Event Type");
    ImGui::SameLine(150);
    ImGui::InputText("##Input Event Type", textBuf, size);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Event Type", ImVec2{200,50})){
        DatabaseEvents::Add::EventType e { ShootingEventType { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
}
void add_WeaponType(char* textBuf, size_t size){
    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Weapon Type to choose from when creating a gun.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Dummy(ImVec2{0.0f, 50.0f});
    
    ImGui::Text("Weapon Type");
    ImGui::SameLine(150);
    ImGui::InputText("##Input Weapon Type", textBuf, size, ImGuiInputTextFlags_CharsUppercase);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Weapon Type", ImVec2{200,50})){
        DatabaseEvents::Add::WeaponType e { WeaponType { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
}
void add_Location(char* textBuf, size_t size){
    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Location to choose from when creating an Event.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Text("Location");
    ImGui::SameLine(150);
    ImGui::InputText("##Input Location", textBuf, size);
    ImGui::Unindent();

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Location", ImVec2{200,50})){
        DatabaseEvents::Add::Location e { Location { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
}
void GunWindow::main(
        ScreenData::Add::GunWindow& data, 
        const std::set<Cartridge>&  cartridges,
        const std::set<WeaponType>& weaponTypes
    )
{
    const ImVec2 regionAvail { ImGui::GetContentRegionAvail() };
    data.topWinSize.x   = regionAvail.x / 2 - 5;
    data.mainWinSize    = ImVec2{ regionAvail.x - 2, regionAvail.y - data.topWinSize.y - 5};

    if(data.topWinSize.x < data.minWinSize.x)
        data.topWinSize.x = data.minWinSize.x;

    if(data.mainWinSize.x < data.minWinSize.x)
        data.mainWinSize.x = data.minWinSize.x;
    if(data.mainWinSize.y < data.minWinSize.y)
        data.mainWinSize.y = data.minWinSize.y;

    if(ImGui::BeginChild("Directions", data.topWinSize)){
        ImGui::Indent(20);
        ImGui::Text("Directions");
        ImGui::BulletText("Enter in new Gun information");
        ImGui::BulletText("Must save before exiting LAS otherwise changes will not be made");
        ImGui::Unindent();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Submit", data.topWinSize)){
        centerNextItemX(data.buttonSize.x * 2 + 20); // 20 is dummy size
        ImGui::BeginGroup();

        if(ImGui::Button("Add Gun", data.buttonSize)){
            DatabaseEvents::Add::Gun addGun { GunMetadata { data.name, data.cartridge, data.weaponType } };
            pushEvent(&addGun);
        }
        ImGui::SameLine();
        ImGui::Dummy( ImVec2 { 20, 10} );
        ImGui::SameLine();
        if(ImGui::Button("Reset All", data.buttonSize)){
            UIEvents::SetScreenData::Add_GunWindow resetInputs { };
            pushEvent(&resetInputs);
        }

        ImGui::EndGroup();
    }
    ImGui::EndChild();

    if(ImGui::BeginChild("Gun Info", data.mainWinSize)){
        ImGui::SeparatorText("Input Gun Information");
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Indent(20);
        ImGui::Text("WeaponType");
        ImGui::SameLine(100);
        ComboBoxes::weaponTypes(weaponTypes, data.weaponType);

        ImGui::Text("Cartridge");
        ImGui::SameLine(100);
        ComboBoxes::cartridges(cartridges, data.cartridge);

        ImGui::Text("Name");
        ImGui::SameLine(100);
        ImGui::InputText("##Input Name", data.name, MAX_CHAR_METADATA_ITEM);

        ImGui::Spacing();
    }
    ImGui::EndChild();


}
void EventWindow::main(  
        ScreenData::Add::EventWindow& data, 
        size_t notesSize,            
        const std::set<Location>& locations,
        const std::set<ShootingEventType>& eventTypes,
        const std::map<Cartridge, std::map<AmmoMetadata, StockpileAmmo>>& stockpile,
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory
    )
{
    if(ImGui::BeginTabBar("Add Event Tabs")){
        if(ImGui::BeginTabItem("Event Information")){
            eventMetadataWindow(data.metadataWindow, data.event, notesSize, locations, eventTypes); 
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Guns and Ammo")){
            gunsAndAmmoWindow(data, data.event, stockpile, armory); 
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Review And Submit")){
            review(data.reviewWindow, data.event); 
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
void EventWindow::eventMetadataWindow(
        ScreenData::Add::EventWindow::MetadataWindow& data, 
        ShootingEvent& event,
        size_t notesSize,            
        const std::set<Location>& locations,
        const std::set<ShootingEventType>& eventTypes
    ) 
{
    using namespace std::chrono;

    const ImVec2 regionAvail { ImGui::GetContentRegionAvail() };

    data.mainWinSize = ImVec2{ regionAvail.x, (data.minWinSize.y + data.buttonSize.y + 30) }; // Only enough for whats needed plus padding

    // Ensure minimum windows
    if(data.mainWinSize.x < (data.minWinSize.x * 2) )
        data.mainWinSize.x = data.minWinSize.x;
    if(data.mainWinSize.y < data.minWinSize.y)
        data.mainWinSize.y = data.minWinSize.y;

    bool submit { false };
    infoWindow(
            event.getInfo(),
            submit,
            data.infoWinSize,
            data.triedToVerifyEventInfo,
            data.minWinSize,
            data.mainWinSize,
            data.buttonSize
        );

    // Button inside infoWindow was pressed
    if(submit){
        data.triedToVerifyEventInfo = true;

        ymd date { year{data.year}, month{data.month}, day{data.day} };

        if(verifyMetadata(data.selectedLocation, data.selectedET, date) ){
            data.triedToVerifyEventInfo = false;

            event.setInfo ( ShootingEventMetadata { 
                    std::string{data.notes},
                    data.selectedLocation,
                    data.selectedET,
                    date
                }
            );
            data.selectedET = EMPTY_EVENT_TYPE;
            data.selectedLocation = EMPTY_LOCATION;
            data.day = 0;
            data.month = 0;
            data.year = 0;
            resetText(data.notes, ShootingEventMetadata::MAX_CHAR_NOTES);
        }
    }
    
 
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText("Input Event Information");
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent(20);
    ImGui::Text("Location");
    ImGui::SameLine(100);
    ComboBoxes::locations(locations, data.selectedLocation);

    ImGui::Text("Event Type");
    ImGui::SameLine(100);
    ComboBoxes::eventTypes(eventTypes, data.selectedET);

    ImGui::Text("Notes");
    ImGui::SameLine(100);
    ImGui::InputTextMultiline("##Input Notes", data.notes, notesSize, ImVec2{600, 100}, ImGuiInputTextFlags_CtrlEnterForNewLine);
    ImGui::SameLine();
    ImGui::TextDisabled("(May be left blank)");

    ImGui::Spacing();

    ImGui::Text("Date");
    ImGui::SameLine(100);
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
    ImGui::SetCursorPosX(125);
    ImGui::TextDisabled("(Day)");
    ImGui::SameLine(0, 70);
    ImGui::TextDisabled("(Month)");
    ImGui::SameLine(0, 70);
    ImGui::TextDisabled("(Year)");
    ImGui::Unindent();
}
void EventWindow::infoWindow(
        const ShootingEventMetadata& event,
        bool&           submitted,
        ImVec2&         subWindowSize,
        const bool      attemptedVerify,
        const ImVec2&   minWinSize,
        const ImVec2&   windowSize,
        const ImVec2&   buttonSize
    )
{
    if(ImGui::BeginChild("Info Window", windowSize)){
        subWindowSize = ImVec2{ (ImGui::GetContentRegionAvail().x / 2 ) - 5, minWinSize.y };

        if(subWindowSize.x < minWinSize.x)
            subWindowSize.x = minWinSize.x;
        if(subWindowSize.y < minWinSize.y)
            subWindowSize.y = minWinSize.y;

        if(ImGui::BeginChild("Directions Window", subWindowSize)){
            ImGui::Indent(20);
            ImGui::Text("Directions");
            ImGui::BulletText("Navigate through the tabs to proceed to create the Event");
            ImGui::BulletText("Once filled out, verify the information to continue");
            ImGui::Unindent();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if(ImGui::BeginChild("Event Metadata Buffer", subWindowSize)){

            if(event != EMPTY_EVENT_METADATA){
                if(ImGui::BeginChild("Info", ImVec2 { ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y })) {
                    centerTextDisabled("Event Information");
                    ImGui::Separator();

                    ImGui::TextDisabled("Date: ");
                    ImGui::SameLine(100);
                    ImGui::Text("%s", printDate(event.date).c_str());

                    ImGui::TextDisabled("Location: ");
                    ImGui::SameLine(100);
                    ImGui::Text("%s", event.location.getName());

                    ImGui::TextDisabled("Event Type: ");
                    ImGui::SameLine(100);
                    ImGui::Text("%s", event.eventType.getName());
                }
                ImGui::EndChild();

                ImGui::SameLine();

                if(ImGui::BeginChild("Notes", ImGui::GetContentRegionAvail() )){
                    centerTextDisabled("Notes");
                    ImGui::Separator();
                    ImGui::Text("%s", event.notes.c_str());
                }
                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
        
        if(centerButton("Submit", buttonSize)){
            submitted = true;
        }
 
        if(attemptedVerify)
            centerTextDisabled("(Make Sure Data is Correct)");
        else
            ImGui::Dummy( ImVec2 { 0, ImGui::CalcTextSize("PLACEHOLDER").y } );
    }
    ImGui::EndChild();

}
bool EventWindow::verifyMetadata(
       const Location& location,
       const ShootingEventType& et,
       const ymd& date
   )
{
    using namespace std::chrono;

    if(location == EMPTY_LOCATION)
        return false;
    if(et == EMPTY_EVENT_TYPE)
        return false;
    
    if(!date.ok())
        return false;

    return true;
}

void EventWindow::gunsAndAmmoWindow(
        ScreenData::Add::EventWindow& eventWindow, 
        ShootingEvent& event,
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&    stockpile,
        const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>&     armory
    )
{
    ScreenData::Add::EventWindow::GunsAndAmmoWindow& data { eventWindow.gunsAndAmmoWindow };

    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("Add type and amount of Ammo used for any number of Guns");
    ImGui::BulletText("Click the button to proceed to the next step");
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
    viewGunsUsedWindow(
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
    }
    
    if(!data.verticalLayout)
        ImGui::SameLine();

    bool removeAmmo { false };
    viewAmmoUsedWindow(
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

        ImGui::BeginDisabled();
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
                addGun(data.addGunWindow, eventWindow.event, armory, data.mainTableSize);
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
                    addAmmoToGun(data.addAmmoWindow, event.getGun(data.selectedGun), stockpile, data.mainTableSize);

                ImGui::EndTabItem();
            }

            if(!data.selectedGunValid)
                ImGui::EndDisabled();

            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
}
void EventWindow::viewGunsUsedWindow(
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
void EventWindow::viewAmmoUsedWindow(
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
void EventWindow::addGun (
        ScreenData::Add::EventWindow::GunsAndAmmoWindow::AddGunWindow& data, 
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
        // TODO -- add commands here for popup
        if(event.hasUsedGun(data.selectedGun))
            std::cout << "gun already exists popup\n";
        else{
            if(event.addGun(GunTrackingAmmoUsed{data.selectedGun}))
                data.selectedGun = EMPTY_GUN_METADATA;
            else
                std::cout << "gun not added popup\n";
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
void EventWindow::addAmmoToGun (
        ScreenData::Add::EventWindow::GunsAndAmmoWindow::AddAmmoWindow& data, 
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
void EventWindow::review(
        ScreenData::Add::EventWindow::ReviewWindow& data,
        ShootingEvent& event
    )
{
    const ImVec2 regionAvail { ImGui::GetContentRegionAvail() };
    data.optionsWinSize = ImVec2{ regionAvail.x / 2 - 5, data.optionsWinSize.y };
    data.reviewWinSize  = ImVec2{ regionAvail.x / 3 - 10, regionAvail.y - data.optionsWinSize.y - 5};

    if(data.reviewWinSize.x < data.minWinSize.x ){
        data.verticalLayout = true;
        data.optionsWinSize.x = regionAvail.x;
        data.reviewWinSize = regionAvail;
    }
    else
        data.verticalLayout = false;

    if(data.optionsWinSize.x < data.minWinSize.x)
        data.optionsWinSize.x = data.minWinSize.x;

    if(data.reviewWinSize.x < data.minWinSize.x)
        data.reviewWinSize.x = data.minWinSize.x;
    if(data.reviewWinSize.y < data.minWinSize.y)
        data.reviewWinSize.y = data.minWinSize.y;

    if(ImGui::BeginChild("Directions", data.optionsWinSize)){
        ImGui::Indent(20);
        ImGui::Text("Directions");
        ImGui::BulletText("Review the following information before submitting");
        ImGui::BulletText("Must save before exiting LAS otherwise changes will not be made");
        ImGui::Unindent();
    }
    ImGui::EndChild();

    if(!data.verticalLayout)
        ImGui::SameLine();

    bool submitEvent { false };
    bool resetInputs { false };
    reviewOptionsWindow(
            submitEvent,
            resetInputs,
            data.applyToArmory,
            data.applyToStockpile,
            data.optionsWinSize,
            data.buttonSize
        );
    if(submitEvent){
        DatabaseEvents::Add::Event add { event, data.applyToArmory, data.applyToStockpile };
        pushEvent(&add);
    }

    if(resetInputs){
        UIEvents::SetScreenData::Add_EventWindow resetBuffers { };
        pushEvent(&resetBuffers);
    }

    data.eventMetadataValid = event.getInfo() != EMPTY_EVENT_METADATA;
    
    reviewEventInfoWindow(
            event.getInfo(),
            data.eventMetadataValid,
            data.reviewWinSize
        );
    
    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Guns Used", data.reviewWinSize)){
        ImGui::SeparatorText("Guns Used");
        ImGui::Spacing();
        ImGui::Spacing();

        data.tableSize.x = ImGui::GetContentRegionAvail().x-2;
        if(data.tableSize.x < data.minTableWidth)
            data.tableSize.x = data.minTableWidth;
        if(data.tableSize.x > data.maxTableWidth)
            data.tableSize.x = data.maxTableWidth;

        if(event.getGunsUsed().empty()) {
            centerNextItemY(5);
            centerTextDisabled("No Guns added to Event");
        }
        else{
            centerNextItemX(data.tableSize.x);
            Tables::Selectable::gunMetadataWithRoundCount(event.getGunsUsed(), data.selectedGun, data.tableSize);
        }
    }
    ImGui::EndChild();

    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Ammo Used", data.reviewWinSize)){
        ImGui::SeparatorText("Ammo Used");
        ImGui::Spacing();
        ImGui::Spacing();

        static size_t lastSize { 0 };
        const size_t currentSize { event.getGunsUsed().size() };

        // Ensure if size changes reset selected gun metadata
        if(lastSize != currentSize){
            data.selectedGun = EMPTY_GUN_METADATA;
            lastSize = currentSize;
        }

        if(data.selectedGun == EMPTY_GUN_METADATA) {
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Ammo Used");
        }
        else if(event.getGun(data.selectedGun).getAmmoUsed().empty()){
            centerNextItemY(5);
            centerTextDisabled(std::format("No Ammo used for '{}'", data.selectedGun.name));
        }
        else{
            centerNextItemX(data.tableSize.x);
            Tables::amountOfAmmo(
                    event.getGun(data.selectedGun).getAmmoUsed(), 
                    data.tableSize
                );
        }
    }
    ImGui::EndChild();
}
void EventWindow::reviewOptionsWindow(
        bool& submitEvent,
        bool& resetInputs,
        bool& applyToArmory,
        bool& applyToStockpile,
        const ImVec2& windowSize,
        const ImVec2& buttonSize
    )
{ 
    if(ImGui::BeginChild("Options", windowSize)){
        const ImVec2 childSizes { ImGui::GetContentRegionAvail().x / 2 - 5, ImGui::GetContentRegionAvail().y - 30 };

        centerTextDisabled("Options And Confirm");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        if(ImGui::BeginChild("Apply", childSizes)){ 
            ImGui::Text("Apply to Stockpile");
            ImGui::SameLine(150);
            ImGui::Checkbox("##Apply To Stockpile", &applyToStockpile);

            ImGui::Text("Apply to Armory");
            ImGui::SameLine(150);
            ImGui::Checkbox("##Apply To Guns", &applyToArmory);
        }
        ImGui::EndChild();

        ImGui::SameLine();
        if(ImGui::BeginChild("Submit", childSizes )){ 
            centerNextItemX(buttonSize.x * 2 + 20); // 20 is dummy size
            ImGui::BeginGroup();

            submitEvent = ImGui::Button("Add Event", buttonSize);

            ImGui::SameLine();
            ImGui::Dummy( ImVec2 { 20, 10} );
            ImGui::SameLine();

            resetInputs = ImGui::Button("Reset All", buttonSize);

            ImGui::EndGroup();
        }    
        ImGui::EndChild();
    }
    ImGui::EndChild();

}
void EventWindow::reviewEventInfoWindow(
        const ShootingEventMetadata& info,
        const bool isEventValid,
        const ImVec2& windowSize
    )
{
    if(ImGui::BeginChild("Event Information", windowSize)){
        ImGui::SeparatorText("Event Information");
        ImGui::Spacing();
        ImGui::Spacing();

        if(!isEventValid) {
            centerNextItemY(5);
            centerTextDisabled("Enter Event Information");
        }
        else{
            centerNextItemX(400);
            ImGui::BeginGroup();
            ImGui::Text("Date:              %s", printDate(info.date).c_str());
            ImGui::Text("Location:          %s", info.location.getName());
            ImGui::Text("Event Type:        %s", info.eventType.getName()); 
            ImGui::EndGroup();

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::SeparatorText("Notes");
            ImGui::Spacing();
            ImGui::Indent(20); 
            ImGui::TextWrapped("%s", info.notes.c_str());
            ImGui::Unindent();
        }
    }
    ImGui::EndChild();
}

}   // End SC::UI::Add
