#include "AddScreen.h"

namespace ShooterCentral::UI {

void Add::main(const Database& database, ScreenData::Add& data){
    static constexpr float MIN_WIN_SIZE_X { 200 };
    static constexpr float MIN_WIN_SIZE_Y { 600 };

    static constexpr float MIN_TABLE_SIZE_X { 200 };
    static constexpr float MAX_TABLE_SIZE_X { 400 };

    bool verticalLayout {false};

    ImVec2 topWindowSize;
    ImVec2 bottomWindowSize;
    ImVec2 topTableSize;

    if(ImGui::GetContentRegionAvail().x < (MIN_WIN_SIZE_X * 2)){
        verticalLayout = true;

        topWindowSize  = {MIN_WIN_SIZE_X * 3, 800};
        bottomWindowSize = topWindowSize;
    }
    else{
        topWindowSize  = {ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2};
        if(topWindowSize.x < MIN_WIN_SIZE_X)
            topWindowSize.x = MIN_WIN_SIZE_X;
        if(topWindowSize.y < MIN_WIN_SIZE_Y)
            topWindowSize.y = MIN_WIN_SIZE_Y;

        bottomWindowSize =  { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2};
        if(bottomWindowSize.x < MIN_WIN_SIZE_X)
            bottomWindowSize.x = MIN_WIN_SIZE_X;
        if(bottomWindowSize.y < MIN_WIN_SIZE_Y)
            bottomWindowSize.y = MIN_WIN_SIZE_Y;
    }

    // Size the table
    topTableSize = { topWindowSize.x-2, 400};
    if(topTableSize.x < MIN_TABLE_SIZE_X)
        topTableSize.x = MIN_TABLE_SIZE_X;
    if(topTableSize.x > MAX_TABLE_SIZE_X)
        topTableSize.x = MAX_TABLE_SIZE_X;


    if(ImGui::BeginChild("Select Category", topWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::Spacing();
        ImGui::Spacing();

        centerNextItemY(20);
        ImGui::BeginGroup();

        centerNextComboBoxX("   Select An Item", 200);
        ComboBoxes::subItem(data.subItem);

        ImGui::EndGroup();

        ImGui::Spacing();
        ImGui::Spacing();
    }
    ImGui::EndChild();

    if(!verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Existing Items View", topWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::SeparatorText("View Existing Items"); 
        ImGui::Spacing();
        ImGui::Spacing();

        centerNextItemX(topTableSize.x);
        centerNextItemY(topTableSize.y);
        showExistingItemsWindow(database, data.subItem, topTableSize);
    }
    ImGui::EndChild();

    if(ImGui::BeginChild("Add Item Window", bottomWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::SeparatorText("Add Item"); 

        ImGui::Dummy(ImVec2{0.0f, 20.0f});

        addItemWindow(database, data);
    }
    ImGui::EndChild();

}
void Add::showExistingItemsWindow (const Database& database, const SubItem& selected, ImVec2 size){
    ShootingEventMetadata   selectedEvent;
    GunMetadata             selectedGun;
    AmmoMetadata            selectedAmmo;

    ImGui::BeginGroup();
    switch(selected){
        case SubItem::EVENT_EVENT:
            centerTextDisabled("All Events");
            Tables::Selectable::eventsWithGunsUsed(database.getEvents(), selectedEvent, size);

            if(selectedEvent != EMPTY_EVENT_METADATA)
                std::cout << "Command change to view event\n";

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

            if(selectedAmmo != EMPTY_AMMO_METADATA)
                std::cout << "Change to view ammo command\n";

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

            if(selectedGun != EMPTY_GUN_METADATA)
                std::cout << "Command change to view gun\n";

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
void Add::addItemWindow(const Database& database, ScreenData::Add& screenData){
    switch(screenData.subItem){
        case SubItem::EVENT_EVENT:
            Add::add_Event(
                    screenData.eventBuffer, 
                    ScreenData::Add::MAX_CHAR_INPUT, 
                    database.getLocations(), 
                    database.getEventTypes(),
                    database.getStockpile(),
                    database.getArmory()
                );
            break;
        case SubItem::EVENT_TYPE:
            Add::add_EventType(screenData.subItemBuffers.eventType, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::EVENT_LOCATION: 
            Add::add_Location(screenData.subItemBuffers.location, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::AMMO_AMMO:
       
            break;
        case SubItem::AMMO_MANUFACTURER:
            Add::add_Manufacturer(screenData.subItemBuffers.manufacturer, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::GUN_AMMO_CARTRIDGE:
            Add::add_Cartridge(screenData.subItemBuffers.cartridge, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        case SubItem::GUN_GUN:

            break;
        case SubItem::GUN_WEAPON_TYPE:
            Add::add_WeaponType(screenData.subItemBuffers.weaponType, ScreenData::Add::MAX_CHAR_INPUT);
            break;
        default:
            centerTextDisabled("Select an Item");
            break;
    }
 
}
void Add::add_Manufacturer(char* textBuf, size_t size){
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
void Add::add_Cartridge       (char* textBuf, size_t size){
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
void Add::add_EventType       (char* textBuf, size_t size){
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
void Add::add_WeaponType(char* textBuf, size_t size){
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
void Add::add_Location(char* textBuf, size_t size){
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
void Add::add_Event(  
    ScreenData::Add::EventBuffer& buffer, 
    size_t size,            
    const std::set<Location>& locations,
    const std::set<ShootingEventType>& eventTypes,
    const std::map<Cartridge, std::map<AmmoMetadata, StockpileAmmo>>& stockpile,
    const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory
){
    typedef ScreenData::Add::EventTab EventTab;

    static EventTab lastTab;
    ImGuiTabItemFlags eventInfoFlags { 0 }, gunsUsedFlags { 0 }, reviewFlags { 0 };
    bool applyForce { false };

    if(lastTab != buffer.currentTab)
        applyForce = true;

    if(applyForce){
        switch(buffer.currentTab){
            case EventTab::INFO:
                eventInfoFlags   |= ImGuiTabItemFlags_SetSelected;
                break;
            case EventTab::GUNS_AND_AMMO:
                gunsUsedFlags    |= ImGuiTabItemFlags_SetSelected;
                break;
            case EventTab::REVIEW_AND_SUBMIT:
                reviewFlags      |= ImGuiTabItemFlags_SetSelected;
                break;
            default:
                LAS::log_warn("SC Add Event Tab case not handled");
                break;
        }
    }

    if(ImGui::BeginTabBar("Add Event Tabs")){
        if(ImGui::BeginTabItem("Event Information", nullptr, eventInfoFlags)){
            buffer.currentTab = EventTab::INFO;
            ImGui::EndTabItem();
        }
        
        if(!buffer.eventInfoVerified)
            ImGui::BeginDisabled();

        if(ImGui::BeginTabItem("Guns and Ammo", nullptr, gunsUsedFlags)){
            buffer.currentTab = EventTab::GUNS_AND_AMMO;
            ImGui::EndTabItem();
        }

        if(!buffer.eventInfoVerified)
            ImGui::EndDisabled();


        if(!buffer.gunsVerified) 
            ImGui::BeginDisabled();

        if(ImGui::BeginTabItem("Review And Submit", nullptr, reviewFlags)){
            buffer.currentTab = EventTab::REVIEW_AND_SUBMIT;
            ImGui::EndTabItem();
        }

        if(!buffer.gunsVerified) 
            ImGui::EndDisabled();


        ImGui::EndTabBar();
    }
    
    lastTab = buffer.currentTab;

    switch(buffer.currentTab){
        case EventTab::INFO:
            add_Event_InformationTab(buffer, size, locations, eventTypes); 
            break;
        case EventTab::GUNS_AND_AMMO:
            add_Event_GunsUsedTab(buffer.gunsUsed, buffer.selectedGun, buffer.addItemsCurrentTab, stockpile, armory); 
            break;
        case EventTab::REVIEW_AND_SUBMIT:
            break;
        default:
            LAS::log_warn("SC Add Event Tab case not handled");
            break;
    }

}
void Add::add_Event_InformationTab(
                ScreenData::Add::EventBuffer& buffer, 
                size_t size,            
                const std::set<Location>& locations,
                const std::set<ShootingEventType>& eventTypes
) {
    if(ImGui::BeginChild("Add Event Directions", ImVec2{ImGui::GetContentRegionAvail().x / 2, 120} ) ) {
        ImGui::Text("Directions");
        ImGui::BulletText("Navigate through the tabs to proceed to create the Event");
        ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    }
    ImGui::EndChild();

    ImGui::SameLine(); 

    if(ImGui::BeginChild("Add Event Options", ImVec2{ImGui::GetContentRegionAvail().x, 120})){

        ImGui::Spacing();
        ImGui::Spacing();

        if(ImGui::BeginChild("Apply Amounts", ImVec2{ImGui::GetContentRegionAvail().x/2, 110})) {
            ImGui::Text("Apply to Stockpile");
            ImGui::SameLine(135);
            ImGui::Checkbox("##Apply To Stockpile", &buffer.applyToStockpile);

            ImGui::Text("Apply to Armory");
            ImGui::SameLine(135);
            ImGui::Checkbox("##Apply To Guns", &buffer.applyToArmory);
        }
        ImGui::EndChild();

    }
    ImGui::EndChild();

    if(centerButton("Next", ImVec2{200,50})){
        if(add_Event_verifyInformation(buffer)){
            buffer.currentTab = ScreenData::Add::EventTab::GUNS_AND_AMMO;
            buffer.eventInfoVerified = true;
            std::cout << "verified\n";
        }
        else
            std::cout << "Popup verify failed\n";
    }

    ImGui::SeparatorText("Input Event Information");

    ImGui::Text("Location");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Choose Location Combo", buffer.location.getName(), ImGuiComboFlags_HeightSmall)) {
        for (const auto& loc : locations) {
            bool isSelected { buffer.location == loc };

            if (ImGui::Selectable(loc.getName(), isSelected)){
                buffer.location = loc;
                ImGui::SetItemDefaultFocus();
            } 
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Event Type");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Choose Event Type Combo", buffer.eventType.getName(), ImGuiComboFlags_HeightSmall)) {
        for (const auto& et : eventTypes) {
            bool isSelected {buffer.eventType == et};

            if (ImGui::Selectable(et.getName(), isSelected)){
                buffer.eventType = et;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Notes");
    ImGui::SameLine(100);
    ImGui::InputTextMultiline("##Input Notes", buffer.notes, size, ImVec2{ImGui::GetContentRegionAvail().x, 100}, ImGuiInputTextFlags_CtrlEnterForNewLine);
    ImGui::SameLine();
    ImGui::TextDisabled("(May be left blank)");

    ImGui::Spacing();

    ImGui::Text("Date");
    ImGui::SameLine(100);
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Input Day", &buffer.day);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Input Month", &buffer.month);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Input Year", &buffer.year);

    // Button to auto fill today's date
    ImGui::SameLine(0, 30);
    if(ImGui::Button("Today", ImVec2(75, 20))){
        const std::chrono::zoned_time now {std::chrono::current_zone(), std::chrono::system_clock::now( ) };

        const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now.get_local_time())};
        buffer.day = static_cast<unsigned>(ymd.day());
        buffer.month = static_cast<unsigned>(ymd.month());
        buffer.year = static_cast<int>(ymd.year());
    }
    // Grayed out details, approximate centers of text box did the trick
    ImGui::SetCursorPosX(125);
    ImGui::TextDisabled("(Day)");
    ImGui::SameLine(0, 70);
    ImGui::TextDisabled("(Month)");
    ImGui::SameLine(0, 70);
    ImGui::TextDisabled("(Year)");
}
bool Add::add_Event_verifyInformation(const ScreenData::Add::EventBuffer& buffer){
    using namespace std::chrono;

    static const ScreenData::Add::EventBuffer EMPTY_BUFFER { }; 

    if(EMPTY_BUFFER.location == buffer.location)
        return false;
    if(EMPTY_BUFFER.eventType == buffer.eventType)
        return false;
    
    const year_month_day  dateBuf { year{buffer.year}, month{buffer.month}, day{buffer.day}};

    if(!dateBuf.ok())
        return false;

    return true;
}

void Add::add_Event_GunsUsedTab(
    std::vector<GunTrackingAmmoUsed>& gunsUsed,
    std::vector<GunTrackingAmmoUsed>::iterator& selectedGun,
    ScreenData::Add::EventTab_AddItemsScreen& currentTab,
    const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>&    stockpile,
    const std::map<Cartridge, std::map<GunMetadata,   ArmoryGun>>&     armory
){
    // Table size calculations
    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    ImVec2 tableSize = { ImGui::GetContentRegionAvail().x-2, 400};
    if(tableSize.x < MIN_TABLE_SIZE_X)
        tableSize.x = MIN_TABLE_SIZE_X;
    if(tableSize.x > MAX_TABLE_SIZE_X)
        tableSize.x = MAX_TABLE_SIZE_X;

    if(ImGui::BeginChild("Add Gun Directions", ImVec2{ImGui::GetContentRegionAvail().x / 2, 120} ) ) {
        ImGui::Text("Directions");
        ImGui::BulletText("Add type and amount of Ammo used for any number of Guns");
        ImGui::BulletText("Click the button to proceed to the next step");
    }
    ImGui::EndChild();

    ImGui::SameLine(); 

    if(ImGui::BeginChild("Add Gun Options", ImVec2{ImGui::GetContentRegionAvail().x, 120})){
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Button("Change Me", ImVec2{100, 50});
    }
    ImGui::EndChild();

    if(ImGui::BeginChild("Guns Info", ImVec2{ImGui::GetContentRegionAvail().x/4, 400}, ImGuiChildFlags_Border)){
        ImGui::SeparatorText("Guns Used");

        if(centerButton("Add New Gun", ImVec2{100, 30} ))
            currentTab = ScreenData::Add::EventTab_AddItemsScreen::GUN;

        ImGui::Spacing();
        ImGui::Spacing();

        centerNextItemX(ImGui::GetContentRegionAvail().x);
        Tables::Selectable::gunMetadata(gunsUsed, selectedGun, ImVec2{ImGui::GetContentRegionAvail().x-2, 350 });
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Ammo Used", ImVec2{ImGui::GetContentRegionAvail().x/4, 400}, ImGuiChildFlags_Border)){
        ImGui::SeparatorText("Ammo Used"); 

        if(selectedGun == gunsUsed.end())
            ImGui::BeginDisabled();

        if(centerButton("Add Ammo To Gun", ImVec2{150, 30} ))
            currentTab = ScreenData::Add::EventTab_AddItemsScreen::AMMO;

        if(selectedGun == gunsUsed.end())
            ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Spacing();
    
        if(selectedGun == gunsUsed.end()){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Ammo Used");
        }
        else{
            centerNextItemX(ImGui::GetContentRegionAvail().x);
            Tables::amountOfAmmo(selectedGun->getAmmoUsed(), ImVec2{ImGui::GetContentRegionAvail().x-2, 350 });
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Add Area", ImVec2{ImGui::GetContentRegionAvail().x, 400}, ImGuiChildFlags_Border)){
        if(currentTab == ScreenData::Add::EventTab_AddItemsScreen::GUN){
           // Revalidate selectedGun itr if an insertion took place
           if(add_Event_Gun(armory, gunsUsed))
               selectedGun = gunsUsed.end();
        }
        else if(currentTab == ScreenData::Add::EventTab_AddItemsScreen::AMMO){
            static AmountOfAmmo ammoBuf { };

            const Cartridge& selectedCartridge { selectedGun->getGunInfo().cartridge };

            if(selectedGun == gunsUsed.end()){
                centerNextItemY(5);
                centerTextDisabled("Select a Gun To Add Ammo");
            }
            else if(!stockpile.contains(selectedCartridge)){
                centerNextItemY(5);
                centerTextDisabled("There are no Ammo Types for that Cartridge");
            }
            else{
                ImGui::SeparatorText(
                        std::format("Add '{}' Ammo to Gun '{}'", 
                            selectedCartridge.getName(), 
                            selectedGun->getGunInfo().name
                        ).c_str() 
                    );

                if(add_Event_AmmoForGun(stockpile.at(selectedCartridge), ammoBuf)){
                    selectedGun->addAmmoUsed(ammoBuf);
                    ammoBuf = EMPTY_AMOUNT_OF_AMMO;
                }
            }
        }
    }
    ImGui::EndChild();
}

bool Add::add_Event_Gun (
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& allGuns,
        std::vector<GunTrackingAmmoUsed>& gunsUsed
    )
{
    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    ImVec2 tableSize = { ImGui::GetContentRegionAvail().x-2, 400};
    if(tableSize.x < MIN_TABLE_SIZE_X)
        tableSize.x = MIN_TABLE_SIZE_X;
    if(tableSize.x > MAX_TABLE_SIZE_X)
        tableSize.x = MAX_TABLE_SIZE_X;

    ImGui::Spacing();
    ImGui::Spacing();

    static GunMetadata selectedGun { };

    bool isGunValid { selectedGun != EMPTY_GUN_METADATA };

    if(!isGunValid)
        ImGui::BeginDisabled();

    if(centerButton("Add Gun", ImVec2 { 100, 30 })){
        gunsUsed.emplace_back( GunTrackingAmmoUsed { selectedGun } );
        selectedGun = EMPTY_GUN_METADATA;
        return true; 
    }

    if(!isGunValid)
        ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Spacing();
    
    centerNextItemX(tableSize.x);
    Tables::Selectable::gunMetadataWithRoundCount(allGuns, selectedGun, tableSize);

    return false;
}
bool Add::add_Event_AmmoForGun (
        const std::map<AmmoMetadata,  StockpileAmmo>& ammoList,
        AmountOfAmmo& ammoUsed
    )
{
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
        return true;
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

    return false;
}



}
