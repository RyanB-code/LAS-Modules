#include "Window.h"

using namespace ShooterCentral;


ShooterCentralWindow::ShooterCentralWindow() : LAS::Windowing::Window{"Shooter Central"} 
{

}
ShooterCentralWindow::~ShooterCentralWindow(){

}
// MARK: Draw
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

    ImVec2  windowSize { ImGui::GetContentRegionAvail() };
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

    if(ImGui::BeginChild("Armory Quick View", childWindowSizes, ImGuiChildFlags_Border) && showArmory){
        static CartridgeList cartridges;

        ammoTracker->getAllCartridges(cartridges);

        ArmoryUI::home(gunTracker, unsavedChanges.armory, cartridges);
    }
    ImGui::EndChild();

    if(horizontalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Stockpile Quick View", childWindowSizes, ImGuiChildFlags_Border) && showStockpile){
        StockpileUI::home(ammoTracker, unsavedChanges.stockpile);
    }
    ImGui::EndChild();

    if(horizontalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Events Quick View", childWindowSizes, ImGuiChildFlags_Border) && showEvents){
        EventsUI::home(eventTracker, ammoTracker, gunTracker, unsavedChanges.events, unsavedChanges.stockpile);
    }
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




// MARK: UI Helper
void UIHelper::centerText(std::string text){
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text.c_str());
}
void UIHelper::centerTextDisabled(std::string text){
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::TextDisabled("%s", text.c_str());
}
bool UIHelper::centerButton(std::string text, ImVec2 buttonSize){
    auto windowWidth = ImGui::GetWindowSize().x;

    ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
    return ImGui::Button(text.c_str(), buttonSize);
}




// MARK: ARMORY UI
void ArmoryUI::home(GunTrackerPtr gunTracker, bool& unsavedChanges, const CartridgeList& cartridges){

    static WeaponTypeList                               wpnTypes;
    static std::vector<ConstGunPtr>                     gunList;
    static std::unordered_map<std::string, uint64_t>    roundsPerCartridge;

    static bool savingGunsFailed { false }, savingWeaponTypesFailed { false }; // Flags to tell user what went wrong when attempting to save

    gunTracker->getAllGuns(gunList);
    gunTracker->getRoundsShotPerCartridge(roundsPerCartridge);
    gunTracker->getAllWeaponTypeNames(wpnTypes);
    
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

    UIHelper::centerText("Armory");

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

        if(UIHelper::centerButton("Save Armory", ImVec2{200, 50})){
            if(!gunTracker->writeAllGuns())
                savingGunsFailed = true;

            if(!gunTracker->writeAllWeaponTypes())
                savingWeaponTypesFailed = true;

            
            if(!savingGunsFailed || !savingWeaponTypesFailed)
                unsavedChanges = false;  // Saving was successfu, reset unsavedChanges flag
        }
        
        if(savingGunsFailed || savingWeaponTypesFailed)
            ImGui::OpenPopup("Error Saving");


        // Popup showing error messages
        if(ImGui::BeginPopupModal("Error Saving", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            if(savingGunsFailed){
                UIHelper::centerText("All guns could not be saved");
                ImGui::Spacing();
            }
            if(savingWeaponTypesFailed)
                UIHelper::centerText("Weapon Types could not be saved");

            ImGui::Spacing();
            ImGui::Spacing();
            
            if (UIHelper::centerButton("Close", ImVec2{120, 0})){
                savingGunsFailed = false;
                savingWeaponTypesFailed = false;

                // Since error saving, set flag
                unsavedChanges = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(unsavedChanges)
            UIHelper::centerTextDisabled("(There are unsaved changes)");
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
                        ImGui::Text("%s", gun.getWeaponType().getName().c_str());
                        break;
                    case 1:
                        ImGui::Text("%s", gun.getCartridge().getName().c_str());
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
            GunPtr newGun { addGun(unsavedChanges, cartridges, wpnTypes)};

            if(newGun)
                gunTracker->addGun(newGun);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Weapon Type")){
            WeaponType newWT {addWeaponType(unsavedChanges)};

            if(newWT != WeaponType{ })
                gunTracker->addWeaponType(newWT);

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
// MARK: Add Gun
GunPtr ArmoryUI::addGun(bool& unsavedChanges, const CartridgeList& cartridges, const WeaponTypeList& wpnTypes) {
    if(!ImGui::BeginChild("Add Gun")){
        ImGui::EndChild();
        return nullptr;
    }

    static const Cartridge  EMPTY_CARTRIDGE     { };
    static const WeaponType EMPTY_WT            { };

    static char     nameBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "Enter Name";
    
    static Cartridge   selectedCartridge   { };
    static WeaponType  selectedWT          { };

    static bool     invalidName { false }, invalidWeaponType { false }, invalidCartridge { false }; // Flags to let user know bad options when saving

    GunPtr returnVal { nullptr };

    std::string cartComboPreview { };
    std::string wtComboPreview   { };


    // Directions text
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new gun to tracked items.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    ImGui::BulletText("To add a new Weapon Type, see the \"Add New Weapon Type\" tab.");
    ImGui::BulletText("To add a new cartridge, navigate to the Stockpile window.");

    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Name");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Gun Name", nameBuf, UI_SETTINGS.MAX_TEXT_INPUT_CHARS);


    // Apply correct text to display in combo boxes
    if(selectedCartridge != EMPTY_CARTRIDGE)
        cartComboPreview = selectedCartridge.getName().c_str();
    else
        cartComboPreview = "CHOOSE CARTRIDGE";
    
    if(selectedWT != EMPTY_WT)
        wtComboPreview = selectedWT.getName().c_str();
    else
        wtComboPreview = "CHOOSE WEAPON TYPE";

    // Draw Combo Boxes
    ImGui::Text("Weapon Type");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##WeaponType Combo", wtComboPreview.c_str(), ImGuiComboFlags_HeightSmall)) {
        for (const auto& wt : wpnTypes) {
            const bool isSelected = (selectedWT == wt);
            if (ImGui::Selectable(wt.getName().c_str(), isSelected))
                selectedWT = wt;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Cartridge");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Cartridge Combo", cartComboPreview.c_str(), ImGuiComboFlags_HeightSmall)) {
        for (const auto& cart : cartridges) {
            const bool isSelected = (selectedCartridge == cart);
            if (ImGui::Selectable(cart.getName().c_str(), isSelected))
                selectedCartridge = cart;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if(UIHelper::centerButton("Add New Gun", ImVec2{200,50})){
        // Ensure flags are valid
        invalidName         = false;
        invalidWeaponType   = false;
        invalidCartridge    = false;

        // Ensure it is not the default option
        std::string name { nameBuf };
        if(selectedWT == EMPTY_WT)
            invalidWeaponType = true;
        if(selectedCartridge == EMPTY_CARTRIDGE)
            invalidCartridge = true;
        if(name == "Enter Name")
            invalidName = true;

        // All checks passed, create new Gun
        if(!invalidName && !invalidWeaponType &&! invalidWeaponType){

            returnVal = std::make_shared<Gun>(name, selectedWT, selectedCartridge);
            unsavedChanges = true;

            // Reset buffers
            strcpy(nameBuf, "Enter Name");
            selectedWT = EMPTY_WT;
            selectedCartridge = EMPTY_CARTRIDGE;
        }
        else
            ImGui::OpenPopup("Gun Not Created");
    }

    // Display error popup
    if(ImGui::BeginPopupModal("Gun Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        UIHelper::centerText("This gun could not be created.");

        if(invalidName)
            ImGui::BulletText("Invalid name");
        if(invalidWeaponType)
            ImGui::BulletText("Invalid weapon type");
        if(invalidCartridge)
            ImGui::BulletText("Invalid cartridge");

        ImGui::Spacing();
        ImGui::Spacing();

        if (UIHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();

    return returnVal;
}
// MARK: Add Wpn Type
WeaponType ArmoryUI::addWeaponType(bool& unsavedChanges) {
    WeaponType returnVal { };

    static char wtBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "New Weapon Type";

    if(!ImGui::BeginChild("Add Gun")){
        ImGui::EndChild();
        return WeaponType { };
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Weapon Type to choose from when creating a gun.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Weapon Type");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Weapon Type", wtBuf, UI_SETTINGS.MAX_TEXT_INPUT_CHARS, ImGuiInputTextFlags_CharsUppercase);

    ImGui::Spacing();
    ImGui::Spacing();

    if(UIHelper::centerButton("Add New Weapon Type", ImVec2{200,50})){
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
            returnVal = WeaponType{wt};
            strcpy(wtBuf, "New Weapon Type");   // Reset buffers
            unsavedChanges = true;              // Set flag
        }
        else
            ImGui::OpenPopup("Weapon Type Not Created");
    }

    if(ImGui::BeginPopupModal("Weapon Type Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        UIHelper::centerText("The Weapon Type could not be created.");
        UIHelper::centerText("Invalid Weapon Type name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (UIHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();

    return returnVal;
}

// MARK: STOCKPILE UI
void StockpileUI::home (AmmoTrackerPtr ammoTracker, bool& unsavedChanges){
    static bool detailedView { false };

    static const Cartridge EMPTY_CARTRIDGE { };

    static StringVector                     ammoNames;
    static CartridgeList                    cartridges;
    static ManufacturerList                 manufacturers;
    static AmountPerCartridgeList           countByCartridge;

    static std::vector<ConstTrackedAmmoPtr> ammoList;
    static std::vector<TrackedAmmo>         selectedCatridgeAmmoList;

    static Cartridge selectedCartridge { };

    ammoTracker->getAllAmmoNames            (ammoNames);
    ammoTracker->getAllCartridges           (cartridges);
    ammoTracker->getAllManufacturers        (manufacturers);
    ammoTracker->getAmmoCountByCartridge    (countByCartridge);
    ammoTracker->getAllAmmo                 (ammoList);

    std::string cartridgeComboPreview { };

    if(selectedCartridge != EMPTY_CARTRIDGE)
        cartridgeComboPreview = selectedCartridge.getName().c_str();
    else
        cartridgeComboPreview = "CHOOSE CARTRIDGE";

    if(!detailedView)
        selectedCartridge = EMPTY_CARTRIDGE;

    // HUD View measurements
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(detailedView){
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

    UIHelper::centerText("Stockpile");

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginChild("Stockpile Options", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::Checkbox("Show only a single cartridge", &detailedView);
        ImGui::Unindent(20);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Does not divide x by 2 since the region avail (since using SameLine() ) is shortened
    if(ImGui::BeginChild("Stockpile Buttons", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        static bool ammoNotSaved { false }, cartridgesNotSaved { false }, manufacturersNotSaved { false };

        if(UIHelper::centerButton("Save Stockpile", ImVec2{200, 50})){
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
            UIHelper::centerText("There was an error attempting to save");

            if(ammoNotSaved)
                ImGui::BulletText("All ammo could not be saved");
            if(cartridgesNotSaved)
                ImGui::BulletText("Cartridges could not be saved");
            if(manufacturersNotSaved)
                ImGui::BulletText("Manufacturers could not be saved");

            ImGui::Spacing();
            ImGui::Spacing();
            
            if (UIHelper::centerButton("Close", ImVec2{120, 0})){
                // Since error saving, set flag
                unsavedChanges = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(unsavedChanges)
            UIHelper::centerTextDisabled("(There are unsaved changes)");
    }
    ImGui::EndChild();

    if(detailedView){
        ImGui::Text("Cartridge Shown");
        ImGui::SameLine (150);
        if (ImGui::BeginCombo("##Cartridge Shown Combo", cartridgeComboPreview.c_str(), ImGuiComboFlags_HeightSmall)) {
            for (const auto& cart : cartridges) {
                const bool isSelected = (selectedCartridge == cart);
                if (ImGui::Selectable(cart.getName().c_str(), isSelected))
                    selectedCartridge = cart;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ammoTracker->getAllAmmoByCartridge(selectedCatridgeAmmoList, selectedCartridge);  // Populates list of ammo with selected cartridge
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if(detailedView && selectedCartridge != EMPTY_CARTRIDGE){

        // Title appropriately
        std::ostringstream titleText;
        titleText << "All " << selectedCartridge.getName() << " Ammo";
        ImGui::SeparatorText(titleText.str().c_str());

        if(ImGui::BeginTable("Detailed Cartridge Breakdown", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                            ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable | ImGuiTableFlags_HighlightHoveredColumn, tableSize ))
        {
            ImGui::TableSetupColumn("Name",         0);
            ImGui::TableSetupColumn("Manufacturer", 0);
            ImGui::TableSetupColumn("Grain Weight", 0);
            ImGui::TableSetupColumn("Amount",       0);
            ImGui::TableHeadersRow();

            if (selectedCartridge != EMPTY_CARTRIDGE) {
                for(auto itr { selectedCatridgeAmmoList.begin() }; itr != selectedCatridgeAmmoList.end(); ++itr){
                    ImGui::TableNextRow();
                    for (int column{0}; column < 4; ++column) {
                        ImGui::TableSetColumnIndex(column);
                        switch( column ){
                            case 0:
                                ImGui::Text("%s", itr->ammoType.name.c_str());
                                break;
                            case 1:
                                ImGui::Text("%s", itr->ammoType.manufacturer.getName().c_str());
                                break;
                            case 2:
                                ImGui::Text("%d", itr->ammoType.grainWeight);
                                break;
                            case 3:
                                ImGui::Text("%d", itr->amount);
                                break;
                            default:
                                ImGui::Text("Broken table");
                                break;
                        }
                    }        
                } // End populating selected cartridge table
            }
            ImGui::EndTable();
        }
    }
    else{
        // Not detailed view
        if(cartridges.empty())
            UIHelper::centerTextDisabled("There are no tracked cartridges.");
        else{
            ImGui::SeparatorText("Ammo by Cartridge"); // Title appropriately

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
                                ImGui::Text("%s", itr->first.getName().c_str());
                                break;
                            case 1:
                                ImGui::Text("%d", itr->second);
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


    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText("Stockpile Actions");

    if(ImGui::BeginTabBar("Stockpile Tabs",     ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                                ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown
    )){
        if(ImGui::BeginTabItem("Add to Existing Ammo")){
            
            TrackedAmmo returnVal {addToExistingAmmoType(unsavedChanges, ammoList)};

            if(returnVal != TrackedAmmo { })
                ammoTracker->addAmmoToStockpile(returnVal);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Ammo Type")){
            TrackedAmmo returnVal {addAmmoType(unsavedChanges, cartridges, manufacturers)};

            if(returnVal != TrackedAmmo { })
                ammoTracker->addAmmoToStockpile(returnVal);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Cartridge")){
            Cartridge returnVal {addCartridge(unsavedChanges)};

            if(returnVal != Cartridge { })
                ammoTracker->addCartridge(returnVal);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Manufacturer")){
            Manufacturer returnVal {addManufacturer(unsavedChanges)};

            if(returnVal != Manufacturer { })
                ammoTracker->addManufacturer(returnVal);

            ImGui::EndTabItem();

        }
        ImGui::EndTabBar();
    }

}
// MARK: Add Cartrigde
Cartridge StockpileUI::addCartridge (bool& unsavedChanges) {
    Cartridge returnVal    { };

    static char cartridgeBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "New Cartridge";

    if(!ImGui::BeginChild("Add New Cartridge")){
        ImGui::EndChild();
        return Cartridge { };
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new cartridge to choose from when creating a new Ammo Type.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Cartridge");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Cartridge", cartridgeBuf, UI_SETTINGS.MAX_TEXT_INPUT_CHARS, 0);

    ImGui::Spacing();
    ImGui::Spacing();

    if(UIHelper::centerButton("Add New Cartridge", ImVec2{200,50})){
        std::string cartridgeName { cartridgeBuf };
        bool nameRejected { false };
        for(const auto& c : cartridgeName){
            if(!isalnum(c) && c != ' ' && c != '.'){
                nameRejected = true;
                break;
            }
        }

        if(cartridgeName != "New Cartridge" && !nameRejected){
            returnVal = Cartridge {cartridgeName};
            strcpy(cartridgeBuf, "New Cartridge");   // Reset buffers
            unsavedChanges = true;              // Set flag
        }
        else
            ImGui::OpenPopup("Cartridge Not Created");
    }

    if(ImGui::BeginPopupModal("Cartridge Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        UIHelper::centerText("The cartridge could not be created.");
        UIHelper::centerText("Invalid cartridge name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (UIHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();

    return returnVal;
}
// MARK: Add Ammo Type
TrackedAmmo StockpileUI::addAmmoType (bool& unsavedChanges, const CartridgeList& cartridges, const ManufacturerList& manufacturers){
    if(!ImGui::BeginChild("Add Ammo Type")){
        ImGui::EndChild();
        return TrackedAmmo{ };
    }
    
    static const Cartridge      EMPTY_CARTRIDGE     { };
    static const Manufacturer   EMPTY_MANUFACTURER  { };

    static char         nameBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "Enter Name";
    static int          grainWeightBuf                              { 0 };
    static Cartridge    selectedCartridge                           { };
    static Manufacturer selectedManufacturer                        { };

    std::string cartridgeComboPreview       { };
    std::string manufacturerComboPreview    { };

    TrackedAmmo returnVal { };

    if(selectedCartridge != EMPTY_CARTRIDGE)
        cartridgeComboPreview = selectedCartridge.getName();
    else
        cartridgeComboPreview = "CHOOSE CARTRIDGE";
    
    if(selectedManufacturer != EMPTY_MANUFACTURER)
        manufacturerComboPreview = selectedManufacturer.getName();
    else
        manufacturerComboPreview = "CHOOSE MANUFACTURER";


    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new ammo type to tracked items.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    ImGui::BulletText("To add a new manufacturer, see the \"Add New Manufacturer\" tab.");
    ImGui::BulletText("To add a new cartridge, see the \"Add New Cartridge\" tab.");

    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Name");
    ImGui::SameLine(100);
    ImGui::InputText("##Input Ammo Type Name", nameBuf, UI_SETTINGS.MAX_TEXT_INPUT_CHARS);

    ImGui::Text("Manufacturer");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Choose Manufacturer Combo", manufacturerComboPreview.c_str(), ImGuiComboFlags_HeightSmall)) {
        for (const auto& man : manufacturers) {
            const bool isSelected = (selectedManufacturer == man);

            if (ImGui::Selectable(man.getName().c_str(), isSelected))
                selectedManufacturer = man;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }


    ImGui::Text("Cartridge");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Choose Cartridge Combo", cartridgeComboPreview.c_str(), ImGuiComboFlags_HeightSmall)) {
        for (const auto& cart : cartridges) {
            const bool isSelected = (selectedCartridge == cart);
            if (ImGui::Selectable(cart.getName().c_str(), isSelected))
                selectedCartridge = cart;

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
    if(UIHelper::centerButton("Add New Ammo Type", ImVec2{200,50})){
        // Ensure buffers are valid
        invalidName             = false;
        invalidManufacturer     = false;
        invalidCartridge        = false;
        invalidGrainWeight      = false;

        
        // Verify data        
        if(selectedManufacturer == EMPTY_MANUFACTURER) 
            invalidManufacturer = true;
        
        if(selectedCartridge == EMPTY_CARTRIDGE) 
            invalidCartridge = true;

        if(grainWeightBuf <= 0 || grainWeightBuf > 255) 
            invalidGrainWeight = true;

        std::string name { nameBuf };
        if(name == "Enter Name")
            invalidName = true;
        
        if(!invalidName && !invalidManufacturer && !invalidCartridge && !invalidGrainWeight){

            returnVal = TrackedAmmo {AmmoType{ name, selectedManufacturer, selectedCartridge, grainWeightBuf}, 0};

            // Reset buffers
            strcpy(nameBuf, "Enter Name");

            selectedManufacturer    = EMPTY_MANUFACTURER;
            selectedCartridge       = EMPTY_CARTRIDGE;
            grainWeightBuf          = 0;

            unsavedChanges = true; // Set flag

        }
        if(invalidName || invalidManufacturer || invalidCartridge || invalidGrainWeight)
            ImGui::OpenPopup("Ammo Type Not Created");
    }

    if(ImGui::BeginPopupModal("Ammo Type Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        UIHelper::centerText("This ammo type could not be created.");

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

        if (UIHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();

    return returnVal;
}
// MARK: Add Manufacturer
Manufacturer StockpileUI::addManufacturer (bool& unsavedChanges){
    static char manBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "New Manufacturer";

    Manufacturer returnVal { };

    if(!ImGui::BeginChild("Add New Manufacturer")){
        ImGui::EndChild();
        return Manufacturer { };
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new manufacturer to choose from when creating a new Ammo Type.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Manufacturer");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Manufacturer", manBuf, UI_SETTINGS.MAX_TEXT_INPUT_CHARS, 0);

    ImGui::Spacing();
    ImGui::Spacing();

    if(UIHelper::centerButton("Add New Manufacturer", ImVec2{200,50})){
        std::string manName { manBuf };
        bool nameRejected { false };
        for(const auto& c : manName){
            if(!isalnum(c) && c != ' '){
                nameRejected = true;
                break;
            }
        }

        if(manName != "New Manufacturer" && !nameRejected){
            returnVal = Manufacturer{manName};
            strcpy(manBuf, "New Manufacturer");   // Reset buffers
            unsavedChanges = true;              // Set flag
        }
        else
            ImGui::OpenPopup("Manufacturer Not Created");
    }

    if(ImGui::BeginPopupModal("Manufacturer Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        UIHelper::centerText("The manufacturer could not be created.");
        UIHelper::centerText("Invalid manufacturer name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (UIHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();

    return returnVal;
}
// MARK: Add To Ammo Type
TrackedAmmo StockpileUI::addToExistingAmmoType  (bool& unsavedChanges, const std::vector<ConstTrackedAmmoPtr>& ammoList){
    if(!ImGui::BeginChild("Add To Existing Ammo Type")){
        ImGui::EndChild();
        return TrackedAmmo { };
    }
    static const TrackedAmmo    EMPTY_AMMO  { };
    static TrackedAmmo          selectedAmmo{  };
    static int                  amountToAdd { 0 };

    bool            submitAmmo          { false }; // Flag to return the proper selected ammo instead of empty ammo
    std::string     ammoNamePreview     { };
    float           fullWindowWidth     { ImGui::GetWindowSize().x }; // Calculate center button add now (since cannot do inside child since middle will be skewed)

    if(selectedAmmo != EMPTY_AMMO)
        ammoNamePreview = selectedAmmo.ammoType.name;
    else
        ammoNamePreview = "CHOOSE AMMO TYPE";

    ImGui::Text("Directions");
    ImGui::BulletText("This will add the amount to an existing Ammo Type in the stockpile.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();


    if(ImGui::BeginChild("Enter Info", ImVec2{ImGui::GetContentRegionAvail().x-300, ImGui::GetContentRegionAvail().y})){
        ImGui::Text("Ammo Name");
        ImGui::SameLine(100);
        
        if (ImGui::BeginCombo("##Choose Ammo Name Combo", ammoNamePreview.c_str(), ImGuiComboFlags_HeightSmall)){
            for (const auto& ammo : ammoList){ // Add one to account for default option
                const bool isSelected = (selectedAmmo == *ammo);

                if (ImGui::Selectable(ammo->ammoType.name.c_str(), isSelected))
                    selectedAmmo = *ammo;

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

            if(selectedAmmo == EMPTY_AMMO)
                ammoRejected = true;

            // All checks passed
            if(!ammoRejected && !amountRejected){
                submitAmmo = true;
                unsavedChanges = true;

                selectedAmmo.amount = amountToAdd;

                // Reset flags
                amountToAdd = 0;
            }
            else
                ImGui::OpenPopup("Amount Not Added");
        }

        if(ImGui::BeginPopupModal("Amount Not Added", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            UIHelper::centerText("The amount could not be added.");

            if(amountRejected)
                ImGui::BulletText("Invalid amount");
            if(ammoRejected)
                ImGui::BulletText("Invalid Ammo Type");

            ImGui::Spacing();
            ImGui::Spacing();
            
            if (UIHelper::centerButton("Close", ImVec2{120, 0}))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    if(ImGui::BeginChild("Selected Ammo Detailed Information", ImGui::GetContentRegionAvail())){
        UIHelper::centerText("Detailed Ammo Information");
        ImGui::Separator();
        if(selectedAmmo != EMPTY_AMMO){
            ImGui::Text("Name: ");
            ImGui::SameLine(150);
            ImGui::Text("%s", selectedAmmo.ammoType.name.c_str());

            ImGui::Text("Manufacturer: ");
            ImGui::SameLine(150);
            ImGui::Text("%s", selectedAmmo.ammoType.manufacturer.getName().c_str());

            ImGui::Text("Cartridge: ");
            ImGui::SameLine(150);
            ImGui::Text("%s", selectedAmmo.ammoType.cartridge.getName().c_str());

            ImGui::Text("Grain Weight: ");
            ImGui::SameLine(150);
            ImGui::Text("%d", selectedAmmo.ammoType.grainWeight);

            ImGui::Text("Amount On Hand: ");
            ImGui::SameLine(150);
            ImGui::Text("%d", selectedAmmo.amount);
        }
        else
            UIHelper::centerTextDisabled("Select Ammo Type");
    }
    ImGui::EndChild();

    ImGui::EndChild();

    if(submitAmmo){
        TrackedAmmo ammoBuf { selectedAmmo };   // Need this buffer to reset selected Ammo before returning
        selectedAmmo = EMPTY_AMMO;
        return ammoBuf;
    }
    else
        return EMPTY_AMMO;
}

// MARK: EVENTS UI
void EventsUI::home(EventTrackerPtr eventTracker, AmmoTrackerPtr ammoTracker, std::shared_ptr<const GunTracker> gunTracker, bool& unsavedChangesEvents, bool& unsavedChangesStockpile) {
    static std::vector<EventPtr> eventList;

    static EventTypeList            eventTypes      { };
    static LocationList             locations       { };
    static ConstGunPtrList          guns            { };
    static ConstTrackedAmmoPtrList  ammo            { };
    static EventPtr                 selectedEvent   { nullptr }; 

    std::vector<EventPtr>::iterator itr;

    eventTracker->getAllEventTypes(eventTypes);
    eventTracker->getAllLocations(locations);
    gunTracker->getAllGuns(guns);
    ammoTracker->getAllAmmo(ammo);

    eventTracker->getAllEvents(eventList);

    // HUD View measurements
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(tableSize.x < 400)
        tableSize = ImVec2{400, 200};

    
    ImGui::Spacing();
    ImGui::Spacing();

    UIHelper::centerText("Event Tracker");
    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginChild("Events Details", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::Text("Total Events:      %d", eventTracker->getTotalEvents());
        ImGui::Unindent(20);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Does not divide x by 2 since the region avail (since using SameLine() ) is shortened
    if(ImGui::BeginChild("Event Tracker Buttons", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        static bool eventsNotSaved { false }, eventTypesNotSaved { false }, locationsNotSaved { false };

        if(UIHelper::centerButton("Save Events", ImVec2{200, 50})){

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
                unsavedChangesEvents = false;
        }
        
        if(eventsNotSaved || eventTypesNotSaved || locationsNotSaved)
            ImGui::OpenPopup("Event Tracker Saving Error");

        // Popup showing error
        if(ImGui::BeginPopupModal("Event Tracker Saving Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            UIHelper::centerText("There was an error attempting to save");

            if(eventsNotSaved)
                ImGui::BulletText("All events could not be saved");
            if(eventTypesNotSaved)
                ImGui::BulletText("Event Types could not be saved");
            if(locationsNotSaved)
                ImGui::BulletText("Locations could not be saved");

            ImGui::Spacing();
            ImGui::Spacing();
            
            if (UIHelper::centerButton("Close", ImVec2{120, 0})){
                // Since error saving, set flag
                unsavedChangesEvents = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(unsavedChangesEvents)
            UIHelper::centerTextDisabled("(There are unsaved changes)");
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Events");

    int row { 0 };
    if(ImGui::BeginTable("Event Table", 4, 
                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                    ImGuiTableFlags_HighlightHoveredColumn,
                                    tableSize )){
        ImGui::TableSetupColumn("Date",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Location",     ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Event Type",   ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Guns Used",    ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(auto itr { eventList.begin() }; itr != eventList.end(); ++itr){
            const Event& event {**itr};
            bool isEventSelected { false };

            if(selectedEvent){
                if(event == *selectedEvent)
                    isEventSelected = true;
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
                        ImGui::Text("%s", event.getLocation().getName().c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", event.getEventType().getName().c_str());
                        break;
                    case 3:
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

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText("Event Tracker Actions");

    if(ImGui::BeginTabBar("Event Tabs",    ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                            ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown
    )){
        if(ImGui::BeginTabItem("View Event")){
            viewEvent(selectedEvent);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Event")){
            std::pair<bool, EventPtr> newEventBuf { addEvent(unsavedChangesEvents, eventTypes, locations, guns, ammo) };

            if(newEventBuf.second && *newEventBuf.second != Event { }){
                Event& event { *newEventBuf.second };
                eventTracker->addEvent(event);

                // Check if to apply subtraction of ammo
                if(newEventBuf.first){
                    unsavedChangesStockpile = true;
                    std::vector<std::pair<Gun, TrackedAmmo>> gunsUsed { };
                    event.getAllGunsUsed(gunsUsed);                         // Get all guns used

                    for(const auto& [gun, ammo] : gunsUsed)
                        ammoTracker->removeAmmoFromStockpile(ammo);         // Apply subtraction for each gun
                }
            }

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Event Type")){
            EventType newEventType { addEventType(unsavedChangesEvents) };

            if(newEventType != EventType { })
                eventTracker->addEventType(newEventType);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Location")){
            Location newLocation { addLocation(unsavedChangesEvents) };

            if(newLocation != Location { })
                eventTracker->addLocation(newLocation);
            
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
// MARK: View Event
void EventsUI::viewEvent(std::shared_ptr<const Event> event){
    if(!ImGui::BeginChild("View Event")){
        ImGui::EndChild();
        return;
    }
    static bool showAmmoUsed { false };

    if(!event){
        ImGui::Spacing();
        ImGui::Spacing();

        UIHelper::centerTextDisabled("Select an Event");
        ImGui::EndChild();
        return;
    }

    // Detailed Event Information
    if(ImGui::BeginChild("Selected Event Detailed Information", ImVec2{ImGui::GetContentRegionAvail().x/2, 100})){
        UIHelper::centerText("Event Information");
        ImGui::Separator();
        if(event){
            ImGui::TextDisabled("Date: ");
            ImGui::SameLine(100);
            ImGui::Text("%s", event->printDate().c_str());

            ImGui::TextDisabled("Location: ");
            ImGui::SameLine(100);
            ImGui::Text("%s", event->getLocation().getName().c_str());

            ImGui::TextDisabled("Event Type: ");
            ImGui::SameLine(100);
            ImGui::Text("%s", event->getEventType().getName().c_str());
        }
        else
            UIHelper::centerText("(Select Event)");
    }
    ImGui::EndChild();
    ImGui::SameLine();


    if(ImGui::BeginChild("Event Options", ImVec2{ImGui::GetContentRegionAvail().x, 50})){
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        UIHelper::centerText("Show Ammo Used");
        ImGui::SameLine();
        ImGui::Checkbox("##Show Ammo Used Checkbox", &showAmmoUsed);
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Notes");
    ImGui::Spacing();
    ImGui::Spacing();
    if(event)
        ImGui::Text("%s", event->getNotes().c_str());
    ImGui::Spacing();
    ImGui::Spacing();

    if(event){
        static std::vector<std::pair<Gun, TrackedAmmo>> eventGunsUsed;
        event->getAllGunsUsed(eventGunsUsed);
        eventGunTable(eventGunsUsed, showAmmoUsed);
    }

    ImGui::EndChild();  // Ends tab child window
}
// MARK: Add Event
std::pair<bool, EventPtr> EventsUI::addEvent(   bool& unsavedChanges,  const EventTypeList& eventTypes, 
                                                const LocationList& locations,  const ConstGunPtrList& guns,    const ConstTrackedAmmoPtrList& ammoList
){
    if(!ImGui::BeginChild("Add Event")){
        ImGui::EndChild();
        return std::pair(false, nullptr);
    }

    static const Location   EMPTY_LOCATION { };
    static const EventType  EMPTY_EVENTTYPE { };

    static char notesBuf [UI_SETTINGS.MAX_TEXT_INPUT_CHARS_NOTES];
    static int  numGuns { 1 };

    static int              dayBuf          { 0 },  monthBuf        { 0 },  yearBuf         { 2024 };
    static Gun              selectedGun1    {  },   selectedGun2    {  },   selectedGun3    {  };
    static TrackedAmmo      selectedTA1     {  },   selectedTA2     {  },   selectedTA3     {  };

    static Location     selectedLocation     {  };
    static EventType    selectedEventType    {  };

    // Flags to let user know what went wrong when saving
    static bool invalidLocation         { false }, invalidEventType { false },  invalidDate         { false },   
                incompatibleCartridge   { false }, invalidGunOrAmmo { false },  invalidAmmoAmount   { false };

    static bool subtractRoundsUsedFromStockpile { false };  // Flag to apply ammo used to stockpile

    EventPtr    returnVal               { };
    std::string locationComboPreview    { };
    std::string eventTypeComboPreview   { };


    if(selectedLocation != EMPTY_LOCATION)
        locationComboPreview = selectedLocation.getName();
    else
        locationComboPreview = "CHOOSE LOCATION";
    
    if(selectedEventType != EMPTY_EVENTTYPE)
        eventTypeComboPreview = selectedEventType.getName().c_str();
    else
        eventTypeComboPreview = "CHOOSE EVENT TYPE";


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

        if(UIHelper::centerButton("Add Event", ImVec2{200,50})){
            // Reset flags 
            invalidLocation         = false;
            invalidEventType        = false;
            invalidDate             = false;
            incompatibleCartridge   = false;
            invalidGunOrAmmo        = false;
            invalidAmmoAmount       = false;

            // Verify buffers
            using namespace std::chrono;
            std::string notesStr { notesBuf };
            year_month_day dateBuf { year{yearBuf}, month{monthBuf}, day{dayBuf}};

            if(selectedLocation == EMPTY_LOCATION) 
                invalidLocation = true;
            if(selectedEventType == EMPTY_EVENTTYPE) 
                invalidEventType = true;
            if(!dateBuf.ok())
                invalidDate = true;

            
            // Check cartridges of selected ammo and respective gun
            switch (numGuns){
                case 3:
                    if(selectedGun3 == Gun { } || selectedTA3.ammoType == AmmoType { })
                        invalidGunOrAmmo = true;
                    else{
                        if(selectedGun3.getCartridge() != selectedTA3.ammoType.cartridge)
                            incompatibleCartridge = true;
                        if(selectedTA3.amount == 0)
                            invalidAmmoAmount = true;
                    }
                case 2:
                    if(selectedGun2 == Gun { } || selectedTA2.ammoType == AmmoType { })
                        invalidGunOrAmmo = true;
                    else{
                        if(selectedGun2.getCartridge() != selectedTA2.ammoType.cartridge)
                            incompatibleCartridge = true;
                        if(selectedTA2.amount == 0)
                            invalidAmmoAmount = true;
                    }
                case 1:
                    if(selectedGun1 == Gun { } || selectedTA1.ammoType == AmmoType { })
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
            if(!invalidLocation && !invalidEventType && !invalidDate && !incompatibleCartridge && !invalidGunOrAmmo && !invalidAmmoAmount){
                Event eventBuf { selectedLocation, selectedEventType, notesStr, dateBuf};
                switch (numGuns){
                    case 3:
                        eventBuf.addGun(selectedGun3, selectedTA3);
                    case 2:
                        eventBuf.addGun(selectedGun2, selectedTA2);
                    case 1:
                        eventBuf.addGun(selectedGun1, selectedTA1);
                        break;
                    default:
                        break;
                }
                
                returnVal = std::make_shared<Event>(eventBuf);
                unsavedChanges = true; // Set flag

                // Reset buffers
                strcpy(notesBuf,    "");

                selectedLocation    = EMPTY_LOCATION;
                selectedEventType   = EMPTY_EVENTTYPE;
                dayBuf              = 0;
                monthBuf            = 0;
                yearBuf             = 2024;

                selectedGun1 = Gun { };
                selectedGun2 = Gun { };
                selectedGun3 = Gun { };
                selectedTA1 = TrackedAmmo { };
                selectedTA2 = TrackedAmmo { };
                selectedTA3 = TrackedAmmo { };
            }

            // Make popup modal
            if(invalidLocation || invalidEventType || invalidDate || incompatibleCartridge || invalidAmmoAmount || invalidGunOrAmmo)
                ImGui::OpenPopup("Event Not Created");
        }
        // End Add Event Button

        if(ImGui::BeginPopupModal("Event Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            UIHelper::centerText("This Event could not be created.");

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

            if (UIHelper::centerButton("Close", ImVec2{120, 0}))
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
        UIHelper::centerTextDisabled("(Applied regardless of the event being saved)");
    }
    ImGui::EndChild();

    if(ImGui::CollapsingHeader("Event Information")){
        ImGui::Spacing();
        ImGui::Spacing();
        
        ImGui::Text("Location");
        ImGui::SameLine(100);
        if (ImGui::BeginCombo("##Choose Location Combo", locationComboPreview.c_str(), ImGuiComboFlags_HeightSmall)) {
            for (const auto& loc : locations) {
                const bool isSelected = (selectedLocation == loc);
                if (ImGui::Selectable(loc.getName().c_str(), isSelected))
                    selectedLocation = loc;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Event Type");
        ImGui::SameLine(100);
        if (ImGui::BeginCombo("##Choose Event Type Combo", eventTypeComboPreview.c_str(), ImGuiComboFlags_HeightSmall)) {
            for (const auto& et : eventTypes) {
                const bool isSelected = (selectedEventType == et);
                if (ImGui::Selectable(et.getName().c_str(), isSelected))
                    selectedEventType = et;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Notes");
        ImGui::SameLine(100);
        ImGui::InputTextMultiline("##Input Notes", notesBuf, UI_SETTINGS.MAX_TEXT_INPUT_CHARS_NOTES, ImVec2{ImGui::GetContentRegionAvail().x, 100});
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
        ImGui::Spacing();
        ImGui::Spacing();

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
                selectGunTable(guns, selectedGun3);
                ImGui::PopID();
            case 2:
                ImGui::PushID("Select Gun 2");
                selectGunTable(guns, selectedGun2);
                ImGui::PopID();
            case 1:
                ImGui::PushID("Select Gun 1");
                selectGunTable(guns, selectedGun1);
                ImGui::PopID();                
                break;
            default:
                break;
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::CollapsingHeader("Select Ammo Used")){
        ImGui::Spacing();
        ImGui::Spacing();

        switch(numGuns){
            case 3:
                if(selectedGun3 != Gun { }){
                    ImGui::PushID("Select Ammo 3");
                    ImGui::Separator();
                    UIHelper::centerText("Select Ammo for \"" + selectedGun3.getName() + "\"");
                    selectAmmoTable(ammoList, selectedTA3);
                    ImGui::PopID();
                }
            case 2:
                if(selectedGun2 != Gun { }){
                    ImGui::PushID("Select Ammo 2");
                    ImGui::Separator();
                    UIHelper::centerText("Select Ammo for \"" + selectedGun2.getName() + "\"");
                    selectAmmoTable(ammoList, selectedTA2);
                    ImGui::PopID();
                }
            case 1:
                if(selectedGun1 != Gun { }){
                    ImGui::PushID("Select Ammo 1");
                    ImGui::Separator();
                    UIHelper::centerText("Select Ammo for \"" + selectedGun1.getName() + "\"");
                    selectAmmoTable(ammoList, selectedTA1);
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

    return std::pair(subtractRoundsUsedFromStockpile, returnVal);
}
// MARK: ADD Event Type
EventType EventsUI::addEventType (bool& unsavedChanges) {
    static char eventTypeBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "New Event Type";

    EventType returnVal;

    if(!ImGui::BeginChild("Add New Event Type")){
        ImGui::EndChild();
        return EventType { };
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Event Type to choose from when creating an Event.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Event Type");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Event Type", eventTypeBuf, UI_SETTINGS.MAX_TEXT_INPUT_CHARS, ImGuiInputTextFlags_CharsUppercase);

    ImGui::Spacing();
    ImGui::Spacing();

    if(UIHelper::centerButton("Add Event Type", ImVec2{200,50})){
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
            returnVal = EventType {eventTypeStrBuf};
            strcpy(eventTypeBuf, "New Event Type");     // Reset buffers
            unsavedChanges = true;                      // Set flag
        }   
        else
            ImGui::OpenPopup("Event Type Not Created");
    }

    if(ImGui::BeginPopupModal("Event Type Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        UIHelper::centerText("The Event Type could not be created.");
        UIHelper::centerText("Invalid name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (UIHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();

    return returnVal;
}
// MARK: ADD Location
Location EventsUI::addLocation (bool& unsavedChanges) {
    static char locationBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "New Location";

    Location returnVal;

    if(!ImGui::BeginChild("Add New Location")){
        ImGui::EndChild();
        return Location { };
    }

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new location to choose from when creating an Event.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Location");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Location", locationBuf, UI_SETTINGS.MAX_TEXT_INPUT_CHARS, 0);

    ImGui::Spacing();
    ImGui::Spacing();

    if(UIHelper::centerButton("Add Location", ImVec2{200,50})){
        bool nameRejected { false };
        std::string locationStrBuf { locationBuf };
        for(const auto& c : locationStrBuf){
            if(!isalnum(c) && c != ' '){
                nameRejected = true;
                break;
            }
        }

        if(locationStrBuf != "New Location" && !nameRejected){
            returnVal = Location{locationStrBuf};
            strcpy(locationBuf, "New Location");     // Reset buffers
            unsavedChanges = true;                   // Set flag
        }   
        else
            ImGui::OpenPopup("Location Not Created");
    }

    if(ImGui::BeginPopupModal("Location Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        UIHelper::centerText("The location could not be created.");
        UIHelper::centerText("Invalid name.");

        ImGui::Spacing();
        ImGui::Spacing();
        
        if (UIHelper::centerButton("Close", ImVec2{120, 0}))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild();

    return returnVal;
}
// MARK: Event Gun Table
void EventsUI::eventGunTable(const std::vector<std::pair<Gun, TrackedAmmo>>& list, bool showAmmoUsed) {
    std::vector<std::pair<Gun, TrackedAmmo>>::const_iterator itr { list.begin() };

    static const Gun emptyGun   { };

    static int      tableColumns    { 3 };
    static float    tableHeight     { 100 };

    if(showAmmoUsed)
        tableColumns = 7;
    else
        tableColumns = 3;

    ImGui::SeparatorText("Guns Used");
    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginTable("Gun Table", tableColumns, 
                                ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, ImVec2(ImGui::GetContentRegionAvail().x-2, tableHeight) ))
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
                                ImGui::Text("%s", gun.getCartridge().getName().c_str());
                                break;
                            case 2:
                                ImGui::Text("%s", gun.getName().c_str());
                                break;
                            case 3:
                                ImGui::Text("%s", ammo.ammoType.name.c_str());
                                break;
                            case 4:
                                ImGui::Text("%s", ammo.ammoType.manufacturer.getName().c_str());
                                break;
                            case 5:
                                ImGui::Text("%d", ammo.ammoType.grainWeight);
                                break;
                            case 6:
                                ImGui::Text("%d", ammo.amount);
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

// MARK: Select Gun Table
void EventsUI::selectGunTable(const ConstGunPtrList& gunList, Gun& selectedGun){

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
                            ImGui::Text("%s", gun.getCartridge().getName().c_str());
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

// MARK: Select Ammo Table
void EventsUI::selectAmmoTable(const ConstTrackedAmmoPtrList& ammoList, TrackedAmmo& selectedAmmo){
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-4, 200};
    if(tableSize.x < 400){
        tableSize = ImVec2{400, 200};
    }

    int row { 0 };
    if(ImGui::BeginChild("Ammo Table Window", ImVec2{tableSize.x + 2, tableSize.y+2})){

        // Select Ammo Table
        if(ImGui::BeginTable("Ammo Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, tableSize )) {
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
                            ImGui::Text("%s", ammoType.manufacturer.getName().c_str());
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

