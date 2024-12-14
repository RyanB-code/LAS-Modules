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

    if(ImGui::BeginChild("Home"))
        drawHome(ImGui::GetContentRegionAvail());
    ImGui::EndChild();
    
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
        drawArmoryQuickView();
    ImGui::EndChild();

    if(horizontalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Guns Quick View", childWindowSizes, ImGuiChildFlags_Border))
        drawStockpileQuickView();
    ImGui::EndChild();

    if(horizontalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Events Quick View", childWindowSizes, ImGuiChildFlags_Border))
        drawEventsQuickView();
    ImGui::EndChild();
}
// MARK: QUICK VIEW ARMORY
void ShooterCentralWindow::drawArmoryQuickView() const{
    static std::vector<Gun>                             gunList;
    static std::unordered_map<std::string, uint64_t>    roundsPerCartridge;
    static bool unsavedChanges { false };

    gunTracker->getAllGuns(gunList);
    gunTracker->getRoundsShotPerCartridge(roundsPerCartridge);

    uint64_t roundsInLifetime { 0 };
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

    if(ImGui::BeginChild("Armory Quick Details", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::Text("Guns In Armory:       %ld", gunList.size()); 
        ImGui::Text("Lifetime Round Count: %ld", roundsInLifetime);
        ImGui::Unindent(20);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Does not divide x by 2 since the region avail (since using SameLine() ) is shortened
    if(ImGui::BeginChild("Armory Buttons", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        static bool gunsNotSaved { false }, weaponsNotSaved { false };
        if(WindowHelper::centerButton("Save All", ImVec2{200, 50})){
            if(!gunTracker->writeAllGuns())
                gunsNotSaved = true;
            if(!gunTracker->writeAllWeaponTypes())
                weaponsNotSaved = true;

            if(!gunsNotSaved || !weaponsNotSaved)
                unsavedChanges = false;
        }
        
        if(gunsNotSaved || weaponsNotSaved)
            ImGui::OpenPopup("Error Saving");

        // Popup showing error
        if(ImGui::BeginPopupModal("Error Saving", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            if(gunsNotSaved){
                WindowHelper::centerText("All guns could not be saved");
                ImGui::Spacing();
            }
            if(weaponsNotSaved)
                WindowHelper::centerText("Weapon Types could not be saved");

            ImGui::Spacing();
            ImGui::Spacing();
            
            if (WindowHelper::centerButton("Close", ImVec2{120, 0})){
                gunsNotSaved = false;
                weaponsNotSaved = false;

                // Since error saving, set flag
                unsavedChanges = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(unsavedChanges)
            WindowHelper::centerText("(There are unsaved changes)");
    }
    ImGui::EndChild();

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
        if(ImGui::BeginTabItem("Detailed Gun Viewer")){
            ImGui::Text("Tab Stuff");
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Gun")){
            drawAddGun(unsavedChanges);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Weapon Type")){
            drawAddWeaponType(unsavedChanges);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

}
// MARK: ADD GUN
void ShooterCentralWindow::drawAddGun(bool& unsavedChanges) const {
    if(!ImGui::BeginChild("Add Gun")){
        ImGui::EndChild();
        return;
    }
    static char nameBuf[MAX_TEXT_INPUT_CHARS] = "Enter Name";

    // Get char array of cartrigde names
    static StringVector cartridgeNamesBuf;
    ammoTracker->getAllCartridgeNames(cartridgeNamesBuf);

    // Create ImGui array for cartridge names
    const char* cartrdgeList[MAX_LIST_NUM];
    cartrdgeList[0] = "CHOOSE CARTRIDGE";
    int i1 { 1 };
    for(auto& s : cartridgeNamesBuf){
        if(i1 < (MAX_LIST_NUM + 1)){
            cartrdgeList[i1] = s.c_str();
        }
        ++i1;
    }

    // Get char array of weapon type names
    static StringVector weaponTypeNamesBuf;
    gunTracker->getAllWeaponTypeNames(weaponTypeNamesBuf);

    const char* weaponTypeList[MAX_LIST_NUM];
    weaponTypeList[0] = "CHOOSE WEAPON TYPE";

    int i2 { 1 };
    for(auto& s : weaponTypeNamesBuf){
        if(i2 < (MAX_LIST_NUM + 1)){
            weaponTypeList[i2] = s.c_str();
        }
        ++i2;
    }

    static size_t selectedWTIndex { 0 };
    static size_t selectedCartridgeIndex { 0 };

    const char* cartComboPreview    { cartrdgeList      [selectedCartridgeIndex] };
    const char* wtComboPreview      { weaponTypeList    [selectedWTIndex] };

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new gun to tracked items.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    ImGui::BulletText("To add a new Weapon Type, see the \"Add New Weapon Type\" tab.");
    ImGui::BulletText("To add a new cartridge, navigate to the Stockpile window.");

    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Name");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Gun Name", nameBuf, MAX_TEXT_INPUT_CHARS);

    ImGui::Text("Weapon Type");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##WeaponType Combo", wtComboPreview, ImGuiComboFlags_HeightSmall))
    {
        for (size_t chooseWT { 0 }; chooseWT < weaponTypeNamesBuf.size()+1; ++chooseWT) // Add one to account for default option
        {
            const bool isSelected = (selectedWTIndex == chooseWT);
            if (ImGui::Selectable(weaponTypeList[chooseWT], isSelected))
                selectedWTIndex = chooseWT;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Cartridge");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Cartridge Combo", cartComboPreview, ImGuiComboFlags_HeightSmall))
    {
        for (size_t chooseCartridge { 0 }; chooseCartridge < cartridgeNamesBuf.size()+1; ++chooseCartridge) // Add one to account for default option
        {
            const bool isSelected = (selectedCartridgeIndex == chooseCartridge);
            if (ImGui::Selectable(cartrdgeList[chooseCartridge], isSelected))
                selectedCartridgeIndex = chooseCartridge;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    static bool invalidName { false }, invalidWeaponType { false }, invalidCartridge { false };
    if(WindowHelper::centerButton("Add New Gun", ImVec2{200,50})){
        // Ensure flags are valid
        invalidName         = false;
        invalidWeaponType   = false;
        invalidCartridge    = false;

        // Ensure it is not the default option
        std::string name { nameBuf };
        if(selectedWTIndex <= 0)
            invalidWeaponType = true;
        if(selectedCartridgeIndex <= 0)
            invalidCartridge = true;
        if(name == "Enter Name")
            invalidName = true;

        if(!invalidName && !invalidWeaponType &&! invalidWeaponType){
            std::string wtBuf { weaponTypeList[selectedWTIndex]}, cartBuf {cartrdgeList[selectedCartridgeIndex]};

            gunTracker->createGun(name, wtBuf, cartBuf);

            // Reset buffers
            strcpy(nameBuf, "Enter Name");
            selectedCartridgeIndex = 0;
            selectedWTIndex = 0;

            unsavedChanges = true; // Set flag
        }
        else
            ImGui::OpenPopup("Gun Not Created");
    }

    if(ImGui::BeginPopupModal("Gun Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        WindowHelper::centerText("This gun could not be created.");

        if(invalidName)
            ImGui::BulletText("Invalid name");
        if(invalidWeaponType)
            ImGui::BulletText("Invalid weapon type");
        if(invalidCartridge)
            ImGui::BulletText("Invalid cartridge");

        ImGui::Spacing();
        ImGui::Spacing();

        if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}
// MARK: ADD WT
void ShooterCentralWindow::drawAddWeaponType(bool& unsavedChanges) const{
    static char wtBuf[MAX_TEXT_INPUT_CHARS] = "New Weapon Type";

    if(!ImGui::BeginChild("Add Gun")){
        ImGui::EndChild();
        return;
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Weapon Type to choose from when creating a gun.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Weapon Type");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Weapon Type", wtBuf, MAX_TEXT_INPUT_CHARS, ImGuiInputTextFlags_CharsUppercase);

    ImGui::Spacing();
    ImGui::Spacing();

    if(WindowHelper::centerButton("Add New Weapon Type", ImVec2{200,50})){
        bool nameRejected { false };
        std::string wt { wtBuf };

        for(auto& c : wt){
            if(!isalnum(c) && c != ' '){
                nameRejected = true;
                break;
            }
            c = toupper(c);
        }

        if(wt != "New Weapon Type" && !nameRejected){
            gunTracker->addWeaponType(wt);
            strcpy(wtBuf, "New Weapon Type");   // Reset buffers
            unsavedChanges = true;              // Set flag
        }
        else
            ImGui::OpenPopup("Weapon Type Not Created");
    }

    if(ImGui::BeginPopupModal("Weapon Type Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        WindowHelper::centerText("The Weapon Type could not be created.");
        WindowHelper::centerText("Invalid Weapon Type name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}
// MARK: QUICK VIEW STOCKPILE
void ShooterCentralWindow::drawStockpileQuickView() const{
    static bool unsavedChanges  { false };
    static bool detailedView    { true };
    static StringVector                 ammoNames;
    static StringVector                 cartridgeNames;
    static std::vector<TrackedAmmo>     ammo;
    static std::vector<TrackedAmmo>     selectedCatridgeAmmoList;

    static std::vector<std::pair<std::string, uint64_t>> countByCartridge;

    ammoTracker->getAllAmmoNames            (ammoNames);
    ammoTracker->getAllCartridgeNames       (cartridgeNames);
    ammoTracker->getAmmoCountByCartridge    (countByCartridge);
    ammoTracker->getAllAmmo                 (ammo);

    static int  highlilghtedItem        { 0 };
    static bool showAllAmmo             { false };
    static int  columnsInDetailedTable  { 5 };

    const char* cartridgeList[MAX_LIST_NUM];
    cartridgeList[0] = "ALL";       // Default option

    int i { 1 };                                    // Starts at one to offset the all option
    for(const auto& s : cartridgeNames){
        if(i < (MAX_LIST_NUM+1)){      // Add one to offset the additional 'all' option
            cartridgeList[i] = s.c_str();
            ++i;
        }
    }

    // HUD View measurements
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(showAllAmmo){
        if(tableSize.x < 400)
            tableSize = ImVec2{400, 200};
    }
    else{
        if(tableSize.x < 500)
            tableSize = ImVec2{500, 200};
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    WindowHelper::centerText("Stockpile");

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginChild("Stockpile Options", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::Checkbox("Show Detailed View", &detailedView);
        ImGui::Unindent(20);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Does not divide x by 2 since the region avail (since using SameLine() ) is shortened
    if(ImGui::BeginChild("Stockpile Buttons", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        static bool ammoNotSaved { false }, cartridgesNotSaved { false }, manufacturersNotSaved { false };

        if(WindowHelper::centerButton("Save All", ImVec2{200, 50})){
            // Ensure flags are valid
            ammoNotSaved            = false;
            cartridgesNotSaved      = false;
            manufacturersNotSaved   = false;


            if(!ammoTracker->writeAllAmmo())
                ammoNotSaved = true;
            if(!ammoTracker->writeAllCartridges())
                cartridgesNotSaved = true;
            if(!ammoTracker->writeAllManufacturers())
                manufacturersNotSaved = true;

            // Reset unsaved changes flag if all were successful
            if(!ammoNotSaved && !cartridgesNotSaved && !manufacturersNotSaved)
                unsavedChanges = false;
        }
        
        if(ammoNotSaved || cartridgesNotSaved || manufacturersNotSaved)
            ImGui::OpenPopup("Stockpile Saving Error");

        // Popup showing error
        if(ImGui::BeginPopupModal("Stockpile Saving Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            WindowHelper::centerText("There was an error attempting to save");

            if(ammoNotSaved)
                ImGui::BulletText("All ammo could not be saved");
            if(cartridgesNotSaved)
                ImGui::BulletText("Cartridges could not be saved");
            if(manufacturersNotSaved)
                ImGui::BulletText("Manufacturers could not be saved");

            ImGui::Spacing();
            ImGui::Spacing();
            
            if (WindowHelper::centerButton("Close", ImVec2{120, 0})){
                // Since error saving, set flag
                unsavedChanges = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(unsavedChanges)
            WindowHelper::centerText("(There are unsaved changes)");
    }
    ImGui::EndChild();

    ImGui::Text("Cartridge Shown");
    ImGui::SameLine (150);
    ImGui::Combo("##Choose Cartridge", &highlilghtedItem, cartridgeList, cartridgeNames.size()+1); // This is the selector table. Add one to account for 'all' option

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

    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginChild("Ammo Table Window", ImVec2{tableSize.x + 2, tableSize.y+2})){
        if(detailedView){
            if(ImGui::BeginTable("Detailed Cartridge Breakdown", columnsInDetailedTable, 
                                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                                    ImGuiTableFlags_HighlightHoveredColumn,
                                                    tableSize
                                ))
            {
                if(showAllAmmo)
                    ImGui::TableSetupColumn("Cartridge",    0);
                ImGui::TableSetupColumn("Name",         0);
                ImGui::TableSetupColumn("Manufacturer", 0);
                ImGui::TableSetupColumn("Grain Weight", 0);
                ImGui::TableSetupColumn("Amount",       0);
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
        }
        else{
            // Not detailed view
            if(cartridgeNames.empty())
                ImGui::Text("There are no tracked cartridges.");
            else{
                if(ImGui::BeginTable("Stockpile", 2, 
                                                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                                        ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                                        ImGuiTableFlags_HighlightHoveredColumn,
                                                        tableSize
                                                    ))
                {
                    ImGui::TableSetupColumn("Cartridge",    0);
                    ImGui::TableSetupColumn("Amount",       0);
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
    }
    ImGui::EndChild(); // End table vhild window

    ImGui::Spacing();
    ImGui::Spacing();

    WindowHelper::centerText("Stockpile Actions");
    ImGui::Separator();

    if(ImGui::BeginTabBar("Stockpile Tabs",    ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                            ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown
    )){
        if(ImGui::BeginTabItem("Add to Existing Ammo")){
            drawAddToExistingAmmoType(unsavedChanges);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Ammo Type")){
            drawAddNewAmmoType(unsavedChanges);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Cartridge")){
            drawAddNewCartridge(unsavedChanges);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Manufacturer")){
            drawAddNewManufacturer(unsavedChanges);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
// MARK: ADD NEW CARTRIDGE
void ShooterCentralWindow::drawAddNewCartridge (bool& unsavedChanges) const{
    static char cartridgeBuf[MAX_TEXT_INPUT_CHARS] = "New Cartridge";

    if(!ImGui::BeginChild("Add New Cartridge")){
        ImGui::EndChild();
        return;
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new cartridge to choose from when creating a new Ammo Type.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Cartridge");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Cartridge", cartridgeBuf, MAX_TEXT_INPUT_CHARS, 0);

    ImGui::Spacing();
    ImGui::Spacing();

    if(WindowHelper::centerButton("Add New Cartridge", ImVec2{200,50})){
        std::string cartridgeName { cartridgeBuf };
        bool nameRejected { false };
        for(const auto& c : cartridgeName){
            if(!isalnum(c) && c != ' ' && c != '.'){
                nameRejected = true;
                break;
            }
        }

        if(cartridgeName != "New Cartridge" && !nameRejected){
            ammoTracker->addCartridge(cartridgeBuf);
            strcpy(cartridgeBuf, "New Cartridge");   // Reset buffers
            unsavedChanges = true;              // Set flag
        }
        else
            ImGui::OpenPopup("Cartridge Not Created");
    }

    if(ImGui::BeginPopupModal("Cartridge Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        WindowHelper::centerText("The cartridge could not be created.");
        WindowHelper::centerText("Invalid cartridge name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}
// MARK: ADD NEW AMMO TYPE
void ShooterCentralWindow::drawAddNewAmmoType (bool& unsavedChanges) const{
    if(!ImGui::BeginChild("Add Ammo Type")){
        ImGui::EndChild();
        return;
    }
    static char nameBuf[MAX_TEXT_INPUT_CHARS] = "Enter Name";
    static int grainWeightBuf{0};

    // Get char array of cartrigde names
    static StringVector cartridgeNamesBuf;
    static StringVector manufacturerNamesBuf;

    ammoTracker->getAllCartridgeNames(cartridgeNamesBuf);
    ammoTracker->getAllManufacturerNames(manufacturerNamesBuf);


    // Create ImGui array for cartridge names
    const char* cartrdgeList[MAX_LIST_NUM];
    cartrdgeList[0] = "CHOOSE CARTRIDGE";
    int i1 { 1 };
    for(auto& s : cartridgeNamesBuf){
        if(i1 < (MAX_LIST_NUM + 1)){
            cartrdgeList[i1] = s.c_str();
        }
        ++i1;
    }

    // Create ImGui array for manufacturer names
    const char* manufacturerList[MAX_LIST_NUM];
    manufacturerList[0] = "CHOOSE MANUFACTURER";
    int i2 { 1 };
    for(auto& s : manufacturerNamesBuf){
        if(i2 < (MAX_LIST_NUM + 1)){
            manufacturerList[i2] = s.c_str();
        }
        ++i2;
    }

    static size_t selectedManufacturerIndex { 0 };
    static size_t selectedCartridgeIndex { 0 };

    const char* cartComboPreview    { cartrdgeList      [selectedCartridgeIndex] };
    const char* manComboPreview     { manufacturerList  [selectedManufacturerIndex] };

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new ammo type to tracked items.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    ImGui::BulletText("To add a new manufacturer, see the \"Add New Manufacturer\" tab.");
    ImGui::BulletText("To add a new cartridge, see the \"Add New Cartridge\" tab.");

    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Name");
    ImGui::SameLine(100);
    ImGui::InputText("##Input Ammo Type Name", nameBuf, MAX_TEXT_INPUT_CHARS);

    ImGui::Text("Manufacturer");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Choose Manufacturer Combo", manComboPreview, ImGuiComboFlags_HeightSmall))
    {
        for (size_t chooseManufacturer { 0 }; chooseManufacturer < manufacturerNamesBuf.size()+1; ++chooseManufacturer) // Add one to account for default option
        {
            const bool isSelected = (selectedManufacturerIndex == chooseManufacturer);
            if (ImGui::Selectable(manufacturerList[chooseManufacturer], isSelected))
                selectedManufacturerIndex = chooseManufacturer;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Cartridge");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Choose Cartridge Combo", cartComboPreview, ImGuiComboFlags_HeightSmall))
    {
        for (size_t chooseCartridge { 0 }; chooseCartridge < cartridgeNamesBuf.size()+1; ++chooseCartridge) // Add one to account for default option
        {
            const bool isSelected = (selectedCartridgeIndex == chooseCartridge);
            if (ImGui::Selectable(cartrdgeList[chooseCartridge], isSelected))
                selectedCartridgeIndex = chooseCartridge;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Grain Weight");
    ImGui::SameLine(100);
    ImGui::InputInt("##Input Grain Weight", &grainWeightBuf, 1, 0, 0);

    ImGui::Spacing();
    ImGui::Spacing();

    static bool invalidName { false }, invalidManufacturer { false }, invalidCartridge { false }, invalidGrainWeight { false };
    if(WindowHelper::centerButton("Add New Ammo Type", ImVec2{200,50})){
        // Ensure buffers are valid
         invalidName            = false;
        invalidManufacturer     = false;
        invalidCartridge        = false;
        invalidGrainWeight      = false;
        // Verify data        
        if(selectedManufacturerIndex <= 0) 
            invalidManufacturer = true;
        
        if(selectedCartridgeIndex <= 0) 
            invalidCartridge = true;

        if(grainWeightBuf <= 0 || grainWeightBuf > 255) 
            invalidGrainWeight = true;

        std::string name { nameBuf };
        if(name == "Enter Name")
            invalidName = true;
        
        if(!invalidName && !invalidManufacturer && !invalidCartridge && !invalidGrainWeight){
            std::string manBuf { manufacturerList[selectedManufacturerIndex]}, cartBuf {cartrdgeList[selectedCartridgeIndex]};

            ammoTracker->addAmmoToStockpile(0, AmmoType{ name, manBuf, cartBuf, uint8_t{grainWeightBuf}});

            // Reset buffers
            strcpy(nameBuf, "Enter Name");

            selectedCartridgeIndex      = 0;
            selectedManufacturerIndex   = 0;

            unsavedChanges = true; // Set flag

        }
        if(invalidName || invalidManufacturer || invalidCartridge || invalidGrainWeight)
            ImGui::OpenPopup("Ammo Type Not Created");
    }

    if(ImGui::BeginPopupModal("Ammo Type Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        WindowHelper::centerText("This ammo type could not be created.");

        if(invalidName)
            ImGui::BulletText("Invalid name");
        if(invalidManufacturer)
            ImGui::BulletText("Invalid manufacturer");
        if(invalidCartridge)
            ImGui::BulletText("Invalid cartridge");
        if(invalidGrainWeight)
            ImGui::BulletText("Invalid grain weight");

        ImGui::Spacing();
        ImGui::Spacing();

        if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}
// MARK: ADD MANUFACTURER
void ShooterCentralWindow::drawAddNewManufacturer (bool& unsavedChanges) const{
    static char manBuf[MAX_TEXT_INPUT_CHARS] = "New Manufacturer";

    if(!ImGui::BeginChild("Add New Manufacturer")){
        ImGui::EndChild();
        return;
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new manufacturer to choose from when creating a new Ammo Type.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Manufacturer");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Manufacturer", manBuf, MAX_TEXT_INPUT_CHARS, 0);

    ImGui::Spacing();
    ImGui::Spacing();

    if(WindowHelper::centerButton("Add New Manufacturer", ImVec2{200,50})){
        std::string manName { manBuf };
        bool nameRejected { false };
        for(const auto& c : manName){
            if(!isalnum(c) && c != ' '){
                nameRejected = true;
                break;
            }
        }

        if(manName != "New Manufacturer" && !nameRejected){
            ammoTracker->addManufacturer(manName);
            strcpy(manBuf, "New Manufacturer");   // Reset buffers
            unsavedChanges = true;              // Set flag
        }
        else
            ImGui::OpenPopup("Manufacturer Not Created");
    }

    if(ImGui::BeginPopupModal("Manufacturer Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        WindowHelper::centerText("The manufacturer could not be created.");
        WindowHelper::centerText("Invalid manufacturer name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}
// MARK: ADD TO AMMO TYPE
void ShooterCentralWindow::drawAddToExistingAmmoType  (bool& unsavedChanges) const{
    if(!ImGui::BeginChild("Add To Existing Ammo Type")){
        ImGui::EndChild();
        return;
    }

    static std::vector<TrackedAmmo> ammoListBuf;
    ammoTracker->getAllAmmo(ammoListBuf);

    const char* ammoList[MAX_LIST_NUM];
    ammoList[0] = "CHOOSE AMMO TYPE";
    int i { 1 };
    for(auto& trackedAmmo : ammoListBuf){
        if(i < (MAX_LIST_NUM + 1)){
            ammoList[i] = trackedAmmo.ammoType.name.c_str();
        }
        ++i;
    }

    static int amountToAdd              { 0 };
    static size_t selectedAmmoNameIndex    { 0 };
    const char* ammoNameComboPreview    { ammoList[selectedAmmoNameIndex] };

    // Calculate center button add now (since cannot do inside child since middle will be skewed)
    float fullWindowWidth { ImGui::GetWindowSize().x };
    
    static TrackedAmmo emptyAmmo {};
    TrackedAmmo& selectedAmmo { emptyAmmo };

    if(selectedAmmoNameIndex > 0)
        selectedAmmo = ammoListBuf.at(selectedAmmoNameIndex -1 );
    else
        selectedAmmo = emptyAmmo;

    ImGui::Text("Directions");
    ImGui::BulletText("This will add the amount to an existing Ammo Type in the stockpile.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginChild("Enter Info", ImVec2{ImGui::GetContentRegionAvail().x-300, ImGui::GetContentRegionAvail().y})){
        ImGui::Text("Ammo Name");
        ImGui::SameLine(100);
        if (ImGui::BeginCombo("##Choose Ammo Name Combo", ammoNameComboPreview, ImGuiComboFlags_HeightSmall)){
            for (size_t chooseName { 0 }; chooseName < ammoListBuf.size()+1; ++chooseName){ // Add one to account for default option
                const bool isSelected = (selectedAmmoNameIndex == chooseName);
                if (ImGui::Selectable(ammoList[chooseName], isSelected))
                    selectedAmmoNameIndex = chooseName;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::Text("Amount");
        ImGui::SameLine(100);
        ImGui::InputInt("##Input New Amount", &amountToAdd, 1, 50);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::SetCursorPosX((fullWindowWidth - 200) * 0.5f); // 200 is Add button width
        
        static bool amountRejected { false }, ammoRejected { false };
        if(ImGui::Button("Add", ImVec2{200,50})){
            // Ensure buffers are valid before manipulating
            amountRejected = false;
            ammoRejected = false;

            if(amountToAdd <= 0)
                amountRejected = true;

            if(selectedAmmoNameIndex <= 0)
                ammoRejected = true;

            if(!ammoRejected && !amountRejected){
                ammoTracker->addAmmoToStockpile(amountToAdd, selectedAmmo.ammoType);
                selectedAmmoNameIndex = 0;
                amountToAdd = 0;
                unsavedChanges = true;              // Set flag
            }
            else
                ImGui::OpenPopup("Amount Not Added");
        }

        if(ImGui::BeginPopupModal("Amount Not Added", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            WindowHelper::centerText("The amount could not be added.");

            if(amountRejected)
                ImGui::BulletText("Invalid amount");
            if(ammoRejected)
                ImGui::BulletText("Invalid Ammo Type");

            ImGui::Spacing();
            ImGui::Spacing();
            
            if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();


    if(ImGui::BeginChild("Selected Ammo Detailed Information", ImGui::GetContentRegionAvail())){
        WindowHelper::centerText("Detailed Ammo Information");
        ImGui::Separator();
        if(selectedAmmoNameIndex > 0){
            ImGui::Text("Name: ");
            ImGui::SameLine(150);
            ImGui::Text("%s", selectedAmmo.ammoType.name.c_str());

            ImGui::Text("Manufacturer: ");
            ImGui::SameLine(150);
            ImGui::Text("%s", selectedAmmo.ammoType.manufacturer.c_str());

            ImGui::Text("Cartridge: ");
            ImGui::SameLine(150);
            ImGui::Text("%s", selectedAmmo.ammoType.cartridge.c_str());

            ImGui::Text("Grain Weight: ");
            ImGui::SameLine(150);
            ImGui::Text("%d", int{selectedAmmo.ammoType.grainWeight});

            ImGui::Text("Amount On Hand: ");
            ImGui::SameLine(150);
            ImGui::Text("%ld", selectedAmmo.amount);
        }
        else
            WindowHelper::centerText("Select Ammo Type");
    }
    ImGui::EndChild();

    ImGui::EndChild();

}

// MARK: QUICK VIEW EVENTS
void ShooterCentralWindow::drawEventsQuickView() const {
    static bool unsavedChanges { false };
    static std::vector<Event> eventList;
    std::vector<Event>::iterator itr;

    eventTracker->getAllEvents(eventList);

    // HUD View measurements
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(tableSize.x < 400)
        tableSize = ImVec2{400, 200};

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    WindowHelper::centerText("Events");

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginChild("Events Details", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::Text("Total Events:      %ld", eventTracker->getTotalEvents());
        ImGui::Unindent(20);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Does not divide x by 2 since the region avail (since using SameLine() ) is shortened
    if(ImGui::BeginChild("Event Tracker Buttons", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        static bool eventsNotSaved { false }, eventTypesNotSaved { false }, locationsNotSaved { false };

        if(WindowHelper::centerButton("Save All", ImVec2{200, 50})){

            // Ensure flags are valid
            eventsNotSaved = false;
            eventTypesNotSaved = false;
            locationsNotSaved = false;

            if(!eventTracker->writeAllEvents())
                eventsNotSaved = true;
            if(!eventTracker->writeAllEventTypes())
                eventTypesNotSaved = true;
            if(!eventTracker->writeAllLocations())
                locationsNotSaved = true;

            // Reset unsaved cahnges flag
            if(!eventsNotSaved && !eventTypesNotSaved && !locationsNotSaved)
                unsavedChanges = false;
        }
        
        if(eventsNotSaved || eventTypesNotSaved || locationsNotSaved)
            ImGui::OpenPopup("Event Tracker Saving Error");

        // Popup showing error
        if(ImGui::BeginPopupModal("Event Tracker Saving Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            WindowHelper::centerText("There was an error attempting to save");

            if(eventsNotSaved)
                ImGui::BulletText("All events could not be saved");
            if(eventTypesNotSaved)
                ImGui::BulletText("Event Types could not be saved");
            if(locationsNotSaved)
                ImGui::BulletText("Locations could not be saved");

            ImGui::Spacing();
            ImGui::Spacing();
            
            if (WindowHelper::centerButton("Close", ImVec2{120, 0})){
                // Since error saving, set flag
                unsavedChanges = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(unsavedChanges)
            WindowHelper::centerText("(There are unsaved changes)");
    }
    ImGui::EndChild();


    if(ImGui::BeginChild("Event Table Window", ImVec2{tableSize.x + 2, tableSize.y+2})){
        if(ImGui::BeginTable("Event Table", 5, 
                                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                        ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                        ImGuiTableFlags_HighlightHoveredColumn,
                                        tableSize
                                ))
        {
            ImGui::TableSetupColumn("Date",         ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Location",     ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Event Type",   ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Guns Used",    ImGuiTableColumnFlags_None);

            ImGui::TableHeadersRow();

            for(auto itr { eventList.begin() }; itr != eventList.end(); ++itr){
                    ImGui::TableNextRow();
                    for (int column{0}; column < 4; ++column)
                    {
                        const Event& event {*itr};

                        ImGui::TableSetColumnIndex(column);
                        switch( column ){
                            case 0:
                                ImGui::Text("%s", event.printDate().c_str());
                                break;
                            case 1:
                                ImGui::Text("%s", event.getName().c_str());
                                break;
                            case 2:
                                ImGui::Text("%s", event.getLocation().c_str());
                                break;
                            case 3:
                                ImGui::Text("%s", event.getEventType().c_str());
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
    ImGui::EndChild(); // End table window

    ImGui::Spacing();
    ImGui::Spacing();

    WindowHelper::centerText("Event Tracker Actions");
    ImGui::Separator();

    if(ImGui::BeginTabBar("Event Tabs",    ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                            ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown
    )){
        if(ImGui::BeginTabItem("View/Edit Event")){
            drawEventActions(unsavedChanges);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Event")){
            drawAddEvent(unsavedChanges);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Event Type")){
            drawAddEventType(unsavedChanges);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Location")){
            drawAddLocation(unsavedChanges);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
// MARK: VIEW, EDIT, REMOVE EVENT
void ShooterCentralWindow::drawEventActions(bool& unsavedChanges) const{
    if(!ImGui::BeginChild("Event Actions")){
        ImGui::EndChild();
        return;
    }
    static bool showAmmoUsed { false };
    static std::vector<Event> eventsVector;
    eventTracker->getAllEvents(eventsVector);

    // Create array for event names
    const char* eventNamesArray[MAX_LIST_NUM];
    eventNamesArray[0] = "Select An Event";
    int i { 1 };
    for(auto& e : eventsVector){
        if(i < (MAX_LIST_NUM + 1)){
            eventNamesArray[i] =  e.getName().c_str();
        }
        ++i;
    }

    static size_t   selectedEventIndex  { 0 };
    const char*     eventNamePreview    { eventNamesArray[selectedEventIndex] };

    static Event  emptyEvent    { };
    static Event& selectedEvent { emptyEvent };

    if(selectedEventIndex > 0)
        selectedEvent = eventsVector.at(selectedEventIndex - 1 );
    else
        selectedEvent = emptyEvent;

    if(ImGui::BeginChild("Event Directions", ImVec2{ImGui::GetContentRegionAvail().x/2, 50})){
        ImGui::Text("Directions");
        ImGui::BulletText("Select an Event to perform any action");
        ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // Does not divide x by 2 since the region avail (since using SameLine() ) is shortened
    if(ImGui::BeginChild("Event Options Tab", ImVec2{ImGui::GetContentRegionAvail().x, 50})){
        WindowHelper::centerText("Show Ammo Used");
        ImGui::SameLine(0, 20);
        ImGui::Checkbox("##Show Ammo Used", &showAmmoUsed);
    }
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginChild("Select Event for Actions", ImVec2{ImGui::GetContentRegionAvail().x, 35})){
        ImGui::Text("Event");
        ImGui::SameLine(100);
        if (ImGui::BeginCombo("##Event Name Combo Choice", eventNamePreview, ImGuiComboFlags_HeightSmall)){
            for (size_t chooseEvent { 0 }; chooseEvent < eventsVector.size()+1; ++chooseEvent){ // Add one to account for default option
                const bool isSelected = (selectedEventIndex == chooseEvent);
                if (ImGui::Selectable(eventNamesArray[chooseEvent], isSelected))
                    selectedEventIndex = chooseEvent;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    ImGui::EndChild();

    if(selectedEventIndex > 0){
        float widthOfDetailedInfo { ImGui::GetContentRegionAvail().x - (ImGui::GetContentRegionAvail().x - 300) };

        if(ImGui::BeginChild("Selected Event Detailed Information", ImVec2{widthOfDetailedInfo, 140})){
            WindowHelper::centerText("Event Information");
            ImGui::Separator();
            if(selectedEventIndex > 0){
                ImGui::Text("Name: ");
                ImGui::SameLine(150);
                ImGui::Text("%s", selectedEvent.getName().c_str());

                ImGui::Text("Location: ");
                ImGui::SameLine(150);
                ImGui::Text("%s", selectedEvent.getLocation().c_str());

                ImGui::Text("Event Type: ");
                ImGui::SameLine(150);
                ImGui::Text("%s", selectedEvent.getEventType().c_str());

                ImGui::Text("Notes: ");
                ImGui::SameLine(150);
                ImGui::Text("%s", selectedEvent.getNotes().c_str());
            }
            else
                WindowHelper::centerText("Select Event");
        }
        ImGui::EndChild();

        // SAmeline for edit, remove buttons


        // ADD HERE

        if(ImGui::BeginChild("Selected Event Detailed Gun Information", ImVec2{ ImGui::GetContentRegionAvail().x, 140})){
                static std::vector<std::pair<Gun, TrackedAmmo>> eventGunsUsed;
                selectedEvent.getAllGunsUsed(eventGunsUsed);
                drawEventGunTable(eventGunsUsed, showAmmoUsed);
        }
        ImGui::EndChild();
    }

    ImGui::EndChild();  // Ends tab child window
}
// MARK: EVENT GUN TABLE
void ShooterCentralWindow::drawEventGunTable(std::vector<std::pair<Gun, TrackedAmmo>>& list, bool showAmmoUsed) const{
    WindowHelper::centerText("Guns Used");
    ImGui::Separator();

    std::vector<std::pair<Gun, TrackedAmmo>>::iterator itr { list.begin() };

    static const Gun emptyGun   { };
    static int tableColumns     { 3 };
    static float boxSise        { 100 };

    if(showAmmoUsed){
        tableColumns = 7;
        boxSise = 50;
    }
    else{
        tableColumns = 3;
        boxSise = 50;
    }

     if(ImGui::BeginTable("Gun Table", tableColumns, 
                                ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, ImVec2(ImGui::GetContentRegionAvail().x, boxSise)
                        ))
    {
        ImGui::TableSetupColumn("Weapon Type");
        ImGui::TableSetupColumn("Cartridge");
        ImGui::TableSetupColumn("Name");

        if(showAmmoUsed){
            ImGui::TableSetupColumn("Ammo Used");
            ImGui::TableSetupColumn("Manufacturer");
            ImGui::TableSetupColumn("Grain Weight");
            ImGui::TableSetupColumn("Rounds Shot");
        }

        ImGui::TableHeadersRow();

        for(auto itr { list.begin() }; itr != list.end(); ++itr){
                const Gun& gun          {itr->first};
                const TrackedAmmo& ammo { itr->second};

                if(gun != emptyGun){
                    ImGui::TableNextRow();
                    for (int column{0}; column < tableColumns; ++column)
                    {
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
                                ImGui::Text("%s", ammo.ammoType.name.c_str());
                                break;
                            case 4:
                                ImGui::Text("%s", ammo.ammoType.manufacturer.c_str());
                                break;
                            case 5:
                                ImGui::Text("%d", int{ammo.ammoType.grainWeight});
                                break;
                            case 6:
                                ImGui::Text("%lu", ammo.amount);
                                break;
                            default:
                                ImGui::Text("Broken table");
                                break;
                        }
                    }
                }        
            }

        ImGui::EndTable();
    }
}


// MARK: ADD EVENT
void ShooterCentralWindow::drawAddEvent(bool& unsavedChanges) const{
    if(!ImGui::BeginChild("Add Event")){
        ImGui::EndChild();
        return;
    }
    static char nameBuf[MAX_TEXT_INPUT_CHARS]   = "Enter Event Name";
    static char notesBuf[MAX_TEXT_INPUT_CHARS];
    static int  dayBuf  { 0 };
    static int  monthBuf{ 0 };
    static int  yearBuf { 2024 };

    // Get char array of cartrigde names
    static StringVector evenTypesVector {};
    static StringVector locationsVector {};

    eventTracker->getAllEventTypes(evenTypesVector);
    eventTracker->getAllLocations(locationsVector);


    // Create array for event types
    const char* eventTypesArray[MAX_LIST_NUM];
    eventTypesArray[0] = "CHOOSE EVENT TYPE";
    int i1 { 1 };
    for(auto& s : evenTypesVector){
        if(i1 < (MAX_LIST_NUM + 1)){
            eventTypesArray[i1] = s.c_str();
        }
        ++i1;
    }

    // Create array for locaitons
    const char* locationsArray[MAX_LIST_NUM];
    locationsArray[0] = "CHOOSE LOCATION";
    int i2 { 1 };
    for(auto& s : locationsVector){
        if(i2 < (MAX_LIST_NUM + 1)){
            locationsArray[i2] = s.c_str();
        }
        ++i2;
    }

    static size_t selectedLocationIndex     { 0 };
    static size_t selectedEventTypeIndex    { 0 };

    const char* eventTypePreview    { eventTypesArray   [selectedEventTypeIndex] };
    const char* locationPreview     { locationsArray    [selectedLocationIndex] };

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Event to tracked events.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    ImGui::BulletText("To add a new Event Type, see the \"Add New Event Type\" tab.");
    ImGui::BulletText("To add a new location, see the \"Add New Location\" tab.");

    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Name");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Event Name", nameBuf, MAX_TEXT_INPUT_CHARS);

    ImGui::Text("Location");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Choose Location Combo", locationPreview, ImGuiComboFlags_HeightSmall))
    {
        for (size_t chooseLocation { 0 }; chooseLocation < locationsVector.size()+1; ++chooseLocation) // Add one to account for default option
        {
            const bool isSelected = (selectedLocationIndex == chooseLocation);
            if (ImGui::Selectable(locationsArray[chooseLocation], isSelected))
                selectedLocationIndex = chooseLocation;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Event Type");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Choose Event Type Combo", eventTypePreview, ImGuiComboFlags_HeightSmall))
    {
        for (size_t chooseEventType { 0 }; chooseEventType < evenTypesVector.size()+1; ++chooseEventType) // Add one to account for default option
        {
            const bool isSelected = (selectedEventTypeIndex == chooseEventType);
            if (ImGui::Selectable(eventTypesArray[chooseEventType], isSelected))
                selectedEventTypeIndex = chooseEventType;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Notes");
    ImGui::SameLine(100);
    ImGui::InputText("##Input Notes", notesBuf, MAX_TEXT_INPUT_CHARS);
    ImGui::SameLine();
    ImGui::TextDisabled("(May be left blank)");

    ImGui::Text("Date");
    ImGui::SameLine(100);
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Input Day", &dayBuf);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Input Month", &monthBuf);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Input Year", &yearBuf);

    // Grayed out details, approximate centers of text box did the trick
    ImGui::SetCursorPosX(125);
    ImGui::TextDisabled("(Day)");
    ImGui::SameLine(0, 70);
    ImGui::TextDisabled("(Month)");
    ImGui::SameLine(0, 70);
    ImGui::TextDisabled("(Year)");

    // ADD GUN HERE

    
    ImGui::Spacing();
    ImGui::Spacing();

    static bool invalidName { false }, invalidLocation { false }, invalidEventType { false }, invalidDate{ false };
    if(WindowHelper::centerButton("Add Event", ImVec2{200,50})){
        // Reset flags 
        invalidName         = false;
        invalidLocation     = false;
        invalidEventType    = false;
        invalidDate         = false;

        // Verify buffers
        using namespace std::chrono;
        std::string nameStr { nameBuf }, notesStr { notesBuf };
        year_month_day dateBuf { year{yearBuf}, month{monthBuf}, day{dayBuf}};

        if(selectedLocationIndex <= 0) 
            invalidLocation = true;
        if(selectedEventTypeIndex <= 0) 
            invalidEventType = true;
        if(nameStr == "Enter Event Name")
            invalidName = true;
        if(!dateBuf.ok())
            invalidDate = true;

        
        if(!invalidName && !invalidLocation && !invalidEventType && !invalidDate){
            std::string eventTypeStr { eventTypesArray[selectedEventTypeIndex]}, locationStr {locationsArray[selectedLocationIndex]};

            Event eventBuf { nameStr, locationStr, eventTypeStr, notesStr, dateBuf};
            eventTracker->addEvent(eventBuf);

            // Reset buffers
            strcpy(nameBuf,     "Enter Event Name");
            strcpy(notesBuf,    "");

            selectedLocationIndex   = 0;
            selectedEventTypeIndex  = 0;
            dayBuf      = 0;
            monthBuf    = 0;
            yearBuf     = 2024;

            unsavedChanges = true; // Set flag

        }
        if(invalidName || invalidLocation || invalidEventType || invalidDate)
            ImGui::OpenPopup("Event Not Created");
    }

    if(ImGui::BeginPopupModal("Event Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        WindowHelper::centerText("This Event could not be created.");

        if(invalidName)
            ImGui::BulletText("Invalid name");
        if(invalidLocation)
            ImGui::BulletText("Invalid location");
        if(invalidEventType)
            ImGui::BulletText("Invalid Event Type");
        if(invalidDate)
            ImGui::BulletText("Invalid date");

        ImGui::Spacing();
        ImGui::Spacing();

        if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}

// MARK: ADD Event Type
void ShooterCentralWindow::drawAddEventType (bool& unsavedChanges) const{
    static char eventTypeBuf[MAX_TEXT_INPUT_CHARS] = "New Event Type";

    if(!ImGui::BeginChild("Add New Event Type")){
        ImGui::EndChild();
        return;
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Event Type to choose from when creating an Event.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Event Type");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Event Type", eventTypeBuf, MAX_TEXT_INPUT_CHARS, ImGuiInputTextFlags_CharsUppercase);

    ImGui::Spacing();
    ImGui::Spacing();

    if(WindowHelper::centerButton("Add Event Type", ImVec2{200,50})){
        bool nameRejected { false };
        std::string eventTypeStrBuf { eventTypeBuf };

        for(auto& c : eventTypeStrBuf){
            if(!isalnum(c) && c != ' '){
                nameRejected = true;
                break;
            }
            c = toupper(c);
        }

        if(eventTypeStrBuf != "New Event Type" && !nameRejected){
            eventTracker->addEventType(eventTypeStrBuf);
            strcpy(eventTypeBuf, "New Event Type");     // Reset buffers
            unsavedChanges = true;                      // Set flag
        }   
        else
            ImGui::OpenPopup("Event Type Not Created");
    }

    if(ImGui::BeginPopupModal("Event Type Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        WindowHelper::centerText("The Event Type could not be created.");
        WindowHelper::centerText("Invalid name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}
// MARK: ADD LOCATION
void ShooterCentralWindow::drawAddLocation (bool& unsavedChanges) const{
    static char locationBuf[MAX_TEXT_INPUT_CHARS] = "New Location";

    if(!ImGui::BeginChild("Add New Location")){
        ImGui::EndChild();
        return;
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new location to choose from when creating an Event.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Location");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Location", locationBuf, MAX_TEXT_INPUT_CHARS, 0);

    ImGui::Spacing();
    ImGui::Spacing();

    if(WindowHelper::centerButton("Add Location", ImVec2{200,50})){
        bool nameRejected { false };
        std::string locationStrBuf { locationBuf };
        for(const auto& c : locationStrBuf){
            if(!isalnum(c) && c != ' '){
                nameRejected = true;
                break;
            }
        }

        if(locationStrBuf != "New Location" && !nameRejected){
            eventTracker->addLocation(locationStrBuf);
            strcpy(locationBuf, "New Location");     // Reset buffers
            unsavedChanges = true;                   // Set flag
        }   
        else
            ImGui::OpenPopup("Location Not Created");
    }

    if(ImGui::BeginPopupModal("Location Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        WindowHelper::centerText("The location could not be created.");
        WindowHelper::centerText("Invalid name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
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

