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

    // Horizontal bigger than vertical, display tabs side by side
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

    if(ImGui::BeginChild("Stockpile Quick View", childWindowSizes, ImGuiChildFlags_Border) && showStockpile)
        StockpileUI::home(ammoTracker, unsavedChanges.stockpile);
    ImGui::EndChild();

    if(horizontalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Events Quick View", childWindowSizes, ImGuiChildFlags_Border) && showEvents)
        EventsUI::home(eventTracker, ammoTracker, gunTracker, unsavedChanges);
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
    static ConstGunPtr                                  selectedGun { };

    static bool savingGunsFailed { false }, savingWeaponTypesFailed { false }; // Flags to tell user what went wrong when attempting to save

    uint64_t    roundsInLifetime { 0 };
    ImVec2      tableSize { ImGui::GetContentRegionAvail().x, 200};

    gunTracker->getAllGuns(gunList);
    gunTracker->getRoundsShotPerCartridge(roundsPerCartridge);
    gunTracker->getAllWeaponTypeNames(wpnTypes);


    // Calculate rounds in lifetime
    for(const auto& gunElm : gunList)
        roundsInLifetime += gunElm->getRoundCount();

    // Ensure table doesnt get bigger than 400 width
    if(tableSize.x < 400)
        tableSize = ImVec2{400, 200};


    // UI Element Start

    ImGui::Spacing();
    ImGui::Spacing();

    UIHelper::centerText("Armory");
    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Spacing();

    // Armory Details
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

            // All saving successful, reset flag
            if(!savingGunsFailed || !savingWeaponTypesFailed)
                unsavedChanges = false;
        }
        
        // Open popup showing what went wrong
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

        // Alert user
        if(unsavedChanges)
            UIHelper::centerTextDisabled("(There are unsaved changes)");
    }
    ImGui::EndChild();

    // Armory table
    ImGui::SeparatorText( "Guns" );
    ImGui::Spacing();
    int row { 0 };
    if(ImGui::BeginTable("View Gun Table", 4, ImGuiTableFlags_Borders  | ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, tableSize)) {
        ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Round Count",  ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& gunElement : gunList){
            const Gun& gun {*gunElement};
            bool isGunSelected { false };
            
            if(gunElement == selectedGun)
                isGunSelected = true;

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column){
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(gun.getWeaponType().getName().c_str(), &isGunSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isGunSelected)
                            selectedGun = gunElement;

                        break;
                    case 1:
                        ImGui::Text("%s", gun.getCartridge().getName().c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", gun.getName().c_str());
                        break;
                    case 3:
                        ImGui::Text("%d", gun.getRoundCount());
                        break;
                    default:
                        ImGui::TextDisabled("Broken table");
                        break;
                }
            }
            ImGui::PopID();
            ++row;       
        }
        ImGui::EndTable();
    }
   
    ImGui::Spacing();
    ImGui::Spacing();

    // Tabs to add/change armory information
    ImGui::SeparatorText("Armory Actions");
    if(ImGui::BeginTabBar("Armory Tabs",    ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                            ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown)) {
        if(ImGui::BeginTabItem("Detailed Gun Viewer")){
            viewGun(selectedGun);
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
// MARK: View Gun
void ArmoryUI::viewGun(std::shared_ptr<const Gun> gun){

    ImGui::Spacing();
    ImGui::Spacing();

    if(!gun){
        UIHelper::centerTextDisabled("Select a Gun");
        return;
    }

    static ConstTrackedAmmoPtrList ammoList;
    gun->getAllAmmoUsed(ammoList);

    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(tableSize.x < 400)
        tableSize = ImVec2{400, 200};

    int row { 0 };
    if(ImGui::BeginTable("Detailed Gun View Table", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, tableSize )) {
        ImGui::TableSetupColumn("Cartridge",    0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Grain Weight", 0);
        ImGui::TableSetupColumn("Rounds Shot",  0);
        ImGui::TableHeadersRow();            

        for(const auto& ta : ammoList){
            
            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 5; ++column) {
                ImGui::TableSetColumnIndex(column);

                switch( column ){
                    case 0:
                        ImGui::Text("%s", ta->ammoType.cartridge.getName().c_str());
                        break;
                    case 1:
                        ImGui::Text("%s", ta->ammoType.name.c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", ta->ammoType.manufacturer.getName().c_str());
                        break;
                    case 3:
                        ImGui::Text("%d", ta->ammoType.grainWeight);
                        break;
                    case 4:
                        ImGui::Text("%d", ta->amount);
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
}

// MARK: Add Gun
GunPtr ArmoryUI::addGun(bool& unsavedChanges, const CartridgeList& cartridges, const WeaponTypeList& wpnTypes) {
    if(!ImGui::BeginChild("Add Gun")){
        ImGui::EndChild();
        return nullptr;
    }

    static Cartridge    selectedCartridge   { };
    static WeaponType   selectedWT          { };
    static int          verifyGunReturn     { -1 };


    static char nameBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "Enter Name";

    GunPtr      returnVal           { nullptr };
    std::string cartComboPreview    { };
    std::string wtComboPreview      { };

    // Apply correct text to display in combo boxes
    if(selectedCartridge != EMPTY_OBJECTS.CARTRIDGE)
        cartComboPreview = selectedCartridge.getName().c_str();
    else
        cartComboPreview = "CHOOSE CARTRIDGE";
    
    if(selectedWT != EMPTY_OBJECTS.WEAPON_TYPE)
        wtComboPreview = selectedWT.getName().c_str();
    else
        wtComboPreview = "CHOOSE WEAPON TYPE";


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


    // Attempt to make new Gun
    if(UIHelper::centerButton("Add New Gun", ImVec2{200,50})){
        std::string name { nameBuf };

        verifyGunReturn = verifyGun(name, selectedWT, selectedCartridge);
      
        // Check return value
        if(verifyGunReturn == 0){
            returnVal = std::make_shared<Gun>(name, selectedWT, selectedCartridge);
            unsavedChanges = true;

            // Reset buffers
            strcpy(nameBuf, "Enter Name");
            selectedWT = EMPTY_OBJECTS.WEAPON_TYPE;
            selectedCartridge = EMPTY_OBJECTS.CARTRIDGE;
            verifyGunReturn = -1;
        }
        else
            ImGui::OpenPopup("Gun Not Created");
    }

    
    // Display error popup
    if(ImGui::BeginPopupModal("Gun Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        UIHelper::centerText("This gun could not be created.");

        // Display proper message using chmod like handling
        switch(verifyGunReturn){
            case 1:
                ImGui::BulletText("Invalid cartridge");
                break;
            case 2:
                ImGui::BulletText("Invalid weapon type");
                break;
            case 3:
                ImGui::BulletText("Invalid cartridge");
                ImGui::BulletText("Invalid weapon type");
                break;
            case 4:
                ImGui::BulletText("Invalid name");
                break;
            case 5:
                ImGui::BulletText("Invalid name");
                ImGui::BulletText("Invalid cartridge");
                break;
            case 6:
                ImGui::BulletText("Invalid name");
                ImGui::BulletText("Invalid weapon type");
                break;
            case 7:
                ImGui::BulletText("Invalid name");
                ImGui::BulletText("Invalid weapon type");
                ImGui::BulletText("Invalid cartridge");
                break;
            default:
                UIHelper::centerTextDisabled("INVALID RETURN");
                break;
        }

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
        std::string wtName { wtBuf };

        // Make upper case
        for(auto& c : wtName)
            c = toupper(c);

        // Compared to uppercase since already toupper() the string
        if(wtName != "NEW WEAPON TYPE" && verifyWeaponTypeName(wtName)){      
            returnVal = WeaponType{wtName};     // Set return buffer
            strcpy(wtBuf, "New Weapon Type");   // Reset input buffers
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
// MARK: Verify Gun
int ArmoryUI::verifyGun(const std::string& name, const WeaponType& wt, const Cartridge& cartridge){
    int     returnVal           { 0 };
    bool    invalidName         { false };  // 4
    bool    invalidWeaponType   { false };  // 2
    bool    invalidCartridge    { false };  // 1

    if(wt == EMPTY_OBJECTS.WEAPON_TYPE)
        invalidWeaponType = true;
    if(cartridge == EMPTY_OBJECTS.CARTRIDGE)
        invalidCartridge = true;
    if(name.empty() || name == "Enter Name")
        invalidName = true;

    if(invalidName)
        returnVal += 4;
    if(invalidWeaponType)
        returnVal += 2;
    if(invalidCartridge)
        returnVal += 1;
    
    return returnVal;
}
// MARK: Verify WT Name
bool ArmoryUI::verifyWeaponTypeName(const std::string& name){
    // Verify no invalid characters & make upper case
    for(auto& c : name){
        if(!isalnum(c) && c != ' ')
            return false;
    }

    return true;
}

// MARK: STOCKPILE UI
void StockpileUI::home (AmmoTrackerPtr ammoTracker, bool& unsavedChanges){
    static StringVector                     ammoNames;
    static CartridgeList                    cartridges;
    static ManufacturerList                 manufacturers;
    static AmountPerCartridgeList           countByCartridge;

    static std::vector<ConstTrackedAmmoPtr> ammoList;
    static std::vector<TrackedAmmo>         selectedCatridgeAmmoList;

    static Cartridge    selectedCartridge   { };
    static bool         detailedView        { false };

    static bool ammoNotSaved { false }, cartridgesNotSaved { false }, manufacturersNotSaved { false };  // Flags to let user know what failed saving

    std::string cartridgeComboPreview { };

    ammoTracker->getAllAmmoNames            (ammoNames);
    ammoTracker->getAllCartridges           (cartridges);
    ammoTracker->getAllManufacturers        (manufacturers);
    ammoTracker->getAmmoCountByCartridge    (countByCartridge);
    ammoTracker->getAllAmmo                 (ammoList);


    // Apply proper prompt text
    if(selectedCartridge != EMPTY_OBJECTS.CARTRIDGE)
        cartridgeComboPreview = selectedCartridge.getName().c_str();
    else
        cartridgeComboPreview = "CHOOSE CARTRIDGE";

    if(!detailedView)
        selectedCartridge = EMPTY_OBJECTS.CARTRIDGE;

    // Calculate HUD View measurements
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(detailedView){
        if(tableSize.x < 400)
            tableSize = ImVec2{400, 200};
    }
    else{
        if(tableSize.x < 500)
            tableSize = ImVec2{500, 200};
    }


    // UI Elements
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

        if(detailedView){
            ImGui::Spacing();
            ImGui::Spacing();

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
            ammoTracker->getAllAmmoByCartridge(selectedCatridgeAmmoList, selectedCartridge);  // Populates list of ammo with selected cartridge for use in combo dropdown
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Stockpile Buttons", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){

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

            // All checks passed, reset flag
            if(!ammoNotSaved && !cartridgesNotSaved && !manufacturersNotSaved)
                unsavedChanges = false;
        }
        
        // If any checks failed, show popup
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

    ImGui::Spacing();
    ImGui::Spacing();

    // Show ammo only for selected cartridge
    if(detailedView && selectedCartridge != EMPTY_OBJECTS.CARTRIDGE){

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

            if (selectedCartridge != EMPTY_OBJECTS.CARTRIDGE) {
                for(const auto& ta : selectedCatridgeAmmoList){
                    ImGui::TableNextRow();

                    for (int column{0}; column < 4; ++column) {
                        ImGui::TableSetColumnIndex(column);
                        switch( column ){
                            case 0:
                                ImGui::Text("%s", ta.ammoType.name.c_str());
                                break;
                            case 1:
                                ImGui::Text("%s", ta.ammoType.manufacturer.getName().c_str());
                                break;
                            case 2:
                                ImGui::Text("%d", ta.ammoType.grainWeight);
                                break;
                            case 3:
                                ImGui::Text("%d", ta.amount);
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
    // Show ammo amount by cartridge
    else{
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
                for(const auto& [cart, amount] : countByCartridge){
                    ImGui::TableNextRow();
                    for (int column{0}; column < 2; ++column)
                    {
                        ImGui::TableSetColumnIndex(column);
                        switch( column ){
                            case 0:
                                ImGui::Text("%s", cart.getName().c_str());
                                break;
                            case 1:
                                ImGui::Text("%d", amount);
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

    // Stockpile Tabs
    ImGui::SeparatorText("Stockpile Actions");
    if(ImGui::BeginTabBar("Stockpile Tabs",     ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                                ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown
    )){
        if(ImGui::BeginTabItem("Add to Existing Ammo")){        
            TrackedAmmo ammoToAddTo {addToExistingAmmoType(unsavedChanges, ammoList)};

            if(ammoToAddTo != TrackedAmmo { })
                ammoTracker->addAmmoToStockpile(ammoToAddTo);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Ammo Type")){
            TrackedAmmo newAmmoType {addAmmoType(unsavedChanges, cartridges, manufacturers)};

            if(newAmmoType != TrackedAmmo { })
                ammoTracker->addAmmoToStockpile(newAmmoType);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Cartridge")){
            Cartridge newCartridge {addCartridge(unsavedChanges)};

            if(newCartridge != Cartridge { })
                ammoTracker->addCartridge(newCartridge);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Manufacturer")){
            Manufacturer newManufacturer {addManufacturer(unsavedChanges)};

            if(newManufacturer != Manufacturer { })
                ammoTracker->addManufacturer(newManufacturer);

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
// MARK: Add Cartrigde
Cartridge StockpileUI::addCartridge (bool& unsavedChanges) {
    if(!ImGui::BeginChild("Add New Cartridge")){
        ImGui::EndChild();
        return Cartridge { };
    }

    static char cartridgeBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "New Cartridge";

    Cartridge returnVal    { };


    // UI Elements
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
        std::string cartridgeName   { cartridgeBuf };


        // Perform Checks and set return value if passed
        if(cartridgeName != "New Cartridge" && verifyCartridgeName(cartridgeName)){
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
    static char         nameBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "Enter Name";
    static Cartridge    selectedCartridge                           { };
    static Manufacturer selectedManufacturer                        { };
    static int          grainWeightBuf                              { 0 };
    static int          verifyReturnVal                             { -1 };

    std::string cartridgeComboPreview       { };
    std::string manufacturerComboPreview    { };
    TrackedAmmo returnVal                   { };

    // Apply proper prompt text
    if(selectedCartridge != EMPTY_OBJECTS.CARTRIDGE)
        cartridgeComboPreview = selectedCartridge.getName();
    else
        cartridgeComboPreview = "CHOOSE CARTRIDGE";
    
    if(selectedManufacturer != EMPTY_OBJECTS.MANUFACTURER)
        manufacturerComboPreview = selectedManufacturer.getName();
    else
        manufacturerComboPreview = "CHOOSE MANUFACTURER";

    // UI Elements
    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new ammo type to tracked items.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    ImGui::BulletText("To add a new manufacturer, see the \"Add New Manufacturer\" tab.");
    ImGui::BulletText("To add a new cartridge, see the \"Add New Cartridge\" tab.");

    ImGui::Spacing();
    ImGui::Spacing();
    
    // Input information
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


    if(UIHelper::centerButton("Add New Ammo Type", ImVec2{200,50})){
        std::string name { nameBuf };
        
        verifyReturnVal = verifyAmmoType(name, selectedManufacturer, selectedCartridge, grainWeightBuf);
        
        // All checks passed, set return val and reset buffers
        if(verifyReturnVal == 0){

            returnVal = TrackedAmmo {AmmoType{ name, selectedManufacturer, selectedCartridge, grainWeightBuf}, 0};

            // Reset buffers
            strcpy(nameBuf, "Enter Name");

            selectedManufacturer    = EMPTY_OBJECTS.MANUFACTURER;
            selectedCartridge       = EMPTY_OBJECTS.CARTRIDGE;
            grainWeightBuf          = 0;
            verifyReturnVal         = -1;


            unsavedChanges = true;  // Set flag
        }
        else
            ImGui::OpenPopup("Ammo Type Not Created");
    }

    if(ImGui::BeginPopupModal("Ammo Type Not Created", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        errorMakingAmmoTypePopupText(verifyReturnVal);

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
    if(!ImGui::BeginChild("Add New Manufacturer")){
        ImGui::EndChild();
        return Manufacturer { };
    }

    static char manBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "New Manufacturer";

    Manufacturer returnVal { };

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
        std::string manName         { manBuf };

        if(manName != "New Manufacturer" && verifyManufacturerName(manName)){
            returnVal = Manufacturer{manName};
            strcpy(manBuf, "New Manufacturer");     // Reset buffers
            unsavedChanges = true;                  // Set flag
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

    static TrackedAmmo          selectedAmmo{  };
    static int                  amountToAdd { 0 };

    static bool amountRejected { false }, ammoRejected { false };   // Flags to alert user what went wrong

    bool            submitAmmo          { false };                      // Flag to return the proper selected ammo instead of empty ammo
    float           fullWindowWidth     { ImGui::GetWindowSize().x };   // Calculate center button add now (since cannot do inside child since middle will be skewed)
    std::string     ammoNamePreview     { };

    // Apply proper prompt text
    if(selectedAmmo != EMPTY_OBJECTS.TRACKED_AMMO)
        ammoNamePreview = selectedAmmo.ammoType.name;
    else
        ammoNamePreview = "CHOOSE AMMO TYPE";

    // UI Elements
    ImGui::Text("Directions");
    ImGui::BulletText("This will add the amount to an existing Ammo Type in the stockpile.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Spacing();
    ImGui::Spacing();

    if(ImGui::BeginChild("Enter Info", ImVec2{ImGui::GetContentRegionAvail().x-300, ImGui::GetContentRegionAvail().y})){
        ImGui::Text("Ammo Name");
        ImGui::SameLine(100);
        
        if (ImGui::BeginCombo("##Choose Ammo Name Combo", ammoNamePreview.c_str(), ImGuiComboFlags_HeightSmall)){
            for (const auto& ammo : ammoList){
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

        ImGui::SetCursorPosX((fullWindowWidth - 200) * 0.5f); // 200 is button width
        
        if(ImGui::Button("Add", ImVec2{200,50})){
            // Ensure buffers are valid before manipulating
            amountRejected = false;
            ammoRejected = false;

            // Verify buffers are valid
            if(amountToAdd <= 0)
                amountRejected = true;
            if(selectedAmmo == EMPTY_OBJECTS.TRACKED_AMMO)
                ammoRejected = true;

            // All checks passed, set flag to signal to return correct object
            if(!ammoRejected && !amountRejected){
                submitAmmo = true;
                unsavedChanges = true;

                selectedAmmo.amount = amountToAdd;

                amountToAdd = 0; // Reset buffer
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
        if(selectedAmmo != EMPTY_OBJECTS.TRACKED_AMMO){
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
        selectedAmmo = EMPTY_OBJECTS.TRACKED_AMMO;
        return ammoBuf;
    }
    else
        return EMPTY_OBJECTS.TRACKED_AMMO;
}
// MARK: Verify Items
bool StockpileUI::verifyCartridgeName(const std::string& name){
    // Verify characters are valid
    for(const auto& c : name){
        if(!isalnum(c) && c != ' ' && c != '.')
            return false;
    }

    return true;
}
bool StockpileUI::verifyManufacturerName(const std::string& name){
    // Verify characters are valid
    for(const auto& c : name){
        if(!isalnum(c) && c != ' ')
            return false;
    }

    return true;
}
int StockpileUI::verifyAmmoType(const std::string& name, const Manufacturer& manufacturer, const Cartridge& cartridge, int grainWeight){
    int     returnVal           { 0 };
    bool    invalidName         { false };  // 8
    bool    invalidManufacturer { false };  // 4
    bool    invalidCartridge    { false };  // 2
    bool    invalidGrainWeight  { false };  // 1

     // Verify buffers are valid 
    if(manufacturer == EMPTY_OBJECTS.MANUFACTURER) 
        invalidManufacturer = true;
    if(cartridge == EMPTY_OBJECTS.CARTRIDGE) 
        invalidCartridge = true;
    if(grainWeight <= 0 || grainWeight > 500) 
        invalidGrainWeight = true;
    if(name == "Enter Name" || name.empty())
        invalidName = true;
    
    if(invalidName)
        returnVal += 8;
    if(invalidManufacturer)
        returnVal += 4;
    if(invalidCartridge)
        returnVal += 2;
    if(invalidGrainWeight)
        returnVal += 1;
    
    return returnVal;
}
void StockpileUI::errorMakingAmmoTypePopupText(int verifyAmmoReturnVal){
    UIHelper::centerText("This ammo type could not be created.");

    switch(verifyAmmoReturnVal){
        case 1:
            ImGui::BulletText("Invalid grain weight");
            break;
        case 2:
            ImGui::BulletText("Invalid cartridge");
            break;
        case 3:
            ImGui::BulletText("Invalid cartridge");
            ImGui::BulletText("Invalid grain weight");
            break;
        case 4:
            ImGui::BulletText("Invalid manufacturer");
            break;
        case 5:
            ImGui::BulletText("Invalid manufacturer");
            ImGui::BulletText("Invalid grain weight");
            break;
        case 6:
            ImGui::BulletText("Invalid manufacturer");
            ImGui::BulletText("Invalid cartridge");
            break;
        case 7:
            ImGui::BulletText("Invalid manufacturer");
            ImGui::BulletText("Invalid cartridge");
            ImGui::BulletText("Invalid grain weight");
            break;
        case 8:
            ImGui::BulletText("Invalid name");
            break;
        case 9:
            ImGui::BulletText("Invalid name");
            ImGui::BulletText("Invalid grain weight");
            break;
        case 10:
            ImGui::BulletText("Invalid name");
            ImGui::BulletText("Invalid cartridge");
            break;
        case 11:
            ImGui::BulletText("Invalid name");
            ImGui::BulletText("Invalid cartridge");
            ImGui::BulletText("Invalid grain weight");
            break;
        case 12:
            ImGui::BulletText("Invalid name");
            ImGui::BulletText("Invalid manufacturer");
            break;
        case 13:
            ImGui::BulletText("Invalid name");
            ImGui::BulletText("Invalid manufacturer");
            ImGui::BulletText("Invalid grain weight");
            break;
        case 14:
            ImGui::BulletText("Invalid name");
            ImGui::BulletText("Invalid manufacturer");
            ImGui::BulletText("Invalid cartridge");
            break;
        case 15:
            ImGui::BulletText("Invalid name");
            ImGui::BulletText("Invalid manufacturer");
            ImGui::BulletText("Invalid cartridge");
            ImGui::BulletText("Invalid grain weight");
            break;
        default:

            break;
    }
}


// MARK: EVENTS UI
void EventsUI::home(EventTrackerPtr eventTracker, AmmoTrackerPtr ammoTracker, GunTrackerPtr gunTracker, ShooterCentralWindow::UnsavedChanges& unsavedChanges) {
    
    static std::vector<EventPtr>    eventList       { };
    static EventTypeList            eventTypes      { };
    static LocationList             locations       { };
    static ConstTrackedAmmoPtrList  ammo            { };
    static EventPtr                 selectedEvent   { nullptr }; 

    static bool eventsNotSaved { false }, eventTypesNotSaved { false }, locationsNotSaved { false }; // Flags to alert user what went wrong

    eventTracker->getAllEventTypes  (eventTypes);
    eventTracker->getAllLocations   (locations);
    ammoTracker->getAllAmmo         (ammo);
    eventTracker->getAllEvents      (eventList);

    // HUD View measurements
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(tableSize.x < 400)
        tableSize = ImVec2{400, 200};

    // UI Elements
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

    if(ImGui::BeginChild("Event Tracker Buttons", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        if(UIHelper::centerButton("Save Events", ImVec2{200, 50})){

            // Reset flags
            eventsNotSaved      = false;
            eventTypesNotSaved  = false;
            locationsNotSaved   = false;

            if(!eventTracker->writeAllEvents())
                eventsNotSaved = true;
            if(!eventTracker->writeAllEventTypes())
                eventTypesNotSaved = true;
            if(!eventTracker->writeAllLocations())
                locationsNotSaved = true;

            // Reset unsaved changes flag
            if(!eventsNotSaved && !eventTypesNotSaved && !locationsNotSaved)
                unsavedChanges.events = false;
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
                unsavedChanges.events = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(unsavedChanges.events)
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

        for(const auto& e : eventList){
            const Event& event      { *e };
            bool isEventSelected    { false };

            if(selectedEvent){
                if(event == *selectedEvent)
                    isEventSelected = true;
            }

            ImGui::PushID(std::to_string(row).c_str()); // Needed to ensure no selectable collisions due to same ID

            ImGui::TableNextRow();
            for (int column{0}; column < 4; ++column){                   
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(event.printDate().c_str(), &isEventSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isEventSelected)
                            selectedEvent = e;
                        break;
                    case 1:
                        ImGui::Text("%s", event.getLocation().getName().c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", event.getEventType().getName().c_str());
                        break;
                    case 3:
                        ImGui::Text("%d", event.getNumGunsUsed());
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }
            ImGui::PopID();
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
            auto newEventBuf { addEvent(gunTracker, eventTypes, locations, ammo, unsavedChanges.events) };

            // Check should make new event
            if(std::get<2>(newEventBuf) && *std::get<2>(newEventBuf) != Event { }){
                Event& event { *std::get<2>(newEventBuf) };
                eventTracker->addEvent(event);

                // Check if supposed to apply subtraction of ammo OR to guns
                if(std::get<0>(newEventBuf) || std::get<1>(newEventBuf)){
                    std::vector<std::pair<ConstGunPtr, ConstTrackedAmmoPtr>> gunsUsed { };
                    event.getAllGunsUsed(gunsUsed);                         // Get all guns used

                    for(const auto& [gun, ammo] : gunsUsed){

                        // Apply Stockpile changes
                        if(std::get<0>(newEventBuf)){
                            ammoTracker->removeAmmoFromStockpile(*ammo);
                            unsavedChanges.stockpile = true; 
                        }
                        
                        // Apply Gun history changes
                        if(std::get<1>(newEventBuf)){
                            GunPtr gunBuf { gunTracker->getGun(*gun) };
                            gunBuf->addToRoundCount(*ammo);
                            unsavedChanges.armory = true; 
                        }
                    } 
                }
            }

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Event Type")){
            EventType newEventType { addEventType(unsavedChanges.events) };

            if(newEventType != EventType { })
                eventTracker->addEventType(newEventType);

            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add New Location")){
            Location newLocation { addLocation(unsavedChanges.events) };

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

    static std::vector<std::pair<ConstGunPtr, ConstTrackedAmmoPtr>> eventGunsUsed;


    if(!event){
        ImGui::Spacing();
        ImGui::Spacing();

        UIHelper::centerTextDisabled("Select an Event");
        ImGui::EndChild();
        return;
    }
    else
        event->getAllGunsUsed(eventGunsUsed);

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

    if(event)
        eventGunTable(eventGunsUsed, showAmmoUsed);

    ImGui::EndChild();
}
// MARK: Add Event
std::tuple<bool, bool, EventPtr> EventsUI::addEvent(   GunTrackerPtr gunTracker,  const EventTypeList& eventTypes, 
                                                const LocationList& locations,  const ConstTrackedAmmoPtrList& ammoList, bool& unsavedChanges
){
    if(!ImGui::BeginChild("Add Event")){
        ImGui::EndChild();
        return std::tuple(false, false, nullptr);
    }

    static constexpr std::string_view checkboxText { "Subtract Ammo From Stockpile" };

    static ConstGunPtrList guns { };

    static char notesBuf [UI_SETTINGS.MAX_TEXT_INPUT_CHARS_NOTES];
    static int  numGuns { 1 };

    static int          dayBuf          { 0 },  monthBuf        { 0 }, yearBuf         { 2024 };
    static Gun          selectedGun1    {  },   selectedGun2    {  },  selectedGun3    {  };
    static TrackedAmmo  selectedTA1     {  },   selectedTA2     {  },  selectedTA3     {  };

    static Location     selectedLocation     {  };
    static EventType    selectedEventType    {  };

    // Flags to let user know what went wrong when saving
    static bool invalidLocation         { false }, invalidEventType { false },  invalidDate         { false },   
                incompatibleCartridge   { false }, invalidGunOrAmmo { false },  invalidAmmoAmount   { false };

    static bool applyToStockpile { false }, applyToGuns { false };  // Flag to apply items to other parts of SC

    EventPtr    returnVal               { };
    std::string locationComboPreview    { };
    std::string eventTypeComboPreview   { };

    gunTracker->getAllGuns (guns);


    // Apply proper text to prompt
    if(selectedLocation != EMPTY_OBJECTS.LOCATION)
        locationComboPreview = selectedLocation.getName();
    else
        locationComboPreview = "CHOOSE LOCATION";
    
    if(selectedEventType != EMPTY_OBJECTS.EVENT_TYPE)
        eventTypeComboPreview = selectedEventType.getName().c_str();
    else
        eventTypeComboPreview = "CHOOSE EVENT TYPE";

    // UI Elements
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

        if(ImGui::BeginChild("Apply Amounts", ImVec2{ImGui::GetContentRegionAvail().x/2, 110})) {
            ImGui::Text("Apply to Stockpile");
            ImGui::SameLine(135);
            ImGui::Checkbox("##Apply To Stockpile", &applyToStockpile);

            ImGui::Text("Apply to Armory");
            ImGui::SameLine(135);
            ImGui::Checkbox("##Apply To Guns", &applyToGuns);
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if(ImGui::BeginChild("Add Event Button", ImVec2{ImGui::GetContentRegionAvail().x, 110})) {
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
                std::string     notesStr    { notesBuf };
                year_month_day  dateBuf     { year{yearBuf}, month{monthBuf}, day{dayBuf}};

                if(selectedLocation == EMPTY_OBJECTS.LOCATION) 
                    invalidLocation = true;
                if(selectedEventType == EMPTY_OBJECTS.EVENT_TYPE) 
                    invalidEventType = true;
                if(!dateBuf.ok())
                    invalidDate = true;

                
                // Check cartridges of selected ammo and respective gun for valid input
                switch (numGuns){
                    case 3:
                        if(selectedGun3 == EMPTY_OBJECTS.GUN|| selectedTA3 == EMPTY_OBJECTS.TRACKED_AMMO)
                            invalidGunOrAmmo = true;
                        else{
                            if(selectedGun3.getCartridge() != selectedTA3.ammoType.cartridge)
                                incompatibleCartridge = true;
                            if(selectedTA3.amount == 0)
                                invalidAmmoAmount = true;
                        }
                    case 2:
                        if(selectedGun2 == EMPTY_OBJECTS.GUN || selectedTA2 == EMPTY_OBJECTS.TRACKED_AMMO)
                            invalidGunOrAmmo = true;
                        else{
                            if(selectedGun2.getCartridge() != selectedTA2.ammoType.cartridge)
                                incompatibleCartridge = true;
                            if(selectedTA2.amount == 0)
                                invalidAmmoAmount = true;
                        }
                    case 1:
                        if(selectedGun1 == EMPTY_OBJECTS.GUN || selectedTA1== EMPTY_OBJECTS.TRACKED_AMMO)
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

                    selectedLocation    = EMPTY_OBJECTS.LOCATION;
                    selectedEventType   = EMPTY_OBJECTS.EVENT_TYPE;
                    dayBuf              = 0;
                    monthBuf            = 0;
                    yearBuf             = 2024;

                    selectedGun1    = EMPTY_OBJECTS.GUN;
                    selectedGun2    = EMPTY_OBJECTS.GUN;
                    selectedGun3    = EMPTY_OBJECTS.GUN;
                    selectedTA1     = EMPTY_OBJECTS.TRACKED_AMMO;
                    selectedTA2     = EMPTY_OBJECTS.TRACKED_AMMO;
                    selectedTA3     = EMPTY_OBJECTS.TRACKED_AMMO;
                }

                // Make popup modal if there was an error
                if(invalidLocation || invalidEventType || invalidDate || incompatibleCartridge || invalidAmmoAmount || invalidGunOrAmmo)
                    ImGui::OpenPopup("Event Not Created");
            }
            // End Add Event Button
        }
        ImGui::EndChild();

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
                if(selectedGun3 != EMPTY_OBJECTS.GUN){
                    ImGui::PushID("Select Ammo 3");
                    ImGui::Separator();
                    UIHelper::centerText("Select Ammo for \"" + selectedGun3.getName() + "\"");
                    selectAmmoTable(ammoList, selectedTA3);

                    // Select amount of ammo used
                    ImGui::Text("Amount Used");
                    ImGui::SameLine();
                    ImGui::InputInt("##Amount of Ammo Used", &selectedTA3.amount);

                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::PopID();
                }
            case 2:
                if(selectedGun2 != EMPTY_OBJECTS.GUN){
                    ImGui::PushID("Select Ammo 2");
                    ImGui::Separator();
                    UIHelper::centerText("Select Ammo for \"" + selectedGun2.getName() + "\"");
                    selectAmmoTable(ammoList, selectedTA2);

                    // Select amount of ammo used
                    ImGui::Text("Amount Used");
                    ImGui::SameLine();
                    ImGui::InputInt("##Amount of Ammo Used", &selectedTA2.amount);

                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::PopID();
                }
            case 1:
                if(selectedGun1 != EMPTY_OBJECTS.GUN){
                    ImGui::PushID("Select Ammo 1");
                    ImGui::Separator();
                    UIHelper::centerText("Select Ammo for \"" + selectedGun1.getName() + "\"");
                    selectAmmoTable(ammoList, selectedTA1);

                    // Select amount of ammo used
                    ImGui::Text("Amount Used");
                    ImGui::SameLine();
                    ImGui::InputInt("##Amount of Ammo Used", &selectedTA1.amount);

                    ImGui::Spacing();
                    ImGui::Spacing();

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

    return std::tuple(applyToStockpile, applyToGuns, returnVal);
}
// MARK: ADD Event Type
EventType EventsUI::addEventType (bool& unsavedChanges) {
    if(!ImGui::BeginChild("Add New Event Type")){
        ImGui::EndChild();
        return EventType { };
    }

    static char eventTypeBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "New Event Type";

    EventType returnVal;

    // UI Elements
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
        std::string eventTypeStrBuf { eventTypeBuf };

        for(auto& c : eventTypeStrBuf)
            c = toupper(c);

        if(eventTypeStrBuf != "NEW EVENT TYPE" && verifyEventType(eventTypeStrBuf)){
            returnVal = EventType {eventTypeStrBuf};    // Set return value
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
    if(!ImGui::BeginChild("Add New Location")){
        ImGui::EndChild();
        return Location { };
    }

    static char locationBuf[UI_SETTINGS.MAX_TEXT_INPUT_CHARS] = "New Location";

    Location returnVal;

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
        std::string locationStrBuf  { locationBuf };

        if(locationStrBuf != "New Location" && verifyLocation(locationStrBuf)){
            returnVal = Location{locationStrBuf};   // Set return value
            strcpy(locationBuf, "New Location");    // Reset buffers
            unsavedChanges = true;                  // Set flag
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
// MARK: Verify Items
bool    EventsUI::verifyEventType     (const std::string& name){
    // Verify characters are valid
    for(auto& c : name){
        if(!isalnum(c) && c != ' ')
            return false;
    }

    return true;
}
bool    EventsUI::verifyLocation      (const std::string& name){
    // Verify characters are valid
    for(const auto& c : name){
        if(!isalnum(c) && c != ' ')
            return false;
    }

    return false;
}
// MARK: Event Gun Table
void EventsUI::eventGunTable(const std::vector<std::pair<ConstGunPtr, ConstTrackedAmmoPtr>>& list, bool showAmmoUsed) {
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

        for(const auto& [gun, ammo] : list){
            if(*gun != emptyGun){
                ImGui::TableNextRow();
                for (int column{0}; column < tableColumns; ++column){
                    ImGui::TableSetColumnIndex(column);
                    switch( column ){
                        case 0:
                            ImGui::Text("%s", gun->getWeaponType().getName().c_str());
                            break;
                        case 1:
                            ImGui::Text("%s", gun->getCartridge().getName().c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", gun->getName().c_str());
                            break;
                        case 3:
                            ImGui::Text("%s", ammo->ammoType.name.c_str());
                            break;
                        case 4:
                            ImGui::Text("%s", ammo->ammoType.manufacturer.getName().c_str());
                            break;
                        case 5:
                            ImGui::Text("%d", ammo->ammoType.grainWeight);
                            break;
                        case 6:
                            ImGui::Text("%d", ammo->amount);
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
void EventsUI::selectGunTable (const ConstGunPtrList& list, Gun& selectedGun){

    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(tableSize.x < 400)
        tableSize = ImVec2{400, 200};

    int row { 0 };
    if(ImGui::BeginTable("Gun Table", 3, 
                                ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, tableSize
                        ))
    {
        ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();            

        for(const auto& gunElement : list){
            const Gun& gun { *gunElement };
            bool isGunSelected { false };
            
            if(*gunElement == selectedGun)
                isGunSelected = true;
            
            ImGui::PushID(std::to_string(row).c_str()); // Needed to ensure no name collisions for selectable
            
            ImGui::TableNextRow();
            for (int column{0}; column < 3; ++column) {
                ImGui::TableSetColumnIndex(column);

                switch( column ){
                    case 0:
                        ImGui::Selectable(gun.getName().c_str(), &isGunSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isGunSelected)
                            selectedGun = gun;
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
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
   
    ImGui::Spacing();
}

// MARK: Select Ammo Table
void EventsUI::selectAmmoTable(const ConstTrackedAmmoPtrList& list, TrackedAmmo& selectedAmmo){
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(tableSize.x < 400)
        tableSize = ImVec2{400, 200};

    int row { 0 };
    if(ImGui::BeginTable("Ammo Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableRowFlags_Headers | ImGuiTableFlags_HighlightHoveredColumn, tableSize )) {
        ImGui::TableSetupColumn("Cartridge",    0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Grain Weight", 0);
        ImGui::TableHeadersRow();            

            for(const auto& ammoElement : list){
            const AmmoType& ammoType {ammoElement->ammoType};           
            bool isAmmoSelected { false };
            
            if(ammoType == selectedAmmo.ammoType)
                isAmmoSelected = true;

            ImGui::PushID(std::to_string(row).c_str());

            ImGui::TableNextRow();
            for (int column{0}; column < 4; ++column) {
                ImGui::TableSetColumnIndex(column);

                switch( column ){
                    case 0:
                        ImGui::Selectable(ammoType.cartridge.getName().c_str(), &isAmmoSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isAmmoSelected)
                            selectedAmmo.ammoType = ammoElement->ammoType;

                        break;
                    case 1:
                        ImGui::Text("%s", ammoType.name.c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", ammoType.manufacturer.getName().c_str());
                        break;
                    case 3:
                        ImGui::Text("%d", ammoType.grainWeight);
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
    ImGui::Spacing();
}

