#include "Window.h"

using namespace ShooterCentral;


ShooterCentralWindow::ShooterCentralWindow() : LAS::Windowing::Window{"Shooter Central"} 
{

}
ShooterCentralWindow::~ShooterCentralWindow(){

}
void ShooterCentralWindow::draw() {
    if(!ImGui::Begin(title.c_str(), &shown, ImGuiWindowFlags_MenuBar)){
        ImGui::End();
        return;
    }

    if(!ammoTracker || !gunTracker || !eventTracker){
        ImGui::Text("This module has not been setup yet...");
        ImGui::End();
        return;
    }

    if(ImGui::BeginTabBar("Shooter Central Tabs",   ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                                    ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown
    )){
        if(ImGui::BeginTabItem("Home")){
            ImGui::BeginChild("Home");
            drawHome(ImGui::GetContentRegionAvail());
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Stockpile")){
            ImGui::BeginChild("Stockpile");
            drawStockpile();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Events")){
            ImGui::BeginChild("Events");
            drawEvents();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}
bool ShooterCentralWindow::setAmmoTracker(AmmoTrackerPtr setAmmoTracker){
    if(!setAmmoTracker)
        return false;

    ammoTracker = setAmmoTracker;
    return true;
}
bool ShooterCentralWindow::setGunTracker(GunTrackerPtr setGunTracker){
    if(!setGunTracker)
        return false;

    gunTracker = setGunTracker;
    return true;
}
bool ShooterCentralWindow::setEventTracker(EventTrackerPtr setEventTracker){
    if(!setEventTracker)
        return false;

    eventTracker = setEventTracker;
    return true;
}

// MARK: PRIVATE FUNCTIONS

// MARK: HOME
void ShooterCentralWindow::drawHome(ImVec2 windowSize) const{
    windowSize.y -= 10;
    ImVec2  childWindowSizes;
    bool    horizontalLayout { false };

    // Horizontal bigger than vertical
    if( (windowSize.x / 3) > (windowSize.y / 3 ) ){
        childWindowSizes.x = windowSize.x / 3;
        childWindowSizes.y = windowSize.y;
        horizontalLayout = true;
    }
    else{
        childWindowSizes.x = windowSize.x;
        childWindowSizes.y = windowSize.y / 3;
        horizontalLayout = false;
    }

    if(ImGui::BeginChild("Armory Quick View", childWindowSizes, ImGuiChildFlags_Border))
        drawArmoryQuickView(ImGui::GetContentRegionAvail());
    ImGui::EndChild();

    if(horizontalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Guns Quick View", childWindowSizes, ImGuiChildFlags_Border))
        ImGui::Text("Child Window Guns");
    ImGui::EndChild();

    if(horizontalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Events Quick View", childWindowSizes, ImGuiChildFlags_Border))
        ImGui::Text("Child Window Events");
    ImGui::EndChild();
}

// MARK: STOCKPILE
void ShooterCentralWindow::drawStockpile() const{
    static StringVector ammoNames;
    static StringVector cartridgeNames;
    static std::vector<std::pair<std::string, uint64_t>> countByCartridge;
    static std::vector<TrackedAmmo>    ammo;

    ammoTracker->getAllAmmoNames(ammoNames);
    ammoTracker->getAllCartridgeNames(cartridgeNames);
    ammoTracker->getAmmoCountByCartridge(countByCartridge);
    ammoTracker->getAllAmmo(ammo);

    ImGui::SeparatorText("Stockpile");

    // For testing
    static int nameCount{0}, amountCount {10};
    if(ImGui::Button("Make new ammo")){
        std::ostringstream name;
        ++nameCount; amountCount += 2;
        name << "Test Ammo " << nameCount;
        AmmoType ammoBuf { name.str(), "Test man", "Test cart", 69};
        ammoTracker->addAmmoToStockpile(amountCount, ammoBuf);
    }
    ImGui::SameLine();
    if(ImGui::Button("Write ammo")){
        ammoTracker->writeAllAmmo();
    }
    ImGui::SameLine();
    if(ImGui::Button("Read ammo")){
        ammoTracker->readAllAmmo();
    }

    static bool detailedView { false };
    ImGui::Checkbox("Detailed View", &detailedView);

    if(detailedView){
        static int  highlilghtedItem        { 0 };
        static bool showAllAmmo             { false };
        static int  columnsInDetailedTable  { 5 };

        const char* cartridgeList[NUM_CARTRIDGE_NAMES_SHOWN];
        cartridgeList[0] = "ALL";       // Default option

        static std::vector<TrackedAmmo>    selectedCatridgeAmmoList;
        int i { 1 };                                    // Starts at one to offset the all option
        for(const auto& s : cartridgeNames){
            if(i < (NUM_CARTRIDGE_NAMES_SHOWN+1)){      // Add one to offset the additional 'all' option
                cartridgeList[i] = s.c_str();
                ++i;
            }
        }

        ImGui::Combo("Cartridge", &highlilghtedItem, cartridgeList, cartridgeNames.size()+1); // This is the selector table. Add one to account for 'all' option
        ammoTracker->getAllAmmoByCartridge(selectedCatridgeAmmoList, cartridgeList[highlilghtedItem]);  // Populates list of ammo with selected cartridge

        // Change table based on what is highlighted
        if(highlilghtedItem == 0){
            showAllAmmo = true;
            columnsInDetailedTable = 5;
        }
        else{
            showAllAmmo = false;
            columnsInDetailedTable = 4;
        }

        if(ImGui::BeginTable("Detailed Cartridge Breakdown", columnsInDetailedTable, 
                                                ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                                ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                                ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_HighlightHoveredColumn |
                                                ImGuiTableFlags_NoHostExtendX
                            ))
        {
            if(showAllAmmo)
                ImGui::TableSetupColumn("Cartridge",    0, 100);
            ImGui::TableSetupColumn("Name",         0, 100);
            ImGui::TableSetupColumn("Manufacturer", 0, 100);
            ImGui::TableSetupColumn("Grain Weight", 0, 100);
            ImGui::TableSetupColumn("Amount",       0, 100);
            ImGui::TableHeadersRow();

            if(showAllAmmo){
                for(auto itr { ammo.begin() }; itr != ammo.end(); ++itr){
                    ImGui::TableNextRow();
                    for (int column{0}; column < columnsInDetailedTable; ++column)
                    {
                        ImGui::TableSetColumnIndex(column);
                        switch( column ){
                            case 0:
                                ImGui::Text("%s", itr->ammoType.cartridge.c_str());
                                break;
                            case 1:
                                ImGui::Text("%s", itr->ammoType.name.c_str());
                                break;
                            case 2:
                                ImGui::Text("%s", itr->ammoType.manufacturer.c_str());
                                break;
                            case 3:
                                ImGui::Text("%d", int{itr->ammoType.grainWeight});
                                break;
                            case 4:
                                ImGui::Text("%lu", itr->amount);
                                break;
                            default:
                                ImGui::Text("Broken table");
                                break;
                        }
                    }        
                } // End populating all table
            }
            else{
                for(auto itr { selectedCatridgeAmmoList.begin() }; itr != selectedCatridgeAmmoList.end(); ++itr){
                    ImGui::TableNextRow();
                    for (int column{0}; column < columnsInDetailedTable; ++column)
                    {
                        ImGui::TableSetColumnIndex(column);
                        switch( column ){
                            case 0:
                                ImGui::Text("%s", itr->ammoType.name.c_str());
                                break;
                            case 1:
                                ImGui::Text("%s", itr->ammoType.manufacturer.c_str());
                                break;
                            case 2:
                                ImGui::Text("%d", int{itr->ammoType.grainWeight});
                                break;
                            case 3:
                                ImGui::Text("%lu", itr->amount);
                                break;
                            default:
                                ImGui::Text("Broken table");
                                break;
                        }
                    }        
                } // End populating selected cartridge table
            }

            ImGui::EndTable();
        } // End table

        ImGui::Separator();

    } // End detailed view


    if(cartridgeNames.empty())
        ImGui::Text("There are no tracked cartridges.");
    else{
        if(ImGui::BeginTable("Stockpile", 2, 
                                                ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                                ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                                ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_HighlightHoveredColumn |
                                                ImGuiTableFlags_NoHostExtendX
                                                ))
        {
            ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None, 100);
            ImGui::TableSetupColumn("Amount",       ImGuiTableColumnFlags_None, 100);
            ImGui::TableHeadersRow();

            // Display each item
            for(auto itr { countByCartridge.begin() }; itr != countByCartridge.end(); ++itr){
                ImGui::TableNextRow();
                for (int column{0}; column < 2; ++column)
                {
                    ImGui::TableSetColumnIndex(column);
                    switch( column ){
                        case 0:
                            ImGui::Text("%s", itr->first.c_str());
                            break;
                        case 1:
                            ImGui::Text("%lu", itr->second);
                            break;
                        default:
                            ImGui::Text("Broken table");
                            break;
                    }
                }
            }

            ImGui::EndTable();
        } // End table
    }
}
// MARK: EVENTS
void ShooterCentralWindow::drawEvents() const {
    static std::vector<Event> eventList;
    std::vector<Event>::iterator itr;

    eventTracker->getAllEvents(eventList);

    ImGui::SeparatorText("Events");


    if(ImGui::Button("New event")){
        static int num { 0 };
        ++num;

        std::ostringstream os;
        os << "Event Name " << num;

        Event eventBuf { os.str(), "Paul Bunyan", EventTypes::USPSA_MATCH, "N/A", Datestamp{std::chrono::system_clock::now()}};

        eventBuf.addGun(
                Gun { os.str(), WeaponTypes::RIFLE, "test cart" }, 
                TrackedAmmo{ AmmoType{"Test name", "test Man", "test cart", 77}, 250}
            );

        eventTracker->addEvent(eventBuf);
    }
    ImGui::SameLine();
    if(ImGui::Button("Write events")){
        eventTracker->writeAllEvents();
    }
    ImGui::SameLine();
    if(ImGui::Button("Read events")){
        eventTracker->readEvents();
    }

    if(ImGui::BeginTable("Event Table", 5, 
                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                    ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_HighlightHoveredColumn |
                                    ImGuiTableFlags_NoHostExtendX
                            ))
    {
        ImGui::TableSetupColumn("Date",         ImGuiTableColumnFlags_None, 100);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None, 100);
        ImGui::TableSetupColumn("Location",     ImGuiTableColumnFlags_None, 100);
        ImGui::TableSetupColumn("Event Type",   ImGuiTableColumnFlags_None, 100);
        ImGui::TableSetupColumn("Guns Used",    ImGuiTableColumnFlags_None, 100);

        ImGui::TableHeadersRow();

        for(auto itr { eventList.begin() }; itr != eventList.end(); ++itr){
                ImGui::TableNextRow();
                for (int column{0}; column < 4; ++column)
                {
                    const Event& event {*itr};

                    ImGui::TableSetColumnIndex(column);
                    switch( column ){
                        case 0:
                            ImGui::Text("%s", event.getDatestamp().printDate().c_str());
                            break;
                        case 1:
                            ImGui::Text("%s", event.getName().c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", event.getLocation().c_str());
                            break;
                        case 3:
                            ImGui::Text("%s", event.getEventType().getName().c_str());
                            break;
                        case 4:
                            ImGui::Text("%d", int{event.getNumGunsUsed()});
                            break;
                        default:
                            ImGui::Text("Broken table");
                            break;
                    }
                }        
            }

        ImGui::EndTable();
    }
}
// MARK: QUICK VIEW ARMORY
void ShooterCentralWindow::drawArmoryQuickView(ImVec2 windowSize) const{
    static std::vector<Gun> gunList;
    gunTracker->getAllGuns(gunList);

    static std::unordered_map<std::string, uint64_t> roundsPerCartridge;
    gunTracker->getRoundsShotPerCartridge(roundsPerCartridge);

    int roundsInLifetime;
    for(const auto& gunElm : gunList)
        roundsInLifetime += gunElm.getRoundCount();


    // HUD View measurements
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(tableSize.x < 400){
        tableSize = ImVec2{400, 200};
    }

    ImVec2{ImGui::GetContentRegionAvail().x-2, 200};

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    WindowHelper::centerText("Armory");

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent(20);
    ImGui::Text("Guns Tracked:          %ld", gunList.size()); 
    ImGui::Text("Lifetime Round Count:  %d", roundsInLifetime);
    ImGui::Unindent(20);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


    WindowHelper::centerText ( "Gun Tracker" );
    ImGui::Spacing();

    if(ImGui::BeginChild("Gun Table Window", ImVec2{tableSize.x + 2, tableSize.y+2})){
        if(ImGui::BeginTable("Gun Table", 4, 
                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, tableSize
                            ))
        {
            ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Round Count",  ImGuiTableColumnFlags_None);

            ImGui::TableHeadersRow();

            for(auto itr { gunList.begin() }; itr != gunList.end(); ++itr){
                    ImGui::TableNextRow();
                    for (int column{0}; column < 4; ++column)
                    {
                        const Gun& gun {*itr};

                        ImGui::TableSetColumnIndex(column);
                        switch( column ){
                            case 0:
                                ImGui::Text("%s", std::string{gun.getWeaponType()}.c_str());
                                break;
                            case 1:
                                ImGui::Text("%s", gun.getCartridge().c_str());
                                break;
                            case 2:
                                ImGui::Text("%s", gun.getName().c_str());
                                break;
                            case 3:
                                ImGui::Text("%lu", gun.getRoundCount());
                                break;
                            default:
                                ImGui::Text("Broken table");
                                break;
                        }
                    }        
                }

            ImGui::EndTable();
        }
    }
    ImGui::EndChild(); // End table window

    ImGui::Spacing();
    ImGui::Spacing();

    WindowHelper::centerText("Armory Actions");
    ImGui::Separator();

    if(ImGui::BeginTabBar("Armory Tabs",    ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                            ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown
    )){
        if(ImGui::BeginTabItem("Add Gun")){
            drawAddGun();
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Detailed Viewer")){
            ImGui::Text("Tab Stuff");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

}
void ShooterCentralWindow::drawAddGun() const {
    if(ImGui::BeginChild("Add Gun")){
        static constexpr int MAX_LIST_NUM { 10 };
        static char nameBuf[32] = "Enter Name";

        // Get char array of cartrigde names
        static StringVector cartridgeNamesBuf;
        char* cartridgeNames[MAX_LIST_NUM];
        static int selectedCartridgeIndex { 0 };
        ammoTracker->getAllCartridgeNames(cartridgeNamesBuf);

        int catridgeNameIndex { 0 };
        for(auto& s : cartridgeNamesBuf){
            if(catridgeNameIndex < (MAX_LIST_NUM - 1)){
                cartridgeNames[catridgeNameIndex] = &s[0];
            }
            ++catridgeNameIndex;
        }


        // Get char array of weapon type names
        static StringVector weaponTypeNamesBuf;
        char* wtNames[MAX_LIST_NUM];
        static int selectedWTIndex { 0 };
        gunTracker->getAllWeaponTypeNames(weaponTypeNamesBuf);

        int wtNameIndex { 0 };
        for(auto& s : weaponTypeNamesBuf){
            if(wtNameIndex < (MAX_LIST_NUM - 1)){
                wtNames[wtNameIndex] = &s[0];
            }
            ++wtNameIndex;
        }

        const char* cartComboPreview { cartridgeNames[selectedCartridgeIndex] };
        const char* wtComboPreview { wtNames[selectedWTIndex] };

        ImGui::Text("Directions");
        ImGui::BulletText("This will add a gun to tracked items.");
        ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        ImGui::Text("Name");
        ImGui::SameLine(100);
        ImGui::InputText("##", nameBuf, 32);

        ImGui::Text("Weapon Type");
        ImGui::SameLine(100);
        if (ImGui::BeginCombo("##WT Combo", wtComboPreview, ImGuiComboFlags_HeightSmall))
        {
            for (size_t chooseWT { 0 }; chooseWT < weaponTypeNamesBuf.size(); ++chooseWT)
            {
                const bool isSelected = (selectedWTIndex == chooseWT);
                if (ImGui::Selectable(wtNames[chooseWT], isSelected))
                    selectedWTIndex = chooseWT;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Cartridge");
        ImGui::SameLine(100);
        if (ImGui::BeginCombo("##Cart Combo", cartComboPreview, ImGuiComboFlags_HeightSmall))
        {
            for (size_t chooseCartridge { 0 }; chooseCartridge < cartridgeNamesBuf.size(); ++chooseCartridge)
            {
                const bool isSelected = (selectedCartridgeIndex == chooseCartridge);
                if (ImGui::Selectable(cartridgeNames[chooseCartridge], isSelected))
                    selectedCartridgeIndex = chooseCartridge;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Spacing();
        ImGui::Spacing();

        if(WindowHelper::centerButton("Add New Gun", ImVec2{200,50})){
            std::string name { nameBuf }, wtBuf { wtNames[selectedWTIndex]}, cartBuf {cartridgeNames[selectedCartridgeIndex]};

            if(name != "Enter Name"){
                gunTracker->createGun(name, wtBuf, cartBuf);
            }
        }

    }
    ImGui::EndChild();
}

// MARK: WindowHelper
void WindowHelper::centerText(std::string text){
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text.c_str());
}
bool WindowHelper::centerButton(std::string text, ImVec2 buttonSize){
    auto windowWidth = ImGui::GetWindowSize().x;

    ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
    return ImGui::Button(text.c_str(), buttonSize);
}

