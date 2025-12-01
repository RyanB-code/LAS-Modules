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

    // Sizes below the combo box to get proper Y coord after the box
    data.mainWindowSize = ImVec2{ (ImGui::GetContentRegionAvail().x / 4) * 3, ImGui::GetContentRegionAvail().y };
    data.infoWindowSize = ImVec2{ (ImGui::GetContentRegionAvail().x / 4) , ImGui::GetContentRegionAvail().y };

    if(data.infoWindowSize.x < data.minWinSize.x ){
        data.verticalLayout = true;
        data.mainWindowSize = ImGui::GetContentRegionAvail();
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
        if(data.infoTableSize.x < data.minTableSize.x)
            data.infoTableSize.x = data.minTableSize.x;
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
        case SubItem::EVENT_EVENT:
            centerTextDisabled("All Events");
            Tables::Selectable::eventsWithNumGunsUsed(database.getEvents(), selectedEvent, size);

            if(selectedEvent != EMPTY_EVENT_METADATA){
                std::cout << "Command change to view event\n";
                selectedEvent = EMPTY_EVENT_METADATA;
            }

            // TODO - button here to view the event 

            break;
        case SubItem::EVENT_TYPE:
            centerTextDisabled("All Event Types");
            ListBoxes::eventTypes(database.getEventTypes(), size);
            break;
        case SubItem::EVENT_LOCATION:
            centerTextDisabled("All Event Locations");
            ListBoxes::eventLocations(database.getLocations(), size);
            break;
        case SubItem::AMMO_AMMO:
            centerTextDisabled("All Ammo");
            Tables::Selectable::ammoMetadata(database.getStockpile(), selectedAmmo, size);

            if(selectedAmmo != EMPTY_AMMO_METADATA){
                std::cout << "Change to view ammo command\n";
                selectedAmmo = EMPTY_AMMO_METADATA;
            }

            // TODO - butti here to view the ammo

            break;
        case SubItem::AMMO_MANUFACTURER:
            centerTextDisabled("All Manufacturers");
            ListBoxes::manufacturers(database.getManufacturers(), size);
            break;
        case SubItem::GUN_AMMO_CARTRIDGE:
            centerTextDisabled("All Cartridges");
            ListBoxes::cartridges(database.getCartridges(), size);
            break;
        case SubItem::GUN_GUN:
            centerTextDisabled("All Guns");
            Tables::Selectable::gunMetadataWithRoundCount(database.getArmory(), selectedGun, size);

            if(selectedGun != EMPTY_GUN_METADATA){
                std::cout << "Command change to view gun\n";
                selectedGun = EMPTY_GUN_METADATA;
            }

            // - TODO - button here to view the gun
            break;
        case SubItem::GUN_WEAPON_TYPE:
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
        case SubItem::EVENT_EVENT:
            EventWindow::main(
                    data.eventWindow, 
                    ScreenData::Add::MAX_CHAR_INPUT, 
                    database.getLocations(), 
                    database.getEventTypes(),
                    database.getStockpile(),
                    database.getArmory()
                );
            break;
        case SubItem::EVENT_TYPE:
            Add::add_EventType(data.subItemBuffers.eventType, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::EVENT_LOCATION: 
            Add::add_Location(data.subItemBuffers.location, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::AMMO_AMMO:
       
            break;
        case SubItem::AMMO_MANUFACTURER:
            Add::add_Manufacturer(data.subItemBuffers.manufacturer, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::GUN_AMMO_CARTRIDGE:
            Add::add_Cartridge(data.subItemBuffers.cartridge, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::GUN_GUN:

            break;
        case SubItem::GUN_WEAPON_TYPE:
            Add::add_WeaponType(data.subItemBuffers.weaponType, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        default:
            centerTextDisabled("Select an Item");
            break;
    }
 
}
void add_Manufacturer(char* textBuf, size_t size){
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Manufacturer to choose from when creating Ammo.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Text("Manufacturer");
    ImGui::SameLine(100);
    ImGui::InputText("##Input Manufacturer", textBuf, size);

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Manufacturer", ImVec2{200,50})){
        DatabaseEvents::AddManufacturer e { Manufacturer { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
}
void add_Cartridge       (char* textBuf, size_t size){
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Cartridge to choose from when creating Ammo and Guns.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Text("Cartridge");
    ImGui::SameLine(100);
    ImGui::InputText("##Input Cartridge", textBuf, size);

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Cartrige", ImVec2{200,50})){
        DatabaseEvents::AddCartridge e { Cartridge { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
}
void add_EventType       (char* textBuf, size_t size){
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Event Type to choose from when creating an Event");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Text("Event Type");
    ImGui::SameLine(100);
    ImGui::InputText("##Input Event Type", textBuf, size);

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Event Type", ImVec2{200,50})){
        DatabaseEvents::AddEventType e { ShootingEventType { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
}
void add_WeaponType(char* textBuf, size_t size){
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Weapon Type to choose from when creating a gun.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Dummy(ImVec2{0.0f, 50.0f});
    
    ImGui::Text("Weapon Type");
    ImGui::SameLine(100);
    ImGui::InputText("##Input Weapon Type", textBuf, size, ImGuiInputTextFlags_CharsUppercase);

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Weapon Type", ImVec2{200,50})){
        DatabaseEvents::AddWeaponType e { WeaponType { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
}
void add_Location(char* textBuf, size_t size){
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Location to choose from when creating an Event.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    ImGui::Text("Location");
    ImGui::SameLine(100);
    ImGui::InputText("##Input Location", textBuf, size);

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add Location", ImVec2{200,50})){
        DatabaseEvents::AddLocation e { Location { textBuf } };
        pushEvent(&e);

        resetText(textBuf, size);
    }
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
            eventMetadataWindow(data, notesSize, locations, eventTypes); 
            ImGui::EndTabItem();
        }
        
        if(!data.eventInfoVerified)
            ImGui::BeginDisabled();

        if(ImGui::BeginTabItem("Guns and Ammo")){
            gunsAndAmmoWindow(data, data.event, stockpile, armory); 
            ImGui::EndTabItem();
        }

        if(!data.eventInfoVerified)
            ImGui::EndDisabled();


        if(!data.gunsVerified) 
            ImGui::BeginDisabled();

        if(ImGui::BeginTabItem("Review And Submit")){
            ImGui::Text("fuck");
            ImGui::EndTabItem();
        }

        if(!data.gunsVerified) 
            ImGui::EndDisabled();


        ImGui::EndTabBar();
    }
}
void EventWindow::eventMetadataWindow(
        ScreenData::Add::EventWindow& eventWindow, 
        size_t notesSize,            
        const std::set<Location>& locations,
        const std::set<ShootingEventType>& eventTypes
    ) 
{
    typedef ScreenData::Add::EventWindow::MetadataWindow MetadataWindow;
    MetadataWindow& data { eventWindow.metadataWindow };
    using namespace std::chrono;

    ImGui::Indent(20);
    ImGui::Text("Directions");
    ImGui::BulletText("Navigate through the tabs to proceed to create the Event");
    ImGui::BulletText("Once filled out, verify the information to continue");
    ImGui::Unindent();

    if(centerButton("Verify Information", eventWindow.verifyButtonSize)){
        eventWindow.triedToVerifyEventInfo = true;

        ymd date { year{data.year}, month{data.month}, day{data.day} };

        if(verifyMetadata(data.selectedLocation, data.selectedET, date) ){
            eventWindow.eventInfoVerified = true;
            eventWindow.triedToVerifyEventInfo = false;

            eventWindow.event.setInfo ( ShootingEventMetadata { 
                    std::string{data.notes},
                    data.selectedLocation,
                    data.selectedET,
                    date
                }
            );

            data = MetadataWindow { };
        }
    }

    if(eventWindow.eventInfoVerified)
        centerTextDisabled("(Proceed to Next Tab)");
    else if(eventWindow.triedToVerifyEventInfo)
        centerTextDisabled("(Make Sure Data is Correct)");
    else
        ImGui::Dummy( ImVec2 { 0, ImGui::CalcTextSize("PLACEHOLDER").y } );

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
bool EventWindow::verifyMetadata(
       Location& location,
       ShootingEventType& et,
       ymd date
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

    static ImVec2 regionAvail { ImGui::GetContentRegionAvail() };
    regionAvail = ImGui::GetContentRegionAvail();

    data.optionsWinSize.x = regionAvail.x;
    if(data.optionsWinSize.x < data.minWinSize.x)
        data.optionsWinSize.x = data.minWinSize.x;

    if(ImGui::BeginChild("Add Gun Directions", data.optionsWinSize) ){
        ImGui::Text("Directions");
        ImGui::BulletText("Add type and amount of Ammo used for any number of Guns");
        ImGui::BulletText("Click the button to proceed to the next step");
    }
    ImGui::EndChild();

    // Do this after directions since that will be unchanging
    regionAvail = ImGui::GetContentRegionAvail();
    data.mainWindowSize = ImVec2 { (regionAvail.x / 4 ) * 2 - 5, regionAvail.y }; // Minus 5 for offset stuff
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
    

    if(ImGui::BeginChild("Guns Used", data.viewWindowSize)){
        ImGui::SeparatorText("Guns Used");
        ImGui::Spacing();
        ImGui::Spacing();

        data.viewTableSize.x = ImGui::GetContentRegionAvail().x-2;
        if(data.viewTableSize.x < data.minTableSize.x)
            data.viewTableSize.x = data.minTableSize.x;
        if(data.viewTableSize.x > data.maxTableWidth)
            data.viewTableSize.x = data.maxTableWidth;

        centerNextItemX(data.viewTableSize.x);
        Tables::Selectable::gunMetadataWithRoundCount(event.getGunsUsed(), data.selectedGun, data.viewTableSize);
        data.selectedGunValid = data.selectedGun != EMPTY_GUN_METADATA;
    }
    ImGui::EndChild();

    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Ammo Used", data.viewWindowSize)){
        ImGui::SeparatorText("Ammo Used"); 
        ImGui::Spacing();
        ImGui::Spacing();

        if(!data.selectedGunValid){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Ammo Used");
        }
        else{
            centerNextItemX(data.viewTableSize.x);
            Tables::amountOfAmmo(
                    event.getGun(data.selectedGun).getAmmoUsed(), 
                    data.viewTableSize
                );
        }
    }
    ImGui::EndChild();

    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Add Area", data.mainWindowSize)){
        data.mainTableSize.x = ImGui::GetContentRegionAvail().x-2;
        if(data.mainTableSize.x < data.minTableSize.x)
            data.mainTableSize.x = data.minTableSize.x;
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
                addAmmoToGun(data.selectedGun, stockpile);
                ImGui::EndTabItem();
            }

            if(!data.selectedGunValid)
                ImGui::EndDisabled();

            ImGui::EndTabBar();
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
    
    centerNextItemX(tableSize.x);
    Tables::Selectable::gunMetadataWithRoundCount(armory, data.selectedGun, tableSize);
}
void EventWindow::addAmmoToGun (
        GunMetadata& selected,
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&
    )
{
    /*
    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    ImVec2 tableSize = { ImGui::GetContentRegionAvail().x-2, 400};
    if(tableSize.x < MIN_TABLE_SIZE_X)
        tableSize.x = MIN_TABLE_SIZE_X;
    if(tableSize.x > MAX_TABLE_SIZE_X)
        tableSize.x = MAX_TABLE_SIZE_X;

    ImGui::Spacing();
    ImGui::Spacing();

    static AmmoMetadata selectedAmmo { };
    static int amountBuffer { 0 };

    bool isAmmoValid { selectedAmmo != EMPTY_AMMO_METADATA && amountBuffer > 0 };

    if(!isAmmoValid)
        ImGui::BeginDisabled();

    if(centerButton("Add Ammo", ImVec2 { 100, 30 })){
        ammoUsed = AmountOfAmmo { selectedAmmo, amountBuffer }; 
        selectedAmmo = EMPTY_AMMO_METADATA;
        amountBuffer = 0;
    }

    if(!isAmmoValid)
        ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Spacing();

    // amount of ammo here
    centerNextItemX(300);
    ImGui::BeginGroup();
    ImGui::Text("Amount"); 
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    ImGui::InputInt("##Amount Of Ammo", &amountBuffer, 1, 50);
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Spacing();
    
    centerNextItemX(tableSize.x);
    Tables::Selectable::ammoAmountOnHand(ammoList, selectedAmmo, tableSize);
    */
}



}
