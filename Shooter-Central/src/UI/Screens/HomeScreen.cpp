#include "HomeScreen.h"

namespace ShooterCentral::UI {

void Home::main (const Database& database, ScreenData::Home& data, const UnsavedChanges& changes) {

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
            Home::gunWindow(database.getArmory(), data.selectedGun);
        ImGui::EndChild();
    }

     
    if(horizontalLayout)
        ImGui::SameLine();

    if(data.showEvents){
        if(ImGui::BeginChild("Home Events", childWindowSizes, 0) )
            Home::eventsWindow(database.getEvents(), data.selectedEvent);
        ImGui::EndChild();
    }

    if(horizontalLayout)
        ImGui::SameLine();

    if(data.showStockpile){
        if(ImGui::BeginChild("Home Stockpile", childWindowSizes, 0) )
            Home::stockpileWindow(database.getAmountPerCartridge(), data.selectedCartridge); 
        ImGui::EndChild();
    }
}
void Home::gunWindow(const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& guns, GunMetadata& selectedGun){

    // Size table correctly
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};

    if(tableSize.x < 400)
        tableSize.x = 400;
    if(tableSize.x > 800)
        tableSize.x = 800;

    ImGui::SeparatorText( "Guns In Armory" );
    ImGui::Spacing();

    centerNextItemX(tableSize.x); 
    Tables::Selectable::gunMetadataWithRoundCount(guns, selectedGun, tableSize);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText( "Selected Gun" );

    if(selectedGun == GunMetadata { } ) {
        centerTextDisabled("Select A Gun For More Information");
        return;
    }

    Home::gunWindow_selectedGunInformation(guns.at(selectedGun.cartridge).at(selectedGun));

    return;
}
void Home::gunWindow_selectedGunInformation(const ArmoryGun& gun){
    const GunMetadata& gunInfo  { gun.getGunInfo() };

    if(ImGui::BeginChild("Selected Gun Details Left", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::Text("Name:          %s", gunInfo.name.c_str());
        ImGui::Text("Weapon Type:   %s", gunInfo.weaponType.getName()); 
        ImGui::Text("Cartridge:     %s", gunInfo.cartridge.getName());
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
void Home::eventsWindow(const std::map<ShootingEventMetadata, ShootingEvent>& events, ShootingEventMetadata& selected ){

    // Size the table
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};
    if(tableSize.x < 400)
        tableSize.x = 400;
    if(tableSize.x > 800)
        tableSize.x = 800;

    ImGui::SeparatorText( "Events" );
    ImGui::Spacing();

    centerNextItemX(tableSize.x);
    Tables::Selectable::eventsWithGunsUsed(events, selected, tableSize); 

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent(20);
    ImGui::Text("Events:       %ld", events.size()); 
    ImGui::Unindent(20);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText( "Selected Event" );

    if(selected == EMPTY_EVENT_METADATA) {
        centerTextDisabled("Select An Event For More Information");
        return;
    }

    // Show selected event information
    const ShootingEvent& selectedEvent          { events.at(selected) };
    const ShootingEventMetadata& selectedInfo   { selectedEvent.getInfo() };

    if(ImGui::BeginChild("Selected Event Details", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::TextDisabled("Date: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", selectedEvent.printDate().c_str());

        ImGui::TextDisabled("Location: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", selectedInfo.location.getName());

        ImGui::TextDisabled("Event Type: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", selectedInfo.eventType.getName());
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
    Tables::Selectable::cartridgeAmountOnHand(cartridgeList, selected, tableSize);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent(20);
    ImGui::Text("Cartridges in Stockpile:       %ld", cartridgeList.size()); 
    ImGui::Unindent(20);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText( "Selected Cartridge" );

    if(selected == EMPTY_CARTRIDGE) {
        centerTextDisabled("Select A Cartridge For More Information");
        return;
    }

    if(ImGui::BeginChild("Selected Cartridge Details", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        if(centerButton("View More Information", ImVec2 { 200, 50 })){

            /*
            ScreenData::View newBuffer { };
            newBuffer.category = Category::STOCKPILE;
            newBuffer.stockpileTab.selectedCartridge = selected;

            UIEvents::SetScreenData_View setScreenData { newBuffer };
            UIEvents::SetScreen setScreen {Screen::VIEW};

            pushEvent(&setScreenData);
            pushEvent(&setScreen); 
            */
        }
    }
    ImGui::EndChild();

}

}   // End SC::UI namespace
