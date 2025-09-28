#include "UI/UIDrawing.h"

namespace ShooterCentral::UI {

void Home::main (const Containers& containers, ScreenData::Home& data, const UnsavedChanges& changes) {
    
    ImVec2  windowSize { ImGui::GetContentRegionAvail() };
    ImVec2  childWindowSizes { };
    bool    horizontalLayout { false };

    int numShown { 0 }; // Number of child windows

    if(data.showGuns)
        ++numShown;

    if(data.showEvents)
        ++numShown;

    if(data.showStockpile)
        ++numShown;


    // Horizontal bigger than vertical, display tabs side by side
    if( (windowSize.x / numShown) > (windowSize.y / numShown ) ){
        childWindowSizes.x = windowSize.x / numShown;
        childWindowSizes.y = windowSize.y;
        horizontalLayout = true;
    }
    else{
        childWindowSizes.x = windowSize.x;
        childWindowSizes.y = windowSize.y / numShown;
        horizontalLayout = false;
    }

    
    if(data.showGuns){
        if(ImGui::BeginChild("Home Guns", childWindowSizes, 0) )
            Home::gunWindow(containers.getGunsInArmory(), data.selectedGun);
        ImGui::EndChild();
    }

    if(horizontalLayout)
        ImGui::SameLine();

    if(data.showEvents){
        if(ImGui::BeginChild("Home Events", childWindowSizes, 0) )
            Home::eventsWindow(containers.getEvents(), data.selectedEvent);
        ImGui::EndChild();
    }

    if(horizontalLayout)
        ImGui::SameLine();

    if(data.showStockpile){
        if(ImGui::BeginChild("Home Stockpile", childWindowSizes, 0) )
            Home::stockpileWindow(containers.getAmountPerCartridge(), data.selectedCartridge); 
        ImGui::EndChild();
    }
}
void Home::gunWindow  (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, std::weak_ptr<AssociatedGun>& weakSelected ){

    // Size table correctly
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};

    if(tableSize.x < 400)
        tableSize.x = 400;
    if(tableSize.x > 800)
        tableSize.x = 800;

    ImGui::SeparatorText( "Guns In Armory" );
    ImGui::Spacing();

    centerNextItemX(tableSize.x); 
    Tables::selectable_Guns(guns, weakSelected, tableSize);

    std::shared_ptr<AssociatedGun> selected { weakSelected.lock() }; 

    if(!selected)
        weakSelected.reset();

    ImGui::Spacing();
    ImGui::Spacing();


    ImGui::SeparatorText( "Selected Gun" );
    if(!selected) {
        centerTextDisabled("Select A Gun For More Information");
        return;
    }

    Home::gunWindow_selectedGunInformation(*selected);

    return;
}
void Home::gunWindow_selectedGunInformation(const AssociatedGun& gun){
    const GunMetadata& gunInfo  { gun.getGunInfo() };

    if(ImGui::BeginChild("Selected Gun Details Left", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::Text("Name:          %s", gunInfo.name.c_str());
        ImGui::Text("Weapon Type:   %s", gunInfo.weaponType.getName().c_str()); 
        ImGui::Text("Cartridge:     %s", gunInfo.cartridge.getName().c_str());
        ImGui::Unindent(20);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Selected Gun Details Right", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Text("Round Count:       %d", gun.getRoundCount()    );
        ImGui::Text("Events Used In:    %d", gun.totalEventsUsed()  ); 
        //ImGui::Text("Last Event:        %s", 
    }
    ImGui::EndChild();
}
void Home::eventsWindow(const std::map<ShootingEventMetadata, std::shared_ptr<ShootingEvent>>& events, std::weak_ptr<ShootingEvent>& weakSelected ){

    // Size the table
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(tableSize.x < 400)
        tableSize.x = 400;
    if(tableSize.x > 800)
        tableSize.x = 800;

    ImGui::SeparatorText( "Events" );
    ImGui::Spacing();

    centerNextItemX(tableSize.x);
    Tables::selectable_Events(events, weakSelected, tableSize); 

    // Lock or reset selected
    std::shared_ptr<ShootingEvent> selected { weakSelected.lock() }; 
    if(!selected)
        weakSelected.reset();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent(20);
    ImGui::Text("Events:       %ld", events.size()); 
    ImGui::Unindent(20);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText( "Selected Event" );

    if(!selected) {
        centerText("Select An Event For More Information");
        return;
    }

    // Show selected event information
    const ShootingEvent& selectedEvent          { *selected };
    const ShootingEventMetadata& selectedInfo   { selectedEvent.getInfo() };

    if(ImGui::BeginChild("Selected Event Details", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::TextDisabled("Date: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", selectedEvent.printDate().c_str());

        ImGui::TextDisabled("Location: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", selectedInfo.location.getName().c_str());

        ImGui::TextDisabled("Event Type: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", selectedInfo.eventType.getName().c_str());
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Selected Event Notes", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        centerTextDisabled("Notes");
        ImGui::TextWrapped("%s", selectedInfo.notes.c_str());
    }
    ImGui::EndChild();
}
void Home::stockpileWindow(const std::map<Cartridge, int>& cartridgeList, Cartridge& selected){

    // Size the table
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};

    if(tableSize.x < 400)
        tableSize.x = 400;
    if(tableSize.x > 800)
        tableSize.x = 800;

    ImGui::SeparatorText( "Ammo Stockpile" );

    ImGui::Spacing();
    centerNextItemX(tableSize.x);
    Tables::selectable_Cartridges(cartridgeList, selected, tableSize);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent(20);
    ImGui::Text("Cartridges in Stockpile:       %ld", cartridgeList.size()); 
    ImGui::Unindent(20);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText( "Selected Cartridge" );

    if(selected == EMPTY_CARTRIDGE) {
        centerText("Select A Cartridge For More Information");
        return;
    }

    if(ImGui::BeginChild("Selected Cartridge Details", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        if(centerButton("View Detailed\nInformation", ImVec2 { 100, 50 })){
            SetScreen setScreen {Screen::VIEW};
            pushEvent(&setScreen); 
            // FOR TESTING. UNCOMMENT WHEN CIRCULATE DEPENDENCIES ARE FIXED
        }
    }
    ImGui::EndChild();

}
void View::main(const Containers& containers, ScreenData::View& data){

    ImGui::Spacing();
    ImGui::Spacing();

    centerNextComboBoxX("Select A Category", 200);
    ComboBoxes::category(data.category); 

    ImGui::Spacing();
    ImGui::Spacing();

    // Show the necessary window herew
    switch(data.category){
        case Category::GUNS:
            View::gunTab(containers.getGunsInArmory(), data.gunTab);
            break;
        case Category::EVENTS:
            View::eventsTab(containers.getEvents(), data.eventTab);
            break;
        case Category::STOCKPILE:
            View::stockpileTab(containers.getAmmoStockpile(), containers.getAmountPerCartridge(), data.stockpileTab);
            break;
        default:
           
            break;
    }
}
void View::gunTab  (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, ScreenData::View::GunTab& data ){
    
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

    std::shared_ptr selected { data.selectedGun.lock() };
 
    if(ImGui::BeginChild("View Gun Table", topWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::SeparatorText( "Select A Gun" );
        ImGui::Spacing();

        bool reset          { false };
        bool applyYOffset   { false };

        if(selected){
            centerNextItemX(buttonSize.x);
            if(ImGui::Button("Deselect", buttonSize)){
                data.selectedGun.reset();
                selected = nullptr;
                reset = true;
            }
            applyYOffset = true;
        }

        centerNextItemX(topTableSize.x);
        centerNextItemY(topTableSize.y);
        
        if(applyYOffset)
            ImGui::SetCursorPosY(ImGui::GetCursorPos().y + (buttonSize.y * 0.5f) + 2);

        Tables::selectable_Guns(guns, data.selectedGun, topTableSize);
        selected = data.selectedGun.lock();
        
    }
    ImGui::EndChild();

    if(ImGui::BeginChild("Selected Gun Details", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Details" );
        ImGui::Spacing();

        if(!selected){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Detailed Information");
        }
        else
            View::gunTab_selectedGunInformation(*selected);
    }
    ImGui::EndChild();

    if(!verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Selected Gun Events", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Events Used" );
        ImGui::Spacing();

        if(!selected){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to View Events");
        }
        else
            View::gunTab_eventsWindow(*selected, data.selectedEvent);    
    }
    ImGui::EndChild();

    if(!verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Selected Gun Ammo Used", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Ammo Used" );
        ImGui::Spacing();

        if(!selected){
            centerNextItemY(5);
            centerTextDisabled("Select a Gun to Ammo Used");
        }
        else
            View::gunTab_ammoUsedWindow(*selected); 
    }
    ImGui::EndChild();

}
void View::gunTab_selectedGunInformation(const AssociatedGun& gun){
    const GunMetadata& gunInfo  { gun.getGunInfo() };

    centerNextItemX(200);
    ImGui::BeginGroup();
    ImGui::Text("Name:              %s", gunInfo.name.c_str());
    ImGui::Text("Weapon Type:       %s", gunInfo.weaponType.getName().c_str()); 
    ImGui::Text("Cartridge:         %s", gunInfo.cartridge.getName().c_str());
    ImGui::Text("Round Count:       %d", gun.getRoundCount());

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Events Used In:    %d", gun.totalEventsUsed()); 
    ImGui::Text("Ammo Types Used:   %d", gun.totalAmmoTypesUsed()); 
    ImGui::EndGroup();
    
    return;
}
void View::gunTab_eventsWindow(const AssociatedGun& gun, std::weak_ptr<ShootingEvent>& selectedEvent){
    const GunMetadata& gunInfo  { gun.getGunInfo() };

    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    ImVec2 tableSize  { ImGui::GetContentRegionAvail().x-2, 400};
    if(tableSize.x < MIN_TABLE_SIZE_X)
        tableSize.x = MIN_TABLE_SIZE_X;
    if(tableSize.x > MAX_TABLE_SIZE_X)
        tableSize.x = MAX_TABLE_SIZE_X;

    centerNextItemX(tableSize.x);
    Tables::selectable_Events(gun.getEventsUsed(), selectedEvent, tableSize);

    if(selectedEvent.lock()){
        std::cout << "there was an event selected\n"; // Make command to enter change to view event with this Event
        selectedEvent.reset();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Total Events Used: %d", gun.totalEventsUsed());

    return;
}
void View::gunTab_ammoUsedWindow(const AssociatedGun& gun){
    const GunMetadata& gunInfo  { gun.getGunInfo() };

    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    ImVec2 tableSize  { ImGui::GetContentRegionAvail().x-2, 400};
    if(tableSize.x < MIN_TABLE_SIZE_X)
        tableSize.x = MIN_TABLE_SIZE_X;
    if(tableSize.x > MAX_TABLE_SIZE_X)
        tableSize.x = MAX_TABLE_SIZE_X;

    centerNextItemX(tableSize.x);
    Tables::amountOfAmmo(gun.getAmmoUsed(), tableSize);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Total Ammo Types Used: %d", gun.totalAmmoTypesUsed());

    return;

}
void View::eventsTab(
        const std::map<ShootingEventMetadata, std::shared_ptr<ShootingEvent>>& events, 
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

        topWindowSize  = { MIN_WIN_TOP_SIZE_X, MIN_WIN_BOTTOM_SIZE_Y };
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

    std::shared_ptr<ShootingEvent> selected { data.selectedEvent.lock() };
 
    if(ImGui::BeginChild("View Event Table", topWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::SeparatorText( "Select An Event" );
        ImGui::Spacing();

        bool applyYOffset   { false };

        if(selected){
            centerNextItemX(buttonSize.x);
            if(ImGui::Button("Deselect", buttonSize)){
                data.selectedEvent.reset();
                selected = nullptr;
            }
            applyYOffset = true;
        }

        centerNextItemX(topTableSize.x);
        centerNextItemY(topTableSize.y);
        
        if(applyYOffset)
            ImGui::SetCursorPosY(ImGui::GetCursorPos().y + (buttonSize.y * 0.5f) + 2);

        Tables::selectable_Events(events, data.selectedEvent, topTableSize);
        selected = data.selectedEvent.lock();
        
    }
    ImGui::EndChild();

    if(ImGui::BeginChild("Selected Event Details", infoWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Details" );
        ImGui::Spacing(); 

        if(!selected){
            centerNextItemY(5);
            centerTextDisabled("Select an Event to View Detailed Information");
        }
        else
            View::eventsTab_selectedEventInformation(*selected);
    }
    ImGui::EndChild();

    if(!verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Selected Event Guns Used", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText( "Guns Used" );
        ImGui::Spacing(); 

        if(!selected) {
            centerNextItemY(5);
            centerTextDisabled("Select an Event to View Detailed Information");
        }
        else
            View::eventsTab_gunsUsed(*selected, data.selectedGun);
    }
    ImGui::EndChild();

}
void View::eventsTab_selectedEventInformation(const ShootingEvent& event){
    const ShootingEventMetadata& info  { event.getInfo() };

    centerNextItemX(400);
    ImGui::BeginGroup();
    ImGui::Text("Date:              %s", event.printDate().c_str());
    ImGui::Text("Location:          %s", info.location.getName().c_str());
    ImGui::Text("Event Type:        %s", info.eventType.getName().c_str()); 
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText("Notes");
    ImGui::Spacing();
    ImGui::TextWrapped("%s", info.notes.c_str());
    
    return;
}
void View::eventsTab_gunsUsed(const ShootingEvent& event, std::reference_wrapper<const GunAndAmmo>& selectedGunReference){
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
    Tables::selectable_EventGunsUsed(event.getGunsUsed(), selectedGunReference, tableSize);

    if(selectedGunReference.get().getGunInfo() == EMPTY_GUN_AND_AMMO.getGunInfo())
        return;

    ImGui::Spacing();
    ImGui::SeparatorText("Ammo Used");
    ImGui::Spacing();

    centerNextItemX(tableSize.x);
    Tables::amountOfAmmo(selectedGunReference.get().getAmmoUsedList(), tableSize);
}
void View::stockpileTab(
        const std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>>& ammoList,
        const std::map<Cartridge, int>& cartridgeList,
        ScreenData::View::StockpileTab& stockpileTab
){
    static constexpr float MIN_WIN_BOTTOM_SIZE_X { 400 };
    static constexpr float MIN_WIN_BOTTOM_SIZE_Y { 600 };

    static constexpr float MIN_WIN_BOTTOM_INFO_SIZE_X { 200 };
    static constexpr float MIN_WIN_BOTTOM_INFO_SIZE_Y { 600 };

    static constexpr float MIN_WIN_TOP_SIZE_X { (MIN_WIN_BOTTOM_SIZE_X * 2) + MIN_WIN_BOTTOM_INFO_SIZE_X };

    static constexpr float MIN_TABLE_SIZE_X { 400 };
    static constexpr float MAX_TABLE_SIZE_X { 800 };

    // Button size

    bool verticalLayout {false};

    ImVec2 topWindowSize;
    ImVec2 bottomWindowSize;
    ImVec2 topTableSize;
    ImVec2 bottomTableSize;

    if(ImGui::GetContentRegionAvail().x < MIN_WIN_TOP_SIZE_X){
        verticalLayout = true;

        topWindowSize  = { MIN_WIN_TOP_SIZE_X, MIN_WIN_BOTTOM_SIZE_Y };
        bottomWindowSize = topWindowSize;
    }
    else{
        topWindowSize  = {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2};
        if(topWindowSize.x < MIN_WIN_TOP_SIZE_X)
            topWindowSize.x = MIN_WIN_TOP_SIZE_X;
        if(topWindowSize.y < MIN_WIN_BOTTOM_SIZE_Y)
            topWindowSize.y = MIN_WIN_BOTTOM_SIZE_Y;

        bottomWindowSize =  { (ImGui::GetContentRegionAvail().x / 2), ImGui::GetContentRegionAvail().y / 2};
        if(bottomWindowSize.x < MIN_WIN_BOTTOM_SIZE_X)
            bottomWindowSize.x = MIN_WIN_BOTTOM_SIZE_X;
        if(bottomWindowSize.y < MIN_WIN_BOTTOM_SIZE_Y)
            bottomWindowSize.y = MIN_WIN_BOTTOM_SIZE_Y;
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

    static const ImVec2 buttonSize { 100, 40 };
    if(ImGui::BeginChild("Select Cartridge Table", topWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::SeparatorText( "Select A Cartridge" );
        ImGui::Spacing();

        bool applyYOffset   { false };

        if(stockpileTab.selectedCartridge != EMPTY_CARTRIDGE){
            centerNextItemX(buttonSize.x);
            if(ImGui::Button("Deselect", buttonSize)){
                stockpileTab.selectedCartridge = EMPTY_CARTRIDGE;
                stockpileTab.selectedAmmo.reset() ;
            }
            applyYOffset = true;
        }

        centerNextItemX(topTableSize.x);
        centerNextItemY(topTableSize.y);
        
        if(applyYOffset)
            ImGui::SetCursorPosY(ImGui::GetCursorPos().y + (buttonSize.y * 0.5f) + 2);
        Tables::selectable_Cartridges(cartridgeList, stockpileTab.selectedCartridge, topTableSize);
    }
    ImGui::EndChild();

    if(ImGui::BeginChild("Selected Cartridge Details", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText("Detailed Breakdown" );
        ImGui::Spacing(); 
        ImGui::Spacing();

        if(!ammoList.contains(stockpileTab.selectedCartridge)){
            centerNextItemY(5);
            centerTextDisabled("Select a Cartridge to View Ammo On Hand");
        }
        else{
            centerNextItemX(bottomTableSize.x);
            Tables::selectable_SingleCartridgeAmmo(ammoList.at(stockpileTab.selectedCartridge), stockpileTab.selectedAmmo, bottomTableSize);
        }

    }
    ImGui::EndChild();

    std::shared_ptr<AssociatedAmmo> selectedAmmo { stockpileTab.selectedAmmo.lock() };

    if(!verticalLayout)
        ImGui::SameLine();
    
    if(ImGui::BeginChild("Selected Ammo Guns Used Ammo", bottomWindowSize, ImGuiChildFlags_Border)){
        ImGui::SeparatorText("Guns That Have Used Selected Ammo" );
        ImGui::Spacing(); 
        ImGui::Spacing();

        if(!selectedAmmo){
            centerNextItemY(5);
            centerTextDisabled("Select Ammo to View Guns Used");
        }
        else{
            centerNextItemX(bottomTableSize.x);
            Tables::ammoGunsUsed(selectedAmmo->getGunsUsed(), bottomTableSize);
        }

    }
    ImGui::EndChild();
}
void Add::main(const Containers& containers, ScreenData::Add& data){

    static constexpr float MIN_WIN_SIZE_X { 400 };
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
        showExistingItemsWindow(containers, data.subItem, topTableSize);
    }
    ImGui::EndChild();

    if(ImGui::BeginChild("Add Item Window", bottomWindowSize, ImGuiChildFlags_Border)){ 
        ImGui::SeparatorText("Add Item"); 

        ImGui::Dummy(ImVec2{0.0f, 20.0f});

        addItemWindow(data.subItem);
    }
    ImGui::EndChild();

}
void Add::showExistingItemsWindow (const Containers& containers, const SubItem& selected, ImVec2 size){
    ImGui::BeginGroup();

    std::weak_ptr<ShootingEvent> selectedEvent;
    std::weak_ptr<AssociatedGun> selectedGun;
    std::weak_ptr<AmmoMetadata> selectedAmmo;
 
    switch(selected){
        case SubItem::EVENT_EVENT:
            centerTextDisabled("All Events");
            Tables::selectable_Events(containers.getEvents(), selectedEvent, size);

            if(selectedEvent.lock())
                std::cout << "Command change to view event\n";

            break;
        case SubItem::EVENT_TYPE:
            centerTextDisabled("All Event Types");
            ListBoxes::eventTypes(containers.getEventTypes(), size);
            break;
        case SubItem::EVENT_LOCATION:
            centerTextDisabled("All Event Locations");
            ListBoxes::eventLocations(containers.getLocations(), size);
            break;
        case SubItem::AMMO_AMMO:
            centerTextDisabled("All Ammo");
            Tables::selectable_KnownAmmo(containers.getKnownAmmo(), selectedAmmo, size);

            if(selectedAmmo.lock())
                std::cout << "Change to view ammo command\n";

            break;
        case SubItem::AMMO_MANUFACTURER:
            centerTextDisabled("All Manufacturers");
            ListBoxes::manufacturers(containers.getManufacturers(), size);
            break;
        case SubItem::GUN_AMMO_CARTRIDGE:
            centerTextDisabled("All Cartridges");
            ListBoxes::cartridges(containers.getCartridges(), size);
            break;
        case SubItem::GUN_GUN:
            centerTextDisabled("All Guns");
            Tables::selectable_Guns(containers.getGunsInArmory(), selectedGun, size);

            if(selectedGun.lock())
                std::cout << "Command change to view gun\n";

            break;
        case SubItem::GUN_WEAPON_TYPE:
            centerTextDisabled("All Weapon Types");
            ListBoxes::weaponTypes(containers.getWeaponTypes(), size);
            break;
        default:
            centerTextDisabled("Select an Item");
            break;
    }
    ImGui::EndGroup();
}
void Add::addItemWindow(const SubItem& selectedItem){
    switch(selectedItem){
        case SubItem::EVENT_EVENT:
            

            break;
        case SubItem::EVENT_TYPE:
           
          
            break;
        case SubItem::EVENT_LOCATION:
         
        
            break;
        case SubItem::AMMO_AMMO:
       
            break;
        case SubItem::AMMO_MANUFACTURER:

            break;
        case SubItem::GUN_AMMO_CARTRIDGE:

            break;
        case SubItem::GUN_GUN:

            break;
        case SubItem::GUN_WEAPON_TYPE:
            Add::add_WeaponType();
            break;
        default:
            centerTextDisabled("Select an Item");
            break;
    }
 
}
void Add::add_WeaponType(){
    static char textBuf[ScreenData::Add::MAX_CHAR_INPUT] = "New Weapon Type";

    ImGui::Text("Directions");
    ImGui::BulletText("This will add a new Weapon Type to choose from when creating a gun.");
    ImGui::BulletText("Must save before exiting otherwise changes will not be made.");
    
    ImGui::Dummy(ImVec2{0.0f, 50.0f});
    
    ImGui::Text("Weapon Type");
    ImGui::SameLine(100);
    ImGui::InputText("##Input New Weapon Type", textBuf, ScreenData::Add::MAX_CHAR_INPUT, ImGuiInputTextFlags_CharsUppercase);

    ImGui::Dummy(ImVec2{0.0f, 50.0f});

    if(centerButton("Add New Weapon Type", ImVec2{200,50})){
        std::cout << "Command add weapon type\n";
    }
}

void centerNextItemX(float x){
    float windowWidth { ImGui::GetContentRegionAvail().x };
    if(x < windowWidth)
        ImGui::SetCursorPosX((windowWidth - x) * 0.5f);

    return;
}
void centerNextItemY(float y){
    float windowHeight { ImGui::GetContentRegionAvail().y };
    if(y < windowHeight)
        ImGui::SetCursorPosY((windowHeight - y) * 0.5f);

    return;

}
void centerNextComboBoxX(const std::string& text, float comboBoxWidth){
    centerNextItemX(ImGui::CalcTextSize(text.c_str()).x + comboBoxWidth );
    ImGui::Text("%s", text.c_str()); 
    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboBoxWidth);
}

void centerText(const std::string& text){
    float windowWidth { ImGui::GetWindowSize().x };
    float textWidth   { ImGui::CalcTextSize(text.c_str()).x };

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text.c_str());
}
void centerTextDisabled(const std::string& text){
    float windowWidth { ImGui::GetWindowSize().x };
    float textWidth   { ImGui::CalcTextSize(text.c_str()).x };

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::TextDisabled("%s", text.c_str());
}
bool centerButton(const std::string& text, ImVec2 size){
    float windowWidth = ImGui::GetWindowSize().x;

    ImGui::SetCursorPosX((windowWidth - size.x) * 0.5f);
    return ImGui::Button(text.c_str(), size);
}
std::string categoryToString(const Category& category, const std::string& noneStr){
    switch(category){
        case Category::NONE:
            if(noneStr.empty())
                return std::string {"None"};
            else
                return noneStr;
            break;
        case Category::GUNS:
            return std::string {"Guns"};
            break;
        case Category::EVENTS:
            return std::string {"Events"};
            break;
        case Category::STOCKPILE:
            return std::string {"Stockpile"};
            break;
        default:
            return std::string {"NOT HANDLED"};
            break;
    }
}
std::string subItemToString (const SubItem& item, const std::string& noneStr){
    switch(item){
        case SubItem::NONE:
            if(noneStr.empty())
                return std::string {"None"};
            else
                return noneStr;
            break;
        case SubItem::EVENT_EVENT:
            return std::string {"Event"};
            break;
        case SubItem::EVENT_TYPE:
            return std::string {"Event Type"};
            break;
        case SubItem::EVENT_LOCATION:
            return std::string {"Event Location"};
            break;
        case SubItem::AMMO_AMMO:
            return std::string {"Ammo"};
            break;
        case SubItem::AMMO_MANUFACTURER:
            return std::string {"Manufacuter"};
            break;
        case SubItem::GUN_AMMO_CARTRIDGE:
            return std::string {"Cartridge"};
            break;
        case SubItem::GUN_GUN:
            return  std::string {"Gun"};
            break;
        case SubItem::GUN_WEAPON_TYPE:
            return std::string {"Weapon Type"};
            break;
        default:
            return std::string {"NOT HANDLED"}; 
            break;
    }
}

void displayPopUp (char* text) {
    if(!text)
        return;

    ImGui::OpenPopup("Basic PopUp");

    if(ImGui::BeginPopup("Basic PopUp")){
        ImGui::Text("placeholder text");

        ImGui::EndPopup();
    }

}

void  Tables::selectable_Guns(const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& list, std::weak_ptr<AssociatedGun>& weakSelected, ImVec2 size){
    std::shared_ptr<AssociatedGun> selected { weakSelected.lock() };

    int row { 0 };
    if(ImGui::BeginTable("Guns Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size)) {
        ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Round Count",  ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& [cartridge, map] : list){
            // Do not sort by Cartridge so just ignore it
            for(const auto& [gunMetadata, assocGunPtr] : map){
                const GunMetadata& gun {assocGunPtr->getGunInfo()};
                bool isGunSelected { false };
            
                if(selected && selected == assocGunPtr)
                    isGunSelected = true;
            
                ImGui::PushID(std::to_string(row).c_str());
                ImGui::TableNextRow();

                for (int column{0}; column < 4; ++column){
                    ImGui::TableSetColumnIndex(column);
                    switch( column ){
                        case 0:
                            ImGui::Selectable(gun.weaponType.getName().c_str(), &isGunSelected, ImGuiSelectableFlags_SpanAllColumns);

                            if(isGunSelected){
                                weakSelected = assocGunPtr;
                                selected = weakSelected.lock();
                            }

                            break;
                        case 1:
                            ImGui::Text("%s", gun.cartridge.getName().c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", gun.name.c_str());
                            break;
                        case 3:
                            ImGui::Text("%d", assocGunPtr->getRoundCount());
                            break;
                        default:
                            ImGui::TextDisabled("Broken table");
                            break;
                    }
                }
                ImGui::PopID();
                ++row;       
            }
        }
        ImGui::EndTable();
    }
}
void Tables::selectable_EventGunsUsed(const std::vector<GunAndAmmo>& list, std::reference_wrapper<const GunAndAmmo>& selected, ImVec2 size ){
    int row { 0 };
    if(ImGui::BeginTable("Guns Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size)) {
        ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Amount Used",  ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const GunAndAmmo& gunAndAmmo : list){
            const GunMetadata& gunInfo {gunAndAmmo.getGunInfo()};
            bool isGunSelected { false };
        
            if(selected.get().getGunInfo() == gunInfo)
                isGunSelected = true;
        
            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column){
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(gunInfo.weaponType.getName().c_str(), &isGunSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isGunSelected)
                            selected = gunAndAmmo;

                        break;
                    case 1:
                        ImGui::Text("%s", gunInfo.cartridge.getName().c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", gunInfo.name.c_str());
                        break;
                    case 3:
                        ImGui::Text("%d", gunAndAmmo.totalRoundsShot());
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
}

void Tables::selectable_Events( const std::map<ShootingEventMetadata, std::shared_ptr<ShootingEvent>>& events, std::weak_ptr<ShootingEvent>& weakSelected, ImVec2 size ){
    std::shared_ptr<ShootingEvent> selected { weakSelected.lock() };

    int row { 0 };
    if(ImGui::BeginTable("Event Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )){
        ImGui::TableSetupColumn("Date",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Event Type",   ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Location",     ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Guns Used",    ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& [eventMetadata, eventPtr] : events){
            const ShootingEvent& event              { *eventPtr };
            const ShootingEventMetadata& eventInfo  { event.getInfo() };
            bool isEventSelected    { false };

            if(selected && selected == eventPtr)
                isEventSelected = true;
            
            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column){                   
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(event.printDate().c_str(), &isEventSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isEventSelected){
                            weakSelected = eventPtr;
                            selected = weakSelected.lock();
                        }
                        break;
                    case 1:
                        ImGui::Text("%s", eventInfo.eventType.getName().c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", eventInfo.location.getName().c_str());
                        break;
                    case 3:
                        ImGui::Text("%d", event.totalGunsUsed());
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
void Tables::selectable_Cartridges(const std::map<Cartridge, int>& cartridges, Cartridge& selected, ImVec2 size){
    int row { 0 };

    if(ImGui::BeginTable("Cartridge Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Cartridge",    0);
        ImGui::TableSetupColumn("Amount",       0);
        ImGui::TableHeadersRow();

        for(const auto& [cartridge, amount] : cartridges){
            bool isSelected { false };

            if(selected == cartridge)
                isSelected = true;

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 2; ++column){        
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(cartridge.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isSelected)
                            selected = cartridge;
                        break;
                    case 1:
                        ImGui::Text("%d", amount);
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
void Tables::selectable_SingleCartridgeAmmo(
    const std::map<AmmoMetadata, std::shared_ptr<AssociatedAmmo>>& list, 
    std::weak_ptr<AssociatedAmmo>& weakSelected,
    ImVec2 size 
){
    std::shared_ptr<AssociatedAmmo> selected { weakSelected.lock() };

    int row { 0 };
    if(ImGui::BeginTable("Ammo Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Grain Weight", 0);
        ImGui::TableSetupColumn("Amount On Hand",  0);

        ImGui::TableHeadersRow();

        for(const auto& [ammoInfo, associatedAmmoPtr] : list){
            const AssociatedAmmo& associatedAmmo { *associatedAmmoPtr };
            bool isSelected { false };

            if(selected == associatedAmmoPtr)
                isSelected = true;

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column) {
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(ammoInfo.manufacturer.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isSelected){
                            weakSelected = associatedAmmoPtr;
                            selected = weakSelected.lock();
                        }
                        break;
                    case 1:
                        ImGui::Text("%s", ammoInfo.name.c_str());
                        break;
                    case 2:
                        ImGui::Text("%d", ammoInfo.grainWeight);
                        break;
                    case 3:
                        ImGui::Text("%d", associatedAmmo.getAmountOfAmmo().getAmount());
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
void Tables::selectable_KnownAmmo(
    const std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>& list,       
    std::weak_ptr<AmmoMetadata>& weakSelected,
    ImVec2 size
){
    std::shared_ptr<AmmoMetadata> selected { weakSelected.lock() };

    int row { 0 };
    if(ImGui::BeginTable("Ammo Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Cartridge", 0);
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Grain Weight", 0);

        ImGui::TableHeadersRow();

        for(const auto& [ammoInfo, ammoInfoPtr] : list){
            bool isSelected { false };

            if(selected == ammoInfoPtr)
                isSelected = true;

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column) {
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(ammoInfo.cartridge.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns)){
                            weakSelected = ammoInfoPtr;
                            selected = weakSelected.lock();
                        }
                        break;
                    case 1:
                        ImGui::Text("%s", ammoInfo.manufacturer.getName().c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", ammoInfo.name.c_str());
                        break;
                    case 3:
                        ImGui::Text("%d", ammoInfo.grainWeight);
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

void Tables::amountOfAmmo(const std::vector<AmountOfAmmo>& ammoUsed, ImVec2 size){
    int row { 0 };
    if(ImGui::BeginTable("Ammo Used Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Grain Weight", 0);
        ImGui::TableSetupColumn("Amount Used",  0);

        ImGui::TableHeadersRow();

        for(const auto& amountOfAmmo : ammoUsed){
            const AmmoMetadata& info { amountOfAmmo.getAmmoInfo() };
            bool isSelected { false };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column) {
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(info.manufacturer.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isSelected)
                            std::cout << "Ammo type was selected\n"; // Command here to jump to this ammo type next
                        break;
                    case 1:
                        ImGui::Text("%s", info.name.c_str());
                        break;
                    case 2:
                        ImGui::Text("%d", info.grainWeight);
                        break;
                    case 3:
                        ImGui::Text("%d", amountOfAmmo.getAmount());
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
void Tables::amountOfAmmo(  const std::map<AmmoMetadata, AmountOfAmmo>& ammoUsed, ImVec2 size){
    int row { 0 };
    if(ImGui::BeginTable("Detailed Cartridge Breakdown", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Grain Weight", 0);
        ImGui::TableSetupColumn("Amount Used",  0);

        ImGui::TableHeadersRow();

        for(const auto& [info, amountOfAmmo] : ammoUsed){
            bool isItemSelected { false };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column) {
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(info.manufacturer.getName().c_str(), &isItemSelected, ImGuiSelectableFlags_SpanAllColumns);
                        
                        if(isItemSelected)
                            std::cout << "Ammo type was selected\n"; // Command to jump to view this ammo next

                        break;
                    case 1:
                        ImGui::Text("%s", info.name.c_str());
                        break;
                    case 2:
                        ImGui::Text("%d", info.grainWeight);
                        break;
                    case 3:
                        ImGui::Text("%d", amountOfAmmo.getAmount());
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
void Tables::ammoGunsUsed (
    const std::map<GunMetadata, std::shared_ptr<GunMetadata>>& list, 
    ImVec2 size
){
    int row { 0 };
    if(ImGui::BeginTable("Gun Details", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Weapon Type", 0);
        ImGui::TableSetupColumn("Name",        0);

        ImGui::TableHeadersRow();

        for(const auto& [gunInfo, gunInfoPtr] : list){
            bool isSelected { false };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 2; ++column) {
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(gunInfo.weaponType.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
                        
                        if(isSelected)
                            std::cout << "Gun was selected\n"; // Command to jump to view this ammo next

                        break;
                    case 1:
                        ImGui::Text("%s", gunInfo.name.c_str());
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
void ListBoxes::cartridges(const std::map<Cartridge, std::shared_ptr<Cartridge>>& list, ImVec2 size){
    if(ImGui::BeginListBox("##Cartridge List Box", size)){
        for(const auto& [key, ptr] : list) {
            bool isSelected { false };
            ImGui::Selectable(key.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}
void ListBoxes::manufacturers(const std::map<Manufacturer, std::shared_ptr<Manufacturer>>& list, ImVec2 size){
    if(ImGui::BeginListBox("##Manufacturer List Box", size)){
        for(const auto& [key, ptr] : list) {
            bool isSelected { false };
            ImGui::Selectable(key.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}
void ListBoxes::eventLocations (const std::map<Location, std::shared_ptr<Location>>& list,         ImVec2 size){
    if(ImGui::BeginListBox("##Event Location List Box", size)){
        for(const auto& [key, ptr] : list) {
            bool isSelected { false };
            ImGui::Selectable(key.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}
void ListBoxes::eventTypes(const std::map<ShootingEventType, std::shared_ptr<ShootingEventType>>& list,       ImVec2 size){
    if(ImGui::BeginListBox("##Event Type List Box", size)){
        for(const auto& [key, ptr] : list) {
            bool isSelected { false };
            ImGui::Selectable(key.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}
void ListBoxes::weaponTypes(const std::map<WeaponType, std::shared_ptr<WeaponType>>& list,     ImVec2 size){
    if(ImGui::BeginListBox("##Weapon Type List Box", size)){
        for(const auto& [key, ptr] : list) {
            bool isSelected { false };
            ImGui::Selectable(key.getName().c_str(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}

void ComboBoxes::category(Category& selected) {
    std::string text { categoryToString(selected, "Select A Category") };

    if (ImGui::BeginCombo("##Category Select Combo", text.c_str(), ImGuiComboFlags_HeightSmall)) {
        for (const Category& category : CATEGORY_LIST) {
            const bool isSelected = (selected == category);

            if(ImGui::Selectable(categoryToString(category, "Select A Category").c_str(), isSelected)){
                selected = category;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void ComboBoxes::subItem(SubItem& selected){
    std::string text { subItemToString(selected, "Select A Category") };

    if (ImGui::BeginCombo("##Sub Item Select Combo", text.c_str(), ImGuiComboFlags_HeightSmall)) {
        for (const SubItem& item : SUB_ITEM_LIST) {
            const bool isSelected = (selected == item);

            if(ImGui::Selectable(subItemToString(item, "Select A Category").c_str(), isSelected)){
                selected = item;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

}   // End UI namespace
