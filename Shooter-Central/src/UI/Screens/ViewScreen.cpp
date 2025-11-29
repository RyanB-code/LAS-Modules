#include "ViewScreen.h"

namespace ShooterCentral::UI {

void View::main(const Database& database, ScreenData::View& data){

    ImGui::Spacing();
    ImGui::Spacing();

    centerNextComboBoxX("Select A Category", 200);
    ComboBoxes::category(data.category); 

    ImGui::Spacing();
    ImGui::Spacing();

    // Show the necessary window herew
    switch(data.category){
        case Category::GUNS:
            View::gunTab(database.getArmory(), data.gunTab);
            break;
        case Category::EVENTS:
            View::eventsTab(database.getEvents(), data.eventTab);
            break;
        case Category::STOCKPILE:
            View::stockpileTab(database.getStockpile(), database.getAmountPerCartridge(), data.stockpileTab); 
            break;
        default:
           
            break;
    }
}
void View::gunTab  (
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& guns, 
        ScreenData::View::GunTab& data 
    )
{
    static constexpr float MIN_BOTTOM_WIN_SIZE_X { 400 };
    static constexpr float MIN_BOTTOM_WIN_SIZE_Y { 600 };

    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    bool verticalLayout = false;

    ImVec2 topWindowSize;
    ImVec2 bottomWindowSize;
    ImVec2 topTableSize;
    ImVec2 bottomTableSize;

    if(ImGui::GetContentRegionAvail().x < (MIN_BOTTOM_WIN_SIZE_X * 3)){
        verticalLayout = true;

        topWindowSize  = {MIN_BOTTOM_WIN_SIZE_X * 3, MIN_BOTTOM_WIN_SIZE_X * 3};
        bottomWindowSize = topWindowSize;
    }
    else{
        topWindowSize  = {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2};
        if(topWindowSize.x < MIN_BOTTOM_WIN_SIZE_X)
            topWindowSize.x = MIN_BOTTOM_WIN_SIZE_X;
        if(topWindowSize.y < MIN_BOTTOM_WIN_SIZE_Y)
            topWindowSize.y = MIN_BOTTOM_WIN_SIZE_Y;

        bottomWindowSize =  { ImGui::GetContentRegionAvail().x / 3, ImGui::GetContentRegionAvail().y / 2};
        if(bottomWindowSize.x < MIN_BOTTOM_WIN_SIZE_X)
            bottomWindowSize.x = MIN_BOTTOM_WIN_SIZE_X;
        if(bottomWindowSize.y < MIN_BOTTOM_WIN_SIZE_Y)
            bottomWindowSize.y = MIN_BOTTOM_WIN_SIZE_Y;
    }


    // Size the table
    topTableSize = { topWindowSize.x-2, 400};
    if(topTableSize.x < MIN_TABLE_SIZE_X)
        topTableSize.x = MIN_TABLE_SIZE_X;
    if(topTableSize.x > MAX_TABLE_SIZE_X)
        topTableSize.x = MAX_TABLE_SIZE_X;

    bottomTableSize = { bottomWindowSize.x-2, 400};
    if(bottomTableSize.x < MIN_TABLE_SIZE_X)
        bottomTableSize.x = MIN_TABLE_SIZE_X;
    if(bottomTableSize.x > MAX_TABLE_SIZE_X)
        bottomTableSize.x = MAX_TABLE_SIZE_X;


    // Button size
    static const ImVec2 buttonSize { 100, 40 };

    // AFTER the select table is run, use this for further checking if selected is valid
    bool selectedGunValid { data.selectedGun != EMPTY_GUN_METADATA };

    if(ImGui::BeginChild("View Gun Table", topWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::SeparatorText( "Select A Gun" );
        ImGui::Spacing();

        if(!selectedGunValid)
            ImGui::BeginDisabled();

        centerNextItemX(buttonSize.x);
        if(ImGui::Button("Deselect", buttonSize))
            data.selectedGun = EMPTY_GUN_METADATA;

        if(!selectedGunValid)
            ImGui::EndDisabled();
        
        ImGui::Spacing();
        ImGui::Spacing();

        centerNextItemX(topTableSize.x);
        Tables::Selectable::gunMetadataWithRoundCount(guns, data.selectedGun, topTableSize);

        selectedGunValid = data.selectedGun != EMPTY_GUN_METADATA;
    }
    ImGui::EndChild();

    if(ImGui::BeginChild("Selected Gun Details", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Details" );
        ImGui::Spacing();

        if(!selectedGunValid){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Detailed Information");
        }
        else
            View::gunTab_selectedGunInformation(guns.at(data.selectedGun.cartridge).at(data.selectedGun));
    }
    ImGui::EndChild();

    if(!verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Selected Gun Events", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Events Used" );
        ImGui::Spacing();

        if(!selectedGunValid){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Events");
        }
        else
            View::gunTab_eventsWindow(guns.at(data.selectedGun.cartridge).at(data.selectedGun), data.selectedEvent);    
    }
    ImGui::EndChild();

    if(!verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Selected Gun Ammo Used", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Ammo Used" );
        ImGui::Spacing();

        if(!selectedGunValid){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to Ammo Used");
        }
        else
            View::gunTab_ammoUsedWindow(guns.at(data.selectedGun.cartridge).at(data.selectedGun)); 
    }
    ImGui::EndChild();

}
void View::gunTab_selectedGunInformation(const ArmoryGun& gun){
    const GunMetadata& gunInfo  { gun.getGunInfo() };

    centerNextItemX(200);
    ImGui::BeginGroup();
    ImGui::Text("Name:              %s", gunInfo.name.c_str());
    ImGui::Text("Weapon Type:       %s", gunInfo.weaponType.getName()); 
    ImGui::Text("Cartridge:         %s", gunInfo.cartridge.getName());
    ImGui::Text("Round Count:       %d", gun.getRoundCount());

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Events Used In:    %d", gun.totalEventsUsed()); 
    ImGui::Text("Ammo Types Used:   %d", gun.totalAmmoTypesUsed()); 
    ImGui::EndGroup();
}
void View::gunTab_eventsWindow(const ArmoryGun& gun, ShootingEventMetadata& selectedEvent){
    const GunMetadata& gunInfo  { gun.getGunInfo() };

    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    ImVec2 tableSize  { ImGui::GetContentRegionAvail().x-2, 400};
    if(tableSize.x < MIN_TABLE_SIZE_X)
        tableSize.x = MIN_TABLE_SIZE_X;
    if(tableSize.x > MAX_TABLE_SIZE_X)
        tableSize.x = MAX_TABLE_SIZE_X;

    centerNextItemX(tableSize.x);
    Tables::Selectable::eventMetadata(gun.getEventsUsed(), selectedEvent, tableSize);

    // TODO - Button to view selected ammo here
    if(selectedEvent != EMPTY_EVENT_METADATA){
        std::cout << "Command to view event\n"; // Make command to enter change to view event with this Event
        selectedEvent = EMPTY_EVENT_METADATA;
    }
    return;
}
void View::gunTab_ammoUsedWindow(const ArmoryGun& gun){
    const GunMetadata& gunInfo  { gun.getGunInfo() };
    static AmmoMetadata selectedAmmo { };

    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    ImVec2 tableSize  { ImGui::GetContentRegionAvail().x-2, 400};
    if(tableSize.x < MIN_TABLE_SIZE_X)
        tableSize.x = MIN_TABLE_SIZE_X;
    if(tableSize.x > MAX_TABLE_SIZE_X)
        tableSize.x = MAX_TABLE_SIZE_X;

    centerNextItemX(tableSize.x);
    Tables::Selectable::amountOfAmmo(gun.getAmmoUsed(), selectedAmmo, tableSize);
    // TODO - button to view selected ammo here
}
void View::eventsTab(
        const std::map<ShootingEventMetadata, ShootingEvent>& events, 
        ScreenData::View::EventTab& data 
){
    static constexpr float MIN_WIN_BOTTOM_SIZE_X { 400 };
    static constexpr float MIN_WIN_BOTTOM_SIZE_Y { 600 };

    static constexpr float MIN_WIN_BOTTOM_INFO_SIZE_X { 200 };
    static constexpr float MIN_WIN_BOTTOM_INFO_SIZE_Y { 600 };

    static constexpr float MIN_WIN_TOP_SIZE_X { (MIN_WIN_BOTTOM_SIZE_X * 2) + MIN_WIN_BOTTOM_INFO_SIZE_X };

    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    bool verticalLayout = false;

    ImVec2 topWindowSize;
    ImVec2 bottomWindowSize;
    ImVec2 infoWindowSize;
    ImVec2 topTableSize;
    ImVec2 bottomTableSize;

    if(ImGui::GetContentRegionAvail().x < MIN_WIN_TOP_SIZE_X){
        verticalLayout = true;

        topWindowSize  = { MIN_WIN_TOP_SIZE_X, MIN_WIN_BOTTOM_SIZE_X };
        bottomWindowSize = topWindowSize;
        infoWindowSize = topWindowSize;
    }
    else{
        topWindowSize  = {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2};
        if(topWindowSize.x < MIN_WIN_TOP_SIZE_X)
            topWindowSize.x = MIN_WIN_TOP_SIZE_X;
        if(topWindowSize.y < MIN_WIN_BOTTOM_SIZE_Y)
            topWindowSize.y = MIN_WIN_BOTTOM_SIZE_Y;

        bottomWindowSize =  { (ImGui::GetContentRegionAvail().x / 3) * 2, ImGui::GetContentRegionAvail().y / 2};
        if(bottomWindowSize.x < MIN_WIN_BOTTOM_SIZE_X)
            bottomWindowSize.x = MIN_WIN_BOTTOM_SIZE_X;
        if(bottomWindowSize.y < MIN_WIN_BOTTOM_SIZE_Y)
            bottomWindowSize.y = MIN_WIN_BOTTOM_SIZE_Y;

        infoWindowSize = {ImGui::GetContentRegionAvail().x / 3, ImGui::GetContentRegionAvail().y / 2};
        if(infoWindowSize.x < MIN_WIN_BOTTOM_INFO_SIZE_X)
            infoWindowSize.x = MIN_WIN_BOTTOM_INFO_SIZE_X;
        if(infoWindowSize.y < MIN_WIN_BOTTOM_INFO_SIZE_Y)
            infoWindowSize.y = MIN_WIN_BOTTOM_INFO_SIZE_Y;
    }


    // Size the table
    topTableSize = { topWindowSize.x-2, 400};
    if(topTableSize.x < MIN_TABLE_SIZE_X)
        topTableSize.x = MIN_TABLE_SIZE_X;
    if(topTableSize.x > MAX_TABLE_SIZE_X)
        topTableSize.x = MAX_TABLE_SIZE_X;

    bottomTableSize = { bottomWindowSize.x-2, 400};
    if(bottomTableSize.x < MIN_TABLE_SIZE_X)
        bottomTableSize.x = MIN_TABLE_SIZE_X;
    if(bottomTableSize.x > MAX_TABLE_SIZE_X)
        bottomTableSize.x = MAX_TABLE_SIZE_X;


    // Button size
    static const ImVec2 buttonSize { 100, 40 };
    bool selectedEventValid { data.selectedEvent != EMPTY_EVENT_METADATA };

    if(ImGui::BeginChild("View Event Table", topWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::SeparatorText( "Select An Event" );
        ImGui::Spacing();

        if(!selectedEventValid)
            ImGui::BeginDisabled();

        centerNextItemX(buttonSize.x);
        if(ImGui::Button("Deselect", buttonSize))
            data.selectedEvent = EMPTY_EVENT_METADATA;

        if(!selectedEventValid)
            ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Spacing();

        centerNextItemX(topTableSize.x);
        Tables::Selectable::eventsWithGunsUsed(events, data.selectedEvent, topTableSize);
        selectedEventValid = data.selectedEvent != EMPTY_EVENT_METADATA;
    }
    ImGui::EndChild();

    if(ImGui::BeginChild("Selected Event Details", infoWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Details" );
        ImGui::Spacing(); 

        if(!selectedEventValid){
            centerNextItemY(5);
            centerTextDisabled("Select an Event to View Detailed Information");
        }
        else
            View::eventsTab_selectedEventInformation(events.at(data.selectedEvent));
    }
    ImGui::EndChild();

    if(!verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Selected Event Guns Used", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Guns Used" );
        ImGui::Spacing(); 

        if(!selectedEventValid) {
            centerNextItemY(5);
            centerTextDisabled("Select an Event to View Detailed Information");
        }
        else
            View::eventsTab_gunsUsed(events.at(data.selectedEvent), data.selectedGunAmmoUsed);
    }
    ImGui::EndChild();

}
void View::eventsTab_selectedEventInformation(const ShootingEvent& event){
    const ShootingEventMetadata& info  { event.getInfo() };

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
    ImGui::TextWrapped("%s", info.notes.c_str());
}
void View::eventsTab_gunsUsed(const ShootingEvent& event, GunTrackingAmmoUsed& selectedGun){
    const ShootingEventMetadata& info  { event.getInfo() };

    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    ImVec2 tableSize = { ImGui::GetContentRegionAvail().x-2, 400};
    if(tableSize.x < MIN_TABLE_SIZE_X)
        tableSize.x = MIN_TABLE_SIZE_X;
    if(tableSize.x > MAX_TABLE_SIZE_X)
        tableSize.x = MAX_TABLE_SIZE_X;

    ImGui::Spacing();
    centerTextDisabled("Select a Gun to View Ammo Used");
    ImGui::Spacing();

    centerNextItemX(tableSize.x);
    Tables::Selectable::gunMetadataWithRoundCount(event.getGunsUsed(), selectedGun, tableSize);

    if(selectedGun.getGunInfo() == EMPTY_GUN_METADATA)
        return;

    ImGui::Spacing();
    ImGui::SeparatorText("Ammo Used");
    ImGui::Spacing();

    centerNextItemX(tableSize.x);
    Tables::amountOfAmmo(selectedGun.getAmmoUsed(), tableSize);

    // TODO - make this appear like the add Gun and Events in side by side mode 
}
void View::stockpileTab(
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList, 
        const std::map<Cartridge, int>& cartridgeList, 
        ScreenData::View::StockpileTab& tabData 
    )
{
    static constexpr float MIN_WIN_BOTTOM_SIZE_X { 400 };
    static constexpr float MIN_WIN_BOTTOM_SIZE_Y { 600 };

    static constexpr float MIN_WIN_BOTTOM_INFO_SIZE_X { 200 };
    static constexpr float MIN_WIN_BOTTOM_INFO_SIZE_Y { 600 };

    static constexpr float MIN_WIN_TOP_SIZE_X { (MIN_WIN_BOTTOM_SIZE_X * 2) + MIN_WIN_BOTTOM_INFO_SIZE_X };

    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    // Button size
    static const ImVec2 buttonSize { 100, 40 };

    bool verticalLayout = false;

    ImVec2 topWindowSize;
    ImVec2 bottomWindowSize;
    ImVec2 infoWindowSize;
    ImVec2 topTableSize;
    ImVec2 bottomTableSize;

    if(ImGui::GetContentRegionAvail().x < MIN_WIN_TOP_SIZE_X){
        verticalLayout = true;

        topWindowSize  = { MIN_WIN_TOP_SIZE_X, MIN_WIN_BOTTOM_SIZE_X };
        bottomWindowSize = topWindowSize;
        infoWindowSize = topWindowSize;
    }
    else{
        topWindowSize  = {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2};
        if(topWindowSize.x < MIN_WIN_TOP_SIZE_X)
            topWindowSize.x = MIN_WIN_TOP_SIZE_X;
        if(topWindowSize.y < MIN_WIN_BOTTOM_SIZE_Y)
            topWindowSize.y = MIN_WIN_BOTTOM_SIZE_Y;

        bottomWindowSize =  { (ImGui::GetContentRegionAvail().x / 3) * 2, ImGui::GetContentRegionAvail().y / 2};
        if(bottomWindowSize.x < MIN_WIN_BOTTOM_SIZE_X)
            bottomWindowSize.x = MIN_WIN_BOTTOM_SIZE_X;
        if(bottomWindowSize.y < MIN_WIN_BOTTOM_SIZE_Y)
            bottomWindowSize.y = MIN_WIN_BOTTOM_SIZE_Y;

        infoWindowSize = {ImGui::GetContentRegionAvail().x / 3, ImGui::GetContentRegionAvail().y / 2};
        if(infoWindowSize.x < MIN_WIN_BOTTOM_INFO_SIZE_X)
            infoWindowSize.x = MIN_WIN_BOTTOM_INFO_SIZE_X;
        if(infoWindowSize.y < MIN_WIN_BOTTOM_INFO_SIZE_Y)
            infoWindowSize.y = MIN_WIN_BOTTOM_INFO_SIZE_Y;
    }


    // Size the table
    topTableSize = { topWindowSize.x-2, 400};
    if(topTableSize.x < MIN_TABLE_SIZE_X)
        topTableSize.x = MIN_TABLE_SIZE_X;
    if(topTableSize.x > MAX_TABLE_SIZE_X)
        topTableSize.x = MAX_TABLE_SIZE_X;

    bottomTableSize = { bottomWindowSize.x-2, 400};
    if(bottomTableSize.x < MIN_TABLE_SIZE_X)
        bottomTableSize.x = MIN_TABLE_SIZE_X;
    if(bottomTableSize.x > MAX_TABLE_SIZE_X)
        bottomTableSize.x = MAX_TABLE_SIZE_X;


    static std::string comboText { };
    static Cartridge lastCartridge { };

    if(lastCartridge != tabData.selectedCartridge)
        tabData.selectedAmmo = EMPTY_AMMO_METADATA;

    lastCartridge = tabData.selectedCartridge;


    bool selectedCartridgeValid { tabData.selectedCartridge != EMPTY_CARTRIDGE };
    bool selectedAmmoValid      { tabData.selectedAmmo != EMPTY_AMMO_METADATA };

    if(!selectedCartridgeValid)
        comboText = "Select A Cartridge";
    else
        comboText = tabData.selectedCartridge.getName();

 
    if(ImGui::BeginChild("Select Ammo Tab", topWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::Spacing();
        ImGui::Spacing();

        centerNextItemX(300);
        ImGui::BeginGroup();
        ImGui::Text("Select Cartridge"); 
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        
        if (ImGui::BeginCombo("##Cartridge Select Combo", comboText.c_str(), ImGuiComboFlags_HeightSmall)) {
            for (const auto& [cartridge, amount] : cartridgeList) {
                const bool isSelected = (tabData.selectedCartridge == cartridge);

                if(ImGui::Selectable(cartridge.getName(), isSelected))
                    tabData.selectedCartridge = cartridge;

                if(isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::EndGroup();

        selectedCartridgeValid = tabData.selectedCartridge != EMPTY_CARTRIDGE;

        ImGui::Spacing();
        ImGui::Spacing();

        if(!selectedAmmoValid)
            ImGui::BeginDisabled();

        centerNextItemX(buttonSize.x);
        if(ImGui::Button("Deselect", buttonSize))
            tabData.selectedAmmo = EMPTY_AMMO_METADATA;

        if(!selectedAmmoValid)
            ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Spacing();

        if(!selectedCartridgeValid) {
            centerNextItemY(5);
            centerTextDisabled("Select a Cartridge to See Ammo On Hand");
        }
        else{
            centerNextItemX(topTableSize.x);
            Tables::Selectable::ammoAmountOnHand(ammoList.at(tabData.selectedCartridge), tabData.selectedAmmo, topTableSize);
        }
        selectedAmmoValid = tabData.selectedAmmo != EMPTY_AMMO_METADATA;
    }
    ImGui::EndChild();



    if(ImGui::BeginChild("Selected Ammo Details", infoWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Details" );
        ImGui::Spacing(); 

        if(!selectedAmmoValid){
            centerNextItemY(5);
            centerTextDisabled("Select Ammo to View Detailed Information");
        }
        else
            View::stockpileTab_selectedAmmoInformation(ammoList.at(tabData.selectedAmmo.cartridge).at(tabData.selectedAmmo));
    }
    ImGui::EndChild();

    if(!verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Selected Ammo Guns Used", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Guns Used" );
        ImGui::Spacing(); 

        if(!selectedAmmoValid) {
            centerNextItemY(5);
            centerTextDisabled("Select an Event to View Detailed Information");
        }
        else
            View::stockpileTab_selectedAmmoGunsUsed(ammoList.at(tabData.selectedAmmo.cartridge).at(tabData.selectedAmmo).getGunsUsed());
    }
    ImGui::EndChild();


}
void View::stockpileTab_selectedAmmoInformation (const StockpileAmmo& data){
    const AmmoMetadata& ammoInfo  { data.getAmmoInfo() };

    centerNextItemX(200);
    ImGui::BeginGroup();
    ImGui::Text("Name:              %s", ammoInfo.name.c_str());
    ImGui::Text("Cartridge:         %s", ammoInfo.cartridge.getName());
    ImGui::Text("Manufacturer:      %s", ammoInfo.manufacturer.getName()); 
    ImGui::Text("GrainWeight:       %d", ammoInfo.grainWeight);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Amount On Hand:    %d", data.getAmountOnHand()); 
    ImGui::Text("Guns Used In:      %ld", data.getGunsUsed().size()); 
    ImGui::EndGroup();
}
void View::stockpileTab_selectedAmmoGunsUsed (const std::set<GunMetadata>& gunsUsed){
    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    ImVec2 tableSize = { ImGui::GetContentRegionAvail().x-2, 400};
    if(tableSize.x < MIN_TABLE_SIZE_X)
        tableSize.x = MIN_TABLE_SIZE_X;
    if(tableSize.x > MAX_TABLE_SIZE_X)
        tableSize.x = MAX_TABLE_SIZE_X;

    GunMetadata selected { };

    centerNextItemX(tableSize.x);
    Tables::Selectable::gunMetadata(gunsUsed, selected, tableSize);

    if(selected != EMPTY_GUN_METADATA)
        std::cout << "Command move to view this gun\n";
}

}   // End SC::UI
