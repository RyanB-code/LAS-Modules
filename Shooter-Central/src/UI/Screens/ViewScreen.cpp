#include "ViewScreen.h"

namespace ShooterCentral::UI::View {

void main(const Database& database, ScreenData::View& data){

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    centerNextComboBoxX("Select A Category", 200);
    ComboBoxes::category(data.category); 

    ImGui::Spacing();
    ImGui::Spacing();

    switch(data.category){
        case Category::GUNS:
            ArmoryWindow::main(database.getArmory(), data.armoryWindow);
            break;
        case Category::EVENTS:
            EventsWindow::main(database.getEvents(), data.eventsWindow);
            break;
        case Category::STOCKPILE:
            StockpileWindow::main(
                    database.getStockpile(), 
                    database.getAmountPerCartridge(), 
                    data.stockpileWindow
                ); 
            break;
        default:
           
            break;
    }
}





void ArmoryWindow::main(
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory, 
        ScreenData::View::ArmoryWindow& data 
    )
{
    if(ImGui::GetContentRegionAvail().x < (data.minWinSize.x * 3) ){
        data.verticalLayout = true;

        data.topWindowSize  = ImGui::GetContentRegionAvail();
        data.bottomWindowSize = data.topWindowSize;
    }
    else{
        data.bottomWindowSize =  ImVec2{ (ImGui::GetContentRegionAvail().x / 3 ), ImGui::GetContentRegionAvail().y / 2};
        data.topWindowSize =     ImVec2{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2};

        data.verticalLayout = false;
    }

    // Ensure minimum windows
    if(data.topWindowSize.x < data.minWinSize.x)
        data.topWindowSize.x = data.minWinSize.x;
    if(data.topWindowSize.y < data.minWinSize.y)
        data.topWindowSize.y = data.minWinSize.y;

    if(data.bottomWindowSize.x < data.minWinSize.x)
        data.bottomWindowSize.x = data.minWinSize.x;
    if(data.bottomWindowSize.y < data.minWinSize.y)
        data.bottomWindowSize.y = data.minWinSize.y;


    selectGunWindow(
            armory,
            data.selectedGun,
            data.topTableSize,
            data.selectedGunValid,
            data.showInactive,
            data.minTableWidth,
            data.maxTableWidth,
            data.topWindowSize,
            data.deselectButtonSize
        );

    // Now starts bottom Windows \\

    selectedGunInfoWindow(
            armory,
            data.selectedGun,
            data.selectedGunValid,
            data.bottomWindowSize
        ); 
 
    if(!data.verticalLayout)
        ImGui::SameLine();

    selectedGunEventsWindow(
            armory,
            data.selectedGun,
            data.bottomTableSize,
            data.selectedGunValid,
            data.minTableWidth,
            data.maxTableWidth,
            data.bottomWindowSize
        );
    
    if(!data.verticalLayout)
        ImGui::SameLine();

    selectedGunAmmoWindow(
            armory,
            data.selectedGun,
            data.bottomTableSize,
            data.selectedGunValid,
            data.minTableWidth,
            data.maxTableWidth,
            data.bottomWindowSize
        ); 
}
void ArmoryWindow::selectGunWindow(
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
        GunMetadata& selectedGun,
        ImVec2& tableSize,
        bool& isGunValid,
        bool& showInactive,
        float minTableWidth,
        float maxTableWidth,
        const ImVec2& windowSize,
        const ImVec2& buttonSize
    )
{
    isGunValid = selectedGun != EMPTY_GUN_METADATA;

    if(ImGui::BeginChild("Select A Gun", windowSize)){ 
        ImGui::SeparatorText( "Select A Gun" );
        ImGui::Spacing();
        ImGui::Spacing();

        centerNextItemX(350);
        ImGui::BeginGroup();
        ImGui::Text("Show Inactive Guns");
        ImGui::SameLine();
        ImGui::Checkbox("##Show Inactive", &showInactive);

        ImGui::SameLine();
        ImGui::Dummy( ImVec2 { 50, 0 } );
        ImGui::SameLine();

        if(!isGunValid)
            ImGui::BeginDisabled();

        if(ImGui::Button("Deselect", buttonSize))
            selectedGun = EMPTY_GUN_METADATA;

        if(!isGunValid)
            ImGui::EndDisabled();

        ImGui::EndGroup();
        
        ImGui::Spacing();
        ImGui::Spacing();

        tableSize.x = ImGui::GetContentRegionAvail().x;
        if(tableSize.x < minTableWidth)
            tableSize.x = minTableWidth;
        if(tableSize.x > maxTableWidth)
            tableSize.x = maxTableWidth;

        centerNextItemX(tableSize.x);
        Tables::Selectable::gunMetadataWithRoundCount(armory, selectedGun, tableSize, showInactive);
        isGunValid = selectedGun != EMPTY_GUN_METADATA;
    }
    ImGui::EndChild();
}

void ArmoryWindow::selectedGunInfoWindow(
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
        const GunMetadata& gunInfo,
        const bool& isGunValid,
        const ImVec2& windowSize
    )
{
    
    if(ImGui::BeginChild("Selected Gun Details", windowSize)){
        ImGui::SeparatorText( "Details" );
        ImGui::Spacing();
        ImGui::Spacing();

        if(!isGunValid){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Detailed Information");
        }
        else{ 
            const ArmoryGun& gun { armory.at(gunInfo.cartridge).at(gunInfo) };

            centerNextItemX(200);
            ImGui::BeginGroup();
            ImGui::TextDisabled("Name:            ");
            ImGui::SameLine();
            ImGui::Text("%s", gunInfo.name.c_str());


            ImGui::TextDisabled("Weapon Type:     ");
            ImGui::SameLine();
            ImGui::Text("%s", gunInfo.weaponType.getName()); 

            ImGui::TextDisabled("Cartridge:       ");
            ImGui::SameLine();
            ImGui::Text("%s", gunInfo.cartridge.getName());

            ImGui::TextDisabled("Round Count:     ");
            ImGui::SameLine();
            ImGui::Text("%d", gun.getRoundCount());

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::TextDisabled("Events Used In:  ");
            ImGui::SameLine();
            ImGui::Text("%d", gun.totalEventsUsed()); 

            ImGui::TextDisabled("Ammo Types Used: ");
            ImGui::SameLine();
            ImGui::Text("%d", gun.totalAmmoTypesUsed()); 

            ImGui::EndGroup();

        }
    }
    ImGui::EndChild();
}
void ArmoryWindow::selectedGunEventsWindow(
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
        const GunMetadata& selectedGun,
        ImVec2& tableSize,
        const bool isGunValid,
        float minTableWidth,
        float maxTableWidth,
        const ImVec2& windowSize
    )
{
    if(ImGui::BeginChild("Selected Gun Events", windowSize)){
        ImGui::SeparatorText( "Events Used" );
        ImGui::Spacing();
        ImGui::Spacing();

        if(!isGunValid){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Events");
        }
        else{
            centerTextDisabled("Select Event to View Details");
            ImGui::Spacing();
            ImGui::Spacing();

            tableSize.x = ImGui::GetContentRegionAvail().x -2;
            if(tableSize.x < minTableWidth)
                tableSize.x = minTableWidth;
            if(tableSize.x > maxTableWidth)
                tableSize.x = maxTableWidth;

            static ShootingEventMetadata viewEvent { };
            centerNextItemX(tableSize.x);
            Tables::Selectable::eventMetadata(
                    armory.at(selectedGun.cartridge).at(selectedGun).getEventsUsed(),
                    viewEvent,
                    tableSize
                );

            if(viewEvent != EMPTY_EVENT_METADATA){
                ScreenData::View buffer { };
                buffer.eventsWindow.selectedEvent = viewEvent;
                buffer.category = Category::EVENTS;

                UIEvents::SetScreenData::View setView { buffer};
                UIEvents::SetScreen setScreen { Screen::VIEW };
                
                pushEvent(&setView);
                pushEvent(&setScreen);

                viewEvent = EMPTY_EVENT_METADATA;
            }
        }
    }
    ImGui::EndChild();
}
void ArmoryWindow::selectedGunAmmoWindow(
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
        const GunMetadata& selectedGun,
        ImVec2& tableSize,
        const bool isGunValid,
        float minTableWidth,
        float maxTableWidth,
        const ImVec2& windowSize
    )
{
    if(ImGui::BeginChild("Selected Gun Ammo Used", windowSize)){
        ImGui::SeparatorText( "Ammo Used" );
        ImGui::Spacing();
        ImGui::Spacing();

        if(!isGunValid){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Ammo Used");
        }
        else{
            centerTextDisabled("Select Ammo to View Details");
            ImGui::Spacing();
            ImGui::Spacing();

            static AmmoMetadata viewAmmo { };
            centerNextItemX(tableSize.x);
            Tables::Selectable::amountOfAmmo(
                    armory.at(selectedGun.cartridge).at(selectedGun).getAmmoUsed(),
                    viewAmmo, 
                    tableSize);

            if(viewAmmo != EMPTY_AMMO_METADATA){
                ScreenData::View buffer { };
                buffer.stockpileWindow.selectedCartridge = viewAmmo.cartridge;
                buffer.stockpileWindow.selectedAmmo = viewAmmo;
                buffer.category = Category::STOCKPILE;

                UIEvents::SetScreenData::View setView { buffer};
                UIEvents::SetScreen setScreen { Screen::VIEW };
                
                pushEvent(&setView);
                pushEvent(&setScreen);

                viewAmmo = EMPTY_AMMO_METADATA;
            }
        }
    }
    ImGui::EndChild();
}




void EventsWindow::main(
        const std::map<ShootingEventMetadata, ShootingEvent>& events, 
        ScreenData::View::EventsWindow& data 
    )
{
    if(ImGui::GetContentRegionAvail().x < (data.minWinSize.x * 3) ){
        data.verticalLayout = true;

        data.topWindowSize  = ImGui::GetContentRegionAvail();
        data.bottomWindowSize = data.topWindowSize;
    }
    else{
        data.bottomWindowSize =  ImVec2{ (ImGui::GetContentRegionAvail().x / 3 ), ImGui::GetContentRegionAvail().y / 2};
        data.topWindowSize =     ImVec2{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2};

        data.verticalLayout = false;
    }

    // Ensure minimum windows
    if(data.topWindowSize.x < data.minWinSize.x)
        data.topWindowSize.x = data.minWinSize.x;
    if(data.topWindowSize.y < data.minWinSize.y)
        data.topWindowSize.y = data.minWinSize.y;

    if(data.bottomWindowSize.x < data.minWinSize.x)
        data.bottomWindowSize.x = data.minWinSize.x;
    if(data.bottomWindowSize.y < data.minWinSize.y)
        data.bottomWindowSize.y = data.minWinSize.y;

    selectEventWindow(
            events,
            data.selectedEvent,
            data.selectedGun,
            data.topTableSize,
            data.selectedEventValid,
            data.minTableWidth,
            data.maxTableWidth,
            data.topWindowSize,
            data.deselectButtonSize
        );

    
    static ShootingEventMetadata lastEvent { };
    if(data.selectedEvent != lastEvent){
        data.selectedGun = EMPTY_GUN_METADATA;
    }
    lastEvent = data.selectedEvent;

    selectedEventInfoWindow(data.selectedEvent, data.selectedEventValid, data.bottomWindowSize);
 
    if(!data.verticalLayout)
        ImGui::SameLine();

    selectedEventGunsUsedWindow(
            events,
            data.selectedEvent,
            data.selectedGun,
            data.bottomTableSize,
            data.selectedEventValid,
            data.minTableWidth,
            data.maxTableWidth,
            data.bottomWindowSize
        );

    
    if(!data.verticalLayout)
        ImGui::SameLine();

    // Kept function here because it is so short
    if(ImGui::BeginChild("Selected Gun Ammo Used", data.bottomWindowSize)){
        ImGui::SeparatorText( "Ammo Used" );
        ImGui::Spacing(); 
        ImGui::Spacing(); 

        if(data.selectedGun == EMPTY_GUN_METADATA) {
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Ammo Used");
        }
        else{
            centerNextItemX(data.bottomTableSize.x);
            Tables::amountOfAmmo(
                    events.at(data.selectedEvent).getGun(data.selectedGun).getAmmoUsed(), 
                    data.bottomTableSize
                );
        }
    }
    ImGui::EndChild();
}
void EventsWindow::selectEventWindow(
        const std::map<ShootingEventMetadata, ShootingEvent>& events,
        ShootingEventMetadata& selectedEvent,
        GunMetadata& selectedGun,
        ImVec2& tableSize,
        bool& isEventValid,
        float minTableWidth,
        float maxTableWidth,
        const ImVec2& windowSize,
        const ImVec2& buttonSize
    )
{
    isEventValid = selectedEvent != EMPTY_EVENT_METADATA;

    if(ImGui::BeginChild("View Event Table", windowSize)){ 
        ImGui::SeparatorText( "Select An Event" );
        ImGui::Spacing();

        if(!isEventValid)
            ImGui::BeginDisabled();

        if(centerButton("Deselect", buttonSize)){
            selectedEvent  = EMPTY_EVENT_METADATA;
            selectedGun    = EMPTY_GUN_METADATA;
        }

        if(!isEventValid)
            ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Spacing();

        tableSize.x = ImGui::GetContentRegionAvail().x;
        if(tableSize.x < minTableWidth)
            tableSize.x = minTableWidth;
        if(tableSize.x > maxTableWidth)
            tableSize.x = maxTableWidth;

        centerNextItemX(tableSize.x);
        Tables::Selectable::eventsWithNumGunsUsed(events, selectedEvent, tableSize);
        isEventValid = selectedEvent != EMPTY_EVENT_METADATA;
    }
    ImGui::EndChild();
}
void EventsWindow::selectedEventInfoWindow(
        const ShootingEventMetadata& selectedEvent,
        const bool& isEventValid,
        const ImVec2& windowSize
    )
{
    if(ImGui::BeginChild("Selected Event Details", windowSize)){
        ImGui::SeparatorText( "Details" );
        ImGui::Spacing(); 

        if(!isEventValid){
            centerNextItemY(5);
            centerTextDisabled("Select an Event to View Detailed Information");
        }
        else{
            centerNextItemX(400);
            ImGui::BeginGroup();
            ImGui::TextDisabled("Date:              ");
            ImGui::SameLine();
            ImGui::Text("%s", printDate(selectedEvent.date).c_str());

            ImGui::TextDisabled("Location:          ");
            ImGui::SameLine();
            ImGui::Text("%s", selectedEvent.location.getName());

            ImGui::TextDisabled("EventType:         ");
            ImGui::SameLine();
            ImGui::Text("%s", selectedEvent.eventType.getName());
            ImGui::EndGroup();

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();

            centerTextDisabled("Notes");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::TextWrapped("%s", selectedEvent.notes.c_str());
        }
    }
    ImGui::EndChild();
}
void EventsWindow::selectedEventGunsUsedWindow(
        const std::map<ShootingEventMetadata, ShootingEvent>& events,
        const ShootingEventMetadata& selectedEvent,
        GunMetadata& selectedGun,
        ImVec2& tableSize,
        const bool isEventValid,
        float minTableWidth,
        float maxTableWidth,
        const ImVec2& windowSize
    )
{
    if(ImGui::BeginChild("Selected Event Guns Used", windowSize)){
        ImGui::SeparatorText( "Guns Used" );
        ImGui::Spacing(); 
        ImGui::Spacing(); 

        if(!isEventValid) {
            centerNextItemY(5);
            centerTextDisabled("Select an Event to View Detailed Information");
        }
        else{
            tableSize.x = ImGui::GetContentRegionAvail().x;
            if(tableSize.x < minTableWidth)
                tableSize.x = minTableWidth;
            if(tableSize.x > maxTableWidth)
                tableSize.x = maxTableWidth;

            centerTextDisabled("Select a Gun to View Ammo Used");
            ImGui::Spacing();
            ImGui::Spacing();

            centerNextItemX(tableSize.x);
            Tables::Selectable::gunMetadataWithRoundCount(
                    events.at(selectedEvent).getGunsUsed(), 
                    selectedGun, 
                    tableSize
                );
        }
    }
    ImGui::EndChild();
}






void StockpileWindow::main(
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList, 
        const std::map<Cartridge, int>& cartridgeList, 
        ScreenData::View::StockpileWindow& data 
    )
{

    if(ImGui::GetContentRegionAvail().x < (data.minWinSize.x * 2) ){
        data.verticalLayout = true;
        data.windowSize  = ImGui::GetContentRegionAvail();
    }
    else{
        data.verticalLayout = false;
        data.windowSize     = ImVec2{ (ImGui::GetContentRegionAvail().x / 2 ), ImGui::GetContentRegionAvail().y / 2};
    }

    // Ensure minimum windows
    if(data.windowSize.x < data.minWinSize.x)
        data.windowSize.x = data.minWinSize.x;
    if(data.windowSize.y < data.minWinSize.y)
        data.windowSize.y = data.minWinSize.y;

    selectCartridgeWindow(
            cartridgeList,
            data.selectedCartridge,
            data.selectedAmmo,
            data.tableSize,
            data.selectedCartridgeValid,
            data.selectedAmmoValid,
            data.minTableWidth,
            data.maxTableWidth,
            data.windowSize,
            data.deselectButtonSize
        );

    
    static Cartridge lastCartridge { };
    if(data.selectedCartridge != lastCartridge){
        data.selectedAmmo       = EMPTY_AMMO_METADATA;
        data.selectedAmmoValid      = false;
    }
    lastCartridge = data.selectedCartridge;

    if(!data.verticalLayout)
        ImGui::SameLine();

    selectAmmoWindow(
            ammoList,
            data.selectedAmmo,
            data.selectedCartridge,
            data.tableSize,
            data.selectedAmmoValid,
            data.showInactive,
            data.selectedCartridgeValid,
            data.windowSize,
            data.deselectButtonSize
        );


    // Next line of windows here \\
   
    
    selectedAmmoInfoWindow(
            ammoList,
            data.selectedAmmo,
            data.selectedAmmoValid,
            data.windowSize
        );

    if(!data.verticalLayout)
        ImGui::SameLine();

    selectedAmmoGunsUsed(
            ammoList,
            data.selectedAmmo,
            data.tableSize,
            data.selectedAmmoValid,
            data.windowSize
        );

    }
void StockpileWindow::selectCartridgeWindow(
        const std::map<Cartridge, int>& cartridgeList,
        Cartridge&      selectedCartridge,
        AmmoMetadata&   selectedAmmo,
        ImVec2&         tableSize,
        bool&           isCartridgeValid,
        bool&           isAmmoValid,
        float           minTableWidth,
        float           maxTableWidth,
        const ImVec2&   windowSize,
        const ImVec2&   buttonSize
    )
{
    if(ImGui::BeginChild("Select Cartridge Window", windowSize)){ 
        ImGui::SeparatorText("Select Cartridge");
        ImGui::Spacing();
        ImGui::Spacing();

        if(!isCartridgeValid)
            ImGui::BeginDisabled();

        if(centerButton("Deselect", buttonSize)){
            selectedCartridge  = EMPTY_CARTRIDGE;
            selectedAmmo       = EMPTY_AMMO_METADATA;

            isAmmoValid      = false;
        }

        if(!isCartridgeValid)
            ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Spacing();

        tableSize.x = ImGui::GetContentRegionAvail().x-2;
        if(tableSize.x < minTableWidth)
            tableSize.x = minTableWidth;
        if(tableSize.x > maxTableWidth)
            tableSize.x = maxTableWidth;

        centerNextItemX(tableSize.x);
        Tables::Selectable::cartridgeAmountOnHand(
                cartridgeList, 
                selectedCartridge, 
                tableSize
            ); 
        isCartridgeValid = selectedCartridge != EMPTY_CARTRIDGE;
    }
    ImGui::EndChild();

}
void StockpileWindow::selectAmmoWindow(
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
        AmmoMetadata&       selectedAmmo,
        const Cartridge&    selectedCartridge,
        const ImVec2&       tableSize,
        bool&               isAmmoValid,
        bool&               showInactive,
        const bool          isCartridgeValid,
        const ImVec2&       windowSize,
        const ImVec2&       buttonSize
    )
{
    if(ImGui::BeginChild("Select Ammo Window", windowSize)){ 
        ImGui::SeparatorText( "Select Ammo" );
        ImGui::Spacing();
        ImGui::Spacing();

        centerNextItemX(350);
        ImGui::BeginGroup();
        ImGui::Text("Show Inactive Ammo");
        ImGui::SameLine();
        ImGui::Checkbox("##Show Inactive", &showInactive);

        ImGui::SameLine();
        ImGui::Dummy( ImVec2 { 50, 0 } );
        ImGui::SameLine();

        if(!isAmmoValid)
            ImGui::BeginDisabled();

        if(ImGui::Button("Deselect", buttonSize))
            selectedAmmo = EMPTY_AMMO_METADATA;

        if(!isAmmoValid)
            ImGui::EndDisabled();

        ImGui::EndGroup();

        ImGui::Spacing();
        ImGui::Spacing();

        if(!isCartridgeValid) {
            centerNextItemY(5);
            centerTextDisabled("Select a Cartridge to See Ammo On Hand");
        }
        else{
            centerNextItemX(tableSize.x);
            Tables::Selectable::ammoAmountOnHand(
                    ammoList.at(selectedCartridge), 
                    selectedAmmo, 
                    tableSize,
                    showInactive
                );
        }
        isAmmoValid = selectedAmmo != EMPTY_AMMO_METADATA;
    }
    ImGui::EndChild();

}
void StockpileWindow::selectedAmmoInfoWindow(
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
        const AmmoMetadata& selectedAmmo,
        const bool& isAmmoValid,
        const ImVec2& windowSize
    )
{
    if(ImGui::BeginChild("Selected Ammo Details", windowSize)){
        ImGui::SeparatorText( "Selected Ammo Details" );
        ImGui::Spacing(); 
        ImGui::Spacing();

        if(!isAmmoValid){
            centerNextItemY(5);
            centerTextDisabled("Select Ammo to View Detailed Information");
        }
        else{
            const StockpileAmmo& data { ammoList.at(selectedAmmo.cartridge).at(selectedAmmo) };

            centerNextItemX(200);
            ImGui::BeginGroup();

            ImGui::TextDisabled("Name:              ");
            ImGui::SameLine();
            ImGui::Text("%s", selectedAmmo.name.c_str());

            ImGui::TextDisabled("Cartridge:         ");
            ImGui::SameLine();
            ImGui::Text("%s", selectedAmmo.cartridge.getName());

            ImGui::TextDisabled("Manufacturer:      ");
            ImGui::SameLine();
            ImGui::Text("%s", selectedAmmo.manufacturer.getName());

            ImGui::TextDisabled("Grain Weight:      ");
            ImGui::SameLine();
            ImGui::Text("%d", selectedAmmo.grainWeight);

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::TextDisabled("Amount On Hand:    ");
            ImGui::SameLine();
            ImGui::Text("%d", data.getAmountOnHand());

            ImGui::TextDisabled("Guns Used In:      ");
            ImGui::SameLine();
            ImGui::Text("%ld", data.getGunsUsed().size());

            ImGui::EndGroup();
        }
    }
    ImGui::EndChild();

}
void StockpileWindow::selectedAmmoGunsUsed(
        const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
        const AmmoMetadata& selectedAmmo,
        const ImVec2&       tableSize,
        const bool&         isAmmoValid,
        const ImVec2&       windowSize
    )
{
    if(ImGui::BeginChild("Selected Ammo Guns Used", windowSize)){
        ImGui::SeparatorText( "Guns Used" );
        ImGui::Spacing(); 
        ImGui::Spacing();

        if(!isAmmoValid) {
            centerNextItemY(5);
            centerTextDisabled("Select an Event to View Detailed Information");
        }
        else{
            centerTextDisabled("Select a Gun to View Details");
            ImGui::Spacing();
            ImGui::Spacing();

            static GunMetadata viewInfo { };
            centerNextItemX(tableSize.x);
            Tables::Selectable::gunMetadata(
                    ammoList.at(selectedAmmo.cartridge).at(selectedAmmo).getGunsUsed(),
                    viewInfo, 
                    tableSize
                );

            if(viewInfo != EMPTY_GUN_METADATA){
                ScreenData::View buffer { };
                buffer.armoryWindow.selectedGun = viewInfo;
                buffer.category = Category::GUNS;

                UIEvents::SetScreenData::View setView { buffer};
                UIEvents::SetScreen setScreen { Screen::VIEW };
                
                pushEvent(&setView);
                pushEvent(&setScreen);

                viewInfo = EMPTY_GUN_METADATA;
            }
        }
    }
    ImGui::EndChild();

}

}   // End SC::UI
