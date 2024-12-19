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
        ImGui::TextDisabled("This module has not been setup yet...");
        ImGui::End();
        return;
    }

    drawHome(ImGui::GetContentRegionAvail());
    
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
void ShooterCentralWindow::drawHome(ImVec2 windowSize){
    ImVec2  childWindowSizes { };
    bool    horizontalLayout { false };

    // Horizontal bigger than vertical, dispplay tabs side by side
    // 3 = number of child windows
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

    if(ImGui::BeginMenuBar()){
        if(ImGui::BeginMenu("Options")){
            ImGui::MenuItem("Show Armory",      NULL, &showArmory   );
            ImGui::MenuItem("Show Stockpile",   NULL, &showStockpile);
            ImGui::MenuItem("Show Events",      NULL, &showEvents   );
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    static std::vector<ConstGunPtr>                     gunList;
    static std::unordered_map<std::string, uint64_t>    roundsPerCartridge;

    gunTracker->getAllGuns(gunList);
    gunTracker->getRoundsShotPerCartridge(roundsPerCartridge);

    if(ImGui::BeginChild("Armory Quick View", childWindowSizes, ImGuiChildFlags_Border) && showArmory){
        drawArmoryUI(gunList, roundsPerCartridge);
    }
    ImGui::EndChild();

    if(horizontalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Stockpile Quick View", childWindowSizes, ImGuiChildFlags_Border) && showStockpile)
        drawStockpile();
    ImGui::EndChild();

    if(horizontalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Events Quick View", childWindowSizes, ImGuiChildFlags_Border) && showEvents)
        drawEvents();
    ImGui::EndChild();
}
// MARK: ARMORY
void ShooterCentralWindow::drawArmoryUI(const std::vector<ConstGunPtr>& gunList, const std::unordered_map<std::string, uint64_t>& roundsPerCartridge){
    
    static bool savingGunsFailed { false }, savingWeaponTypesFailed { false }; // Flags to tell user what went wrong when attempting to save

    uint64_t roundsInLifetime { 0 };
    for(const auto& gunElm : gunList)
        roundsInLifetime += gunElm->getRoundCount();

    ImVec2 tableSize { ImGui::GetContentRegionAvail().x, 200};
    if(tableSize.x < 400){
        tableSize = ImVec2{400, 200};
    }

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

    // Armory Button Window
    if(ImGui::BeginChild("Armory Buttons", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){

        if(WindowHelper::centerButton("Save All", ImVec2{200, 50})){
            if(!gunTracker->writeAllGuns())
                savingGunsFailed = true;

            if(!gunTracker->writeAllWeaponTypes())
                savingWeaponTypesFailed = true;

            
            if(!savingGunsFailed || !savingWeaponTypesFailed)
                unsavedChanges_Armory = false;  // Saving was successfu, reset unsavedChanges flag
        }
        
        if(savingGunsFailed || savingWeaponTypesFailed)
            ImGui::OpenPopup("Error Saving");


        // Popup showing error messages
        if(ImGui::BeginPopupModal("Error Saving", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            if(savingGunsFailed){
                WindowHelper::centerText("All guns could not be saved");
                ImGui::Spacing();
            }
            if(savingWeaponTypesFailed)
                WindowHelper::centerText("Weapon Types could not be saved");

            ImGui::Spacing();
            ImGui::Spacing();
            
            if (WindowHelper::centerButton("Close", ImVec2{120, 0})){
                savingGunsFailed = false;
                savingWeaponTypesFailed = false;

                // Since error saving, set flag
                unsavedChanges_Armory = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(unsavedChanges_Armory)
            WindowHelper::centerTextDisabled("(There are unsaved changes)");
    }
    ImGui::EndChild();


    ImGui::SeparatorText( "Guns" );
    ImGui::Spacing();

    if(ImGui::BeginTable("Gun Table", 4, ImGuiTableFlags_Borders  | ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, tableSize)) {
        ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Round Count",  ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(auto itr { gunList.begin() }; itr != gunList.end(); ++itr){
            const Gun& gun {**itr};

            ImGui::TableNextRow();
            for (int column{0}; column < 4; ++column){
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Text("%s", gun.getWeaponType().c_str());
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
                        ImGui::TextDisabled("Broken table");
                        break;
                }
            }        
        }
        ImGui::EndTable();
    }
   
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText("Armory Actions");

    if(ImGui::BeginTabBar("Armory Tabs",    ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                            ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown)) {
        if(ImGui::BeginTabItem("Detailed Gun Viewer")){
            ImGui::Text("Tab Stuff");
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Gun")){
            drawAddGun(unsavedChanges_Armory);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Weapon Type")){
            drawAddWeaponType(unsavedChanges_Armory);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

}

// LEFT OFF HERE, DO THE REST OF FUNCTIONS BELOW




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
// MARK: STOCKPILE
void ShooterCentralWindow::drawStockpile() const{
    static bool unsavedChanges  { false };
    static bool detailedView    { true };
    static StringVector                 ammoNames;
    static StringVector                 cartridgeNames;
    static std::vector<TrackedAmmoPtr>  ammo;
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
                        const TrackedAmmo& trackedAmmo { **itr };
                        ImGui::TableNextRow();
                        for (int column{0}; column < columnsInDetailedTable; ++column)
                        {
                            ImGui::TableSetColumnIndex(column);
                            switch( column ){
                                case 0:
                                    ImGui::Text("%s", trackedAmmo.ammoType.cartridge.c_str());
                                    break;
                                case 1:
                                    ImGui::Text("%s", trackedAmmo.ammoType.name.c_str());
                                    break;
                                case 2:
                                    ImGui::Text("%s", trackedAmmo.ammoType.manufacturer.c_str());
                                    break;
                                case 3:
                                    ImGui::Text("%d", int{trackedAmmo.ammoType.grainWeight});
                                    break;
                                case 4:
                                    ImGui::Text("%lu", trackedAmmo.amount);
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

    static std::vector<TrackedAmmoPtr> ammoListBuf;
    ammoTracker->getAllAmmo(ammoListBuf);

    const char* ammoList[MAX_LIST_NUM];
    ammoList[0] = "CHOOSE AMMO TYPE";
    int i { 1 };
    for(auto& trackedAmmo : ammoListBuf){
        if(i < (MAX_LIST_NUM + 1)){
            ammoList[i] = trackedAmmo->ammoType.name.c_str();
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
        selectedAmmo = *ammoListBuf.at(selectedAmmoNameIndex -1 );
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

// MARK: EVENTS
void ShooterCentralWindow::drawEvents() const {
    static bool unsavedChanges { false };
    static std::vector<EventPtr> eventList;
    std::vector<EventPtr>::iterator itr;

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

    static EventPtr selectedEvent { nullptr }; 

    int row { 0 };
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
                const Event& event {**itr};
                bool isEventSelected { false };

                if(selectedEvent){
                    if(event == *selectedEvent){
                        isEventSelected = true;
                    }
                }

                ImGui::TableNextRow();

                std::string imGuiID { event.printDate()};
                imGuiID += "##";
                imGuiID += std::to_string(row);

                for (int column{0}; column < 4; ++column)
                {                   
                    ImGui::TableSetColumnIndex(column);
                    switch( column ){
                        case 0:
                            ImGui::Selectable(imGuiID.c_str(), &isEventSelected, ImGuiSelectableFlags_SpanAllColumns);

                            if(isEventSelected)
                                selectedEvent = *itr;

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
                ++row;        
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
        if(ImGui::BeginTabItem("View Event")){
            drawViewEvent(selectedEvent);
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
// MARK: VIEW EVENT
void ShooterCentralWindow::drawViewEvent(EventPtr selectedEvent) const{
    if(!ImGui::BeginChild("View Event")){
        ImGui::EndChild();
        return;
    }
    static bool showAmmoUsed { false };

    if(!selectedEvent){
        ImGui::Spacing();
        ImGui::Spacing();

        WindowHelper::centerTextDisabled("Select an Event");
        ImGui::EndChild();
        return;
    }

    // Detailed Event Information
    if(ImGui::BeginChild("Selected Event Detailed Information", ImVec2{ImGui::GetContentRegionAvail().x/2, 100})){
        WindowHelper::centerText("Event Information");
        ImGui::Separator();
        if(selectedEvent){
            ImGui::Text("Name: ");
            ImGui::SameLine(150);
            ImGui::Text("%s", selectedEvent->getName().c_str());

            ImGui::Text("Location: ");
            ImGui::SameLine(150);
            ImGui::Text("%s", selectedEvent->getLocation().c_str());

            ImGui::Text("Event Type: ");
            ImGui::SameLine(150);
            ImGui::Text("%s", selectedEvent->getEventType().c_str());
        }
        else
            WindowHelper::centerText("(Select Event)");
    }
    ImGui::EndChild();
    ImGui::SameLine();


    if(ImGui::BeginChild("Event Options", ImVec2{ImGui::GetContentRegionAvail().x, 50})){
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        WindowHelper::centerText("Show Ammo Used");
        ImGui::SameLine();
        ImGui::Checkbox("##Show Ammo Used Checkbox", &showAmmoUsed);

    }
    ImGui::EndChild();

    ImGui::SeparatorText("Notes");
    ImGui::Spacing();
    ImGui::Spacing();
    if(selectedEvent)
        ImGui::Text("%s", selectedEvent->getNotes().c_str());
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();

    // Table To Show Event Information
    if(ImGui::BeginChild("Selected Event Detailed Gun Information", ImVec2{ ImGui::GetContentRegionAvail().x, 140})){
        if(selectedEvent){
            static std::vector<std::pair<Gun, TrackedAmmo>> eventGunsUsed;
            selectedEvent->getAllGunsUsed(eventGunsUsed);
            drawEventGunTable(eventGunsUsed, showAmmoUsed);
        }
    }
    ImGui::EndChild();


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
    static char notesBuf[MAX_TEXT_INPUT_CHARS_NOTES];

    static StringVector                 eventTypesVector {};
    static StringVector                 locationsVector {};
    static std::vector<ConstGunPtr>     gunList;
    static std::vector<TrackedAmmoPtr>  ammoList;

    static int numGuns { 1 };

    static int              dayBuf          { 0 },          monthBuf        { 0 },          yearBuf         { 2024 };
    static Gun              selectedGun1    {  },           selectedGun2    {  },           selectedGun3    {  };
    static TrackedAmmo      selectedTA1     {  },           selectedTA2     {  },           selectedTA3     {  };

    static bool invalidName             { false }, invalidLocation  { false },  invalidEventType    { false },  invalidDate{ false },   
                incompatibleCartridge   { false }, invalidGunOrAmmo { false },  invalidAmmoAmount   { false };
    static bool subtractRoundsUsedFromStockpile { false };


    eventTracker->getAllEventTypes(eventTypesVector);
    eventTracker->getAllLocations(locationsVector);
    gunTracker->getAllGuns(gunList);
    ammoTracker->getAllAmmo(ammoList);


    // Create array for event types
    const char* eventTypesArray[MAX_LIST_NUM];
    eventTypesArray[0] = "CHOOSE EVENT TYPE";
    int i1 { 1 };
    for(auto& s : eventTypesVector){
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

    if(ImGui::BeginChild("Add Event Directions", ImVec2{ImGui::GetContentRegionAvail().x/2, 120})){
        ImGui::Text("Directions");
        ImGui::BulletText("This will add a new Event to tracked events.");
        ImGui::BulletText("Select a Gun and Ammo from the table to add to the event");
        ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
        ImGui::BulletText("To add a new Event Type, see the \"Add New Event Type\" tab.");
        ImGui::BulletText("To add a new location, see the \"Add New Location\" tab.");
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Add Event Options", ImVec2{ImGui::GetContentRegionAvail().x, 120})){
        ImGui::Spacing();
        ImGui::Spacing();

        if(WindowHelper::centerButton("Add Event", ImVec2{200,50})){
            // Reset flags 
            invalidName             = false;
            invalidLocation         = false;
            invalidEventType        = false;
            invalidDate             = false;
            incompatibleCartridge   = false;
            invalidGunOrAmmo        = false;
            invalidAmmoAmount       = false;

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

            
            // Check cartridges of selected ammo and respective gun
            switch (numGuns){
                case 3:
                    if(selectedGun3 != Gun { } || selectedTA3.ammoType == AmmoType { })
                        invalidGunOrAmmo = true;
                    else{
                        if(selectedGun3.getCartridge() != selectedTA3.ammoType.cartridge)
                            incompatibleCartridge = true;
                        if(selectedTA3.amount == 0)
                            invalidAmmoAmount = true;
                    }
                case 2:
                    if(selectedGun2 != Gun { }|| selectedTA2.ammoType == AmmoType { })
                        invalidGunOrAmmo = true;
                    else{
                        if(selectedGun2.getCartridge() != selectedTA2.ammoType.cartridge)
                            incompatibleCartridge = true;
                        if(selectedTA2.amount == 0)
                            invalidAmmoAmount = true;
                    }
                case 1:
                    if(selectedGun1 != Gun { } || selectedTA1.ammoType == AmmoType { })
                        invalidGunOrAmmo = true;
                    else{
                        if(selectedGun1.getCartridge() != selectedTA1.ammoType.cartridge)
                            incompatibleCartridge = true;
                        if(selectedTA1.amount == 0)
                            invalidAmmoAmount = true;
                    }
                    break;
                default:
                    break;
            }

            // If all information is good, create event
            if(!invalidName && !invalidLocation && !invalidEventType && !invalidDate && !incompatibleCartridge && !invalidGunOrAmmo && !invalidAmmoAmount){
                std::string eventTypeStr { eventTypesArray[selectedEventTypeIndex]}, locationStr {locationsArray[selectedLocationIndex]};

                Event eventBuf { nameStr, locationStr, eventTypeStr, notesStr, dateBuf};
                switch (numGuns){
                    case 3:
                        eventBuf.addGun(selectedGun3, selectedTA3);

                        if(subtractRoundsUsedFromStockpile)
                            ammoTracker->removeAmmoFromStockpile(selectedTA3.amount, selectedTA3.ammoType);
                    case 2:
                        eventBuf.addGun(selectedGun2, selectedTA2);

                        if(subtractRoundsUsedFromStockpile)
                            ammoTracker->removeAmmoFromStockpile(selectedTA2.amount, selectedTA2.ammoType);
                    case 1:
                        eventBuf.addGun(selectedGun1, selectedTA1);

                        if(subtractRoundsUsedFromStockpile)
                            ammoTracker->removeAmmoFromStockpile(selectedTA1.amount, selectedTA1.ammoType);
                        break;
                    default:
                        break;
                }
                eventTracker->addEvent(eventBuf);

                // Subtract rounds from stockpile
                if(subtractRoundsUsedFromStockpile){

                }

                // Reset buffers
                strcpy(nameBuf,     "Enter Event Name");
                strcpy(notesBuf,    "");

                selectedLocationIndex   = 0;
                selectedEventTypeIndex  = 0;
                dayBuf      = 0;
                monthBuf    = 0;
                yearBuf     = 2024;

                selectedGun1 = Gun { };
                selectedGun2 = Gun { };
                selectedGun3 = Gun { };
                selectedTA1 = TrackedAmmo { };
                selectedTA2 = TrackedAmmo { };
                selectedTA3 = TrackedAmmo { };

                unsavedChanges = true; // Set flag

            }

            // Make popup modal
            if(invalidName || invalidLocation || invalidEventType || invalidDate || incompatibleCartridge || invalidAmmoAmount || invalidGunOrAmmo)
                ImGui::OpenPopup("Event Not Created");
        }
        // End Add Event Button

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
            if(incompatibleCartridge)
                ImGui::BulletText("Incompatible cartridge for a gun");
            if(invalidGunOrAmmo)
                ImGui::BulletText("Invalid gun or ammo selection");
             if(invalidAmmoAmount)
                ImGui::BulletText("Ammo used amount cannot be 0");

            ImGui::Spacing();
            ImGui::Spacing();

            if (WindowHelper::centerButton("Close", ImVec2{120, 0}))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        ImGui::Spacing();
        ImGui::Spacing();

        // Center the text and checkbox
        static constexpr std::string_view checkboxText { "Subtract Ammo From Stockpile" };
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (ImGui::CalcTextSize(std::string{checkboxText}.c_str()).x + 20)) * 0.5f); // Add 10 for checkbox size
        ImGui::Text("%s", std::string{checkboxText}.c_str());
        ImGui::SameLine();
        ImGui::Checkbox("##SubtractRoundsUsed", &subtractRoundsUsedFromStockpile);
        WindowHelper::centerTextDisabled("(Applied regardless of the event being saved)");

    }
    ImGui::EndChild();

    if(ImGui::CollapsingHeader("Event Information")){
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
            for (size_t chooseEventType { 0 }; chooseEventType < eventTypesVector.size()+1; ++chooseEventType) // Add one to account for default option
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
        ImGui::InputTextMultiline("##Input Notes", notesBuf, MAX_TEXT_INPUT_CHARS_NOTES, ImVec2{ImGui::GetContentRegionAvail().x, 100});
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
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::CollapsingHeader("Select Guns")){
        ImGui::Text("Number of Guns");
        ImGui::SameLine();
        ImGui::InputInt("(Maximum 3)", &numGuns);

        if(numGuns <= 0)
            numGuns = 1;
        if(numGuns > 3)
            numGuns = 3;

        switch(numGuns){
            case 3:
                ImGui::PushID("Select Gun 3");
                WindowHelper::drawSelectGunTable(gunList, selectedGun3);
                ImGui::PopID();
            case 2:
                ImGui::PushID("Select Gun 2");
                WindowHelper::drawSelectGunTable(gunList, selectedGun2);
                ImGui::PopID();
            case 1:
                ImGui::PushID("Select Gun 1");
                WindowHelper::drawSelectGunTable(gunList, selectedGun1);
                ImGui::PopID();                
                break;
            default:
                break;
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::CollapsingHeader("Select Ammo Used")){
       switch(numGuns){
            case 3:
                if(selectedGun3 != Gun { }){
                    ImGui::PushID("Select Ammo 3");
                    ImGui::Separator();
                    WindowHelper::centerText("Select Ammo for \"" + selectedGun3.getName() + "\"");
                    WindowHelper::drawSelectAmmoTable(ammoList, selectedTA3);
                    ImGui::PopID();
                }
            case 2:
                if(selectedGun2 != Gun { }){
                    ImGui::PushID("Select Ammo 2");
                    ImGui::Separator();
                    WindowHelper::centerText("Select Ammo for \"" + selectedGun2.getName() + "\"");
                    WindowHelper::drawSelectAmmoTable(ammoList, selectedTA2);
                    ImGui::PopID();
                }
            case 1:
                if(selectedGun1 != Gun { }){
                    ImGui::PushID("Select Ammo 1");
                    ImGui::Separator();
                    WindowHelper::centerText("Select Ammo for \"" + selectedGun1.getName() + "\"");
                    WindowHelper::drawSelectAmmoTable(ammoList, selectedTA1);
                    ImGui::PopID();
                }
                else
                    ImGui::TextDisabled("Must select a gun first");
                break;
            default:
                break;
        }
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
void WindowHelper::centerTextDisabled(std::string text){
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::TextDisabled("%s", text.c_str());
}
bool WindowHelper::centerButton(std::string text, ImVec2 buttonSize){
    auto windowWidth = ImGui::GetWindowSize().x;

    ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
    return ImGui::Button(text.c_str(), buttonSize);
}
void WindowHelper::drawSelectGunTable(const std::vector<ConstGunPtr>& gunList, Gun& selectedGun){

    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-4, 200};
    if(tableSize.x < 400){
        tableSize = ImVec2{400, 200};
    }

    int row { 0 };

    if(ImGui::BeginChild("Gun Table Window", ImVec2{tableSize.x + 2, tableSize.y+2})){
        if(ImGui::BeginTable("Gun Table", 3, 
                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, tableSize
                            ))
        {
            ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);

            ImGui::TableHeadersRow();            

            for(auto itr { gunList.begin() }; itr != gunList.end(); ++itr){
                const Gun& gun {**itr};           
                bool isGunSelected { false };
                
                if(gun == selectedGun)
                    isGunSelected = true;
                

                ImGui::TableNextRow();
                for (int column{0}; column < 3; ++column)
                {
                    ImGui::TableSetColumnIndex(column);

                    std::string imGuiID { gun.getWeaponType()};
                    imGuiID += "##";
                    imGuiID += std::to_string(row);

                    switch( column ){
                        case 0:
                            ImGui::Selectable(imGuiID.c_str(), &isGunSelected, ImGuiSelectableFlags_SpanAllColumns);

                            if(isGunSelected)
                                selectedGun = **itr;

                            break;
                        case 1:
                            ImGui::Text("%s", gun.getCartridge().c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", gun.getName().c_str());
                            break;
                        default:
                            ImGui::Text("Broken table");
                            break;
                    }
                }
                ++row;
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild(); // End table window
    ImGui::Spacing();
}

void WindowHelper::drawSelectAmmoTable(const std::vector<TrackedAmmoPtr>& ammoList, TrackedAmmo& selectedAmmo){
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-4, 200};
    if(tableSize.x < 400){
        tableSize = ImVec2{400, 200};
    }

    int row { 0 };
    if(ImGui::BeginChild("Ammo Table Window", ImVec2{tableSize.x + 2, tableSize.y+2})){

        // Select Ammo Table
        if(ImGui::BeginTable("Ammo Table", 4, 
                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, tableSize
                            ))
        {
            ImGui::TableSetupColumn("Cartridge",    0);
            ImGui::TableSetupColumn("Name",         0);
            ImGui::TableSetupColumn("Manufacturer", 0);
            ImGui::TableSetupColumn("Grain Weight", 0);
            ImGui::TableHeadersRow();            

             for(auto itr { ammoList.begin() }; itr != ammoList.end(); ++itr){
                const AmmoType& ammoType {itr->get()->ammoType};           
                bool isAmmoSelected { false };
                
                if(ammoType == selectedAmmo.ammoType)
                    isAmmoSelected = true;

                ImGui::TableNextRow();
                for (int column{0}; column < 4; ++column)
                {
                    ImGui::TableSetColumnIndex(column);

                    std::string imGuiID { ammoType.cartridge};
                    imGuiID += "##";
                    imGuiID += std::to_string(row);

                    switch( column ){
                        case 0:
                            ImGui::Selectable(imGuiID.c_str(), &isAmmoSelected, ImGuiSelectableFlags_SpanAllColumns);

                            if(isAmmoSelected)
                                selectedAmmo.ammoType = ammoType;

                            break;
                        case 1:
                            ImGui::Text("%s", ammoType.name.c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", ammoType.manufacturer.c_str());
                            break;
                        case 3:
                            ImGui::Text("%d", int{ammoType.grainWeight});
                            break;
                        default:
                            ImGui::Text("Broken table");
                            break;
                    }
                }
                ++row;
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild(); // End table window
    ImGui::Spacing();

    // Select amount of ammo used
    ImGui::Text("Amount Used");
    ImGui::SameLine();
    ImGui::InputInt("##Amount of Ammo Used", &selectedAmmo.amount);

    ImGui::Spacing();
    ImGui::Spacing();
}

