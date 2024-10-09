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

    if(!ammoTracker || !gunTracker){
        ImGui::Text("This module has not been setup yet...");
        ImGui::End();
        return;
    }

    if(ImGui::BeginTabBar("Shooter Central Tabs",   ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |
                                                    ImGuiTabBarFlags_DrawSelectedOverline | ImGuiTabBarFlags_FittingPolicyResizeDown
    )){
        if(ImGui::BeginTabItem("Armory")){
            drawArmory();
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Stockpile")){
            drawStockpile();
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

// MARK: PRIVATE FUNCTIONS

// MARK: STOCKPILE
void ShooterCentralWindow::drawStockpile() const{
    if(ImGui::BeginChild("Stockpile", ImVec2{500, 250})){

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
    ImGui::EndChild();
}
// MARK: ARMORY
void ShooterCentralWindow::drawArmory() const{
    if(ImGui::BeginChild("Armory", ImVec2{500, 250})){       
        static std::vector<Gun> gunList;

        gunTracker->getAllGuns(gunList);

        ImGui::SeparatorText("Armory");

        static int gunNum { 0 };
        if(ImGui::Button("Add gun")){
            std::ostringstream name;
            name << "Gun " << gunNum;
            AmmoType ammoType1 {"Test name", "Test man", "Test Cart", 69};
            AmmoType ammoType2 {"Test name2", "Test man", "Test Cart", 50};

            GunPtr gunBuf { gunTracker->createPistol(name.str(), "test cartridge")};

            if(gunBuf){
                gunBuf->addToRoundCount(15, ammoType1);
                gunBuf->addToRoundCount(20, ammoType2);

                ++gunNum;
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("Write guns")){
            gunTracker->writeAllGuns();
        }
        ImGui::SameLine();
        if(ImGui::Button("Read guns")){
            gunTracker->readGuns();
        }

        ImGui::Text("Guns tracked %lu", gunTracker->getGunTotal());

        if(ImGui::BeginTable("Gun Table", 4, 
                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                    ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_HighlightHoveredColumn |
                                    ImGuiTableFlags_NoHostExtendX
                            ))
        {
            ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None, 100);
            ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None, 100);
            ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None, 100);
            ImGui::TableSetupColumn("Round Count",  ImGuiTableColumnFlags_None, 100);

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
    ImGui::EndChild();
}