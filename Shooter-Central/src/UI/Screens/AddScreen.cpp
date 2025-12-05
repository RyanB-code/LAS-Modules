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

            // - TODO - button here to view the gun
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
            Add::add_EventType(data.subItemBuffers.eventType, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::LOCATION: 
            Add::add_Location(data.subItemBuffers.location, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::AMMO:
       
            break;
        case SubItem::MANUFACTURER:
            Add::add_Manufacturer(data.subItemBuffers.manufacturer, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::CARTRIDGE:
            Add::add_Cartridge(data.subItemBuffers.cartridge, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::GUN:

            break;
        case SubItem::WEAPON_TYPE:
            Add::add_WeaponType(data.subItemBuffers.weaponType, ScreenData::Add::MAX_CHAR_INPUT);
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


    if(ImGui::BeginChild("Info Window", data.mainWinSize)){
        data.infoWinSize = ImVec2{ (ImGui::GetContentRegionAvail().x / 2 ) - 5, data.minWinSize.y };

        if(data.infoWinSize.x < data.minWinSize.x)
            data.infoWinSize.x = data.minWinSize.x;
        if(data.infoWinSize.y < data.minWinSize.y)
            data.infoWinSize.y = data.minWinSize.y;

        if(ImGui::BeginChild("Directions Window", data.infoWinSize)){
            ImGui::Indent(20);
            ImGui::Text("Directions");
            ImGui::BulletText("Navigate through the tabs to proceed to create the Event");
            ImGui::BulletText("Once filled out, verify the information to continue");
            ImGui::Unindent();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if(ImGui::BeginChild("Event Metadata Buffer", data.infoWinSize)){
            const ShootingEventMetadata& info { event.getInfo() };

            if(info != EMPTY_EVENT_METADATA){
                if(ImGui::BeginChild("Info", ImVec2 { ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y })) {
                    centerTextDisabled("Event Information");
                    ImGui::Separator();

                    ImGui::TextDisabled("Date: ");
                    ImGui::SameLine(100);
                    ImGui::Text("%s", printDate(info.date).c_str());

                    ImGui::TextDisabled("Location: ");
                    ImGui::SameLine(100);
                    ImGui::Text("%s", info.location.getName());

                    ImGui::TextDisabled("Event Type: ");
                    ImGui::SameLine(100);
                    ImGui::Text("%s", info.eventType.getName());
                }
                ImGui::EndChild();

                ImGui::SameLine();

                if(ImGui::BeginChild("Notes", ImGui::GetContentRegionAvail() )){
                    centerTextDisabled("Notes");
                    ImGui::Separator();
                    ImGui::Text("%s", info.notes.c_str());
                }
                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
        
        if(centerButton("Submit", data.buttonSize)){
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

        if(data.triedToVerifyEventInfo)
            centerTextDisabled("(Make Sure Data is Correct)");
        else
            ImGui::Dummy( ImVec2 { 0, ImGui::CalcTextSize("PLACEHOLDER").y } );
    }
    ImGui::EndChild();

 
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
    

    if(ImGui::BeginChild("Guns Used", data.viewWindowSize)){
        ImGui::SeparatorText("Guns Used");
        ImGui::Spacing();
        ImGui::Spacing();

        if(!data.selectedGunValid)
            ImGui::BeginDisabled();

        if(centerButton("Remove Gun", data.buttonSize)){
            event.removeGun(data.selectedGun);
            data.selectedGun    = EMPTY_GUN_METADATA;
            data.selectedAmmo   = EMPTY_AMMO_METADATA;

            data.selectedGunValid = false;
            ImGui::BeginDisabled();
        }

        if(!data.selectedGunValid)
            ImGui::EndDisabled();


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

            if(!data.selectedAmmoValid)
                ImGui::BeginDisabled();

            if(centerButton("Remove Ammo", data.buttonSize)){
                event.getGun(data.selectedGun).removeAmmoUsed(data.selectedAmmo);

                data.selectedAmmo = EMPTY_AMMO_METADATA;
                data.selectedAmmoValid = false;

                ImGui::BeginDisabled();
            }
            
            if(!data.selectedAmmoValid)
                ImGui::EndDisabled();


            ImGui::Spacing();
            ImGui::Spacing();

            centerNextItemX(data.viewTableSize.x);
            Tables::Selectable::amountOfAmmo(
                    event.getGun(data.selectedGun).getAmmoUsed(), 
                    data.selectedAmmo,
                    data.viewTableSize
                );
            data.selectedAmmoValid = data.selectedAmmo != EMPTY_AMMO_METADATA;
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

    if(ImGui::BeginChild("Options", data.optionsWinSize)){
        const ImVec2 childSizes { ImGui::GetContentRegionAvail().x / 2 - 5, ImGui::GetContentRegionAvail().y - 30 };

        centerTextDisabled("Options And Confirm");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        if(ImGui::BeginChild("Apply", childSizes)){ 
            ImGui::Text("Apply to Stockpile");
            ImGui::SameLine(150);
            ImGui::Checkbox("##Apply To Stockpile", &data.applyToStockpile);

            ImGui::Text("Apply to Armory");
            ImGui::SameLine(150);
            ImGui::Checkbox("##Apply To Guns", &data.applyToArmory);
        }
        ImGui::EndChild();

        ImGui::SameLine();
        if(ImGui::BeginChild("Submit", childSizes )){ 
            centerNextItemX(data.buttonSize.x * 2 + 20); // 20 is dummy size
            ImGui::BeginGroup();

            if(ImGui::Button("Add Event", data.buttonSize)){
                DatabaseEvents::Add::Event add { event };
                pushEvent(&add);
            }
            ImGui::SameLine();
            ImGui::Dummy( ImVec2 { 20, 10} );
            ImGui::SameLine();
            if(ImGui::Button("Reset All", data.buttonSize)){
                UIEvents::SetScreenData::Add_Window resetBuffers { };
                pushEvent(&resetBuffers);
            }
            ImGui::EndGroup();
        }    
        ImGui::EndChild();
    }
    ImGui::EndChild();


    if(ImGui::BeginChild("Event Information", data.reviewWinSize)){
        ImGui::SeparatorText("Event Information");
        ImGui::Spacing();
        ImGui::Spacing();

        const ShootingEventMetadata& info  { event.getInfo() };
        if(info == EMPTY_EVENT_METADATA)
            data.eventMetadataValid = false;
        else
            data.eventMetadataValid = true;

        if(!data.eventMetadataValid) {
            centerNextItemY(5);
            centerTextDisabled("Enter Event Information");
        }
        else{
            centerNextItemX(400);
            ImGui::BeginGroup();
            ImGui::Text("Date:              %s", event.printDate().c_str());
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

    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Guns Used", data.reviewWinSize)){
        ImGui::SeparatorText("Guns Used");
        ImGui::Spacing();
        ImGui::Spacing();

        data.tableSize.x = ImGui::GetContentRegionAvail().x-2;
        if(data.tableSize.x < data.minTableSize.x)
            data.tableSize.x = data.minTableSize.x;
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

}   // End SC::UI::Add
