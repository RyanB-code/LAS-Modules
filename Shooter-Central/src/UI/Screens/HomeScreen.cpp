#include "HomeScreen.h"

namespace ShooterCentral::UI::Home {

void main (const Database& database, ScreenData::Home& screenData, const UnsavedChanges& changes) {
    UI::ScreenData::Home::MainWindow& data { screenData.mainWindow };

    data.oneThirdWindowSize.x = ImGui::GetContentRegionAvail().x / 3;
    data.oneThirdWindowSize.y = ImGui::GetContentRegionAvail().y / 3;


    if(data.oneThirdWindowSize.x < data.MIN_WIN_SIZE.x ){
        data.childWindowSize.x = ImGui::GetContentRegionAvail().x;
        data.verticalLayout = true;
    }
    else{
        data.childWindowSize.x = data.oneThirdWindowSize.x;
        data.verticalLayout = false;
    }

    if(data.oneThirdWindowSize.y < data.MIN_WIN_SIZE.y )
        data.childWindowSize.y = data.MIN_WIN_SIZE.y;
    else
        data.childWindowSize.y = ImGui::GetContentRegionAvail().y;

    if(data.childWindowSize.x < data.MIN_WIN_SIZE.x )
        data.childWindowSize.x = data.MIN_WIN_SIZE.x;


    if(ImGui::BeginChild("Home Guns", data.childWindowSize, 0) )
        Home::gunWindow(database.getArmory(), screenData.gunWindow);
    ImGui::EndChild();

    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Home Events", data.childWindowSize, 0) )
        Home::eventsWindow(database.getEvents(), screenData.eventsWindow);
    ImGui::EndChild();

    if(!data.verticalLayout)
        ImGui::SameLine();

    if(ImGui::BeginChild("Home Stockpile", data.childWindowSize, 0) )
        Home::stockpileWindow(database.getAmountPerCartridge(), screenData.stockpileWindow); 
    ImGui::EndChild();
}
void gunWindow(
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& guns, 
        UI::ScreenData::Home::GunWindow& screenData
    )
{
    screenData.tableSize.x = ImGui::GetContentRegionAvail().x;

    if(screenData.tableSize.x < 400)
        screenData.tableSize.x = 400;
    if(screenData.tableSize.x > 800)
        screenData.tableSize.x = 800;


    ImGui::SeparatorText( "Armory" );
    ImGui::Spacing();
    ImGui::Spacing();

    centerNextItemX(screenData.tableSize.x); 
    Tables::Selectable::gunMetadataWithRoundCount(guns, screenData.selectedGun, screenData.tableSize);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::SeparatorText( "Selected Gun" );

    if(screenData.selectedGun == EMPTY_GUN_METADATA) 
        centerTextDisabled("Select A Gun For More Information");
    else
        gunWindow_selectedGunInformation(
                guns.at(screenData.selectedGun.cartridge).at(screenData.selectedGun)
            );
}
void gunWindow_selectedGunInformation(const ArmoryGun& gun){
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
    }
    ImGui::EndChild();
}
void eventsWindow(
        const std::map<ShootingEventMetadata, ShootingEvent>& events, 
        UI::ScreenData::Home::EventsWindow& screenData 
    )
{
    screenData.tableSize.x = ImGui::GetContentRegionAvail().x;
    if(screenData.tableSize.x < 400)
        screenData.tableSize.x = 400;
    if(screenData.tableSize.x > 800)
        screenData.tableSize.x = 800;

    ImGui::SeparatorText( "Events" );
    ImGui::Spacing();
    ImGui::Spacing();
   
    centerNextItemX(screenData.tableSize.x);
    Tables::Selectable::eventsWithNumGunsUsed(events, screenData.selectedEvent, screenData.tableSize); 

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::SeparatorText( "Selected Event" );

    if(screenData.selectedEvent == EMPTY_EVENT_METADATA) {
        centerTextDisabled("Select An Event For More Information");
        return;
    }
    else
        eventsWindow_selectedEventInformation(events.at(screenData.selectedEvent));

}
void eventsWindow_selectedEventInformation(const ShootingEvent& event){
    const ShootingEventMetadata& info   { event.getInfo() };

    if(ImGui::BeginChild("Selected Event Details", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::TextDisabled("Date: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", printDate(info.date).c_str());

        ImGui::TextDisabled("Location: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", info.location.getName());

        ImGui::TextDisabled("Event Type: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", info.eventType.getName());
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Selected Event Notes", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        centerTextDisabled("Notes");
        ImGui::TextWrapped("%s", info.notes.c_str());
    }
    ImGui::EndChild();
}

void stockpileWindow(
        const std::map<Cartridge, int>& cartridgeList,
        UI::ScreenData::Home::StockpileWindow& screenData
    )
{
    screenData.tableSize.x = ImGui::GetContentRegionAvail().x;
    if(screenData.tableSize.x < 400)
        screenData.tableSize.x = 400;
    if(screenData.tableSize.x > 800)
        screenData.tableSize.x = 800;

    ImGui::SeparatorText( "Ammo Stockpile" );
    ImGui::Spacing();
    ImGui::Spacing();

    centerNextItemX(screenData.tableSize.x);
    Tables::Selectable::cartridgeAmountOnHand(cartridgeList, screenData.selectedCartridge, screenData.tableSize);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::SeparatorText( "Selected Cartridge" );

    if(screenData.selectedCartridge == EMPTY_CARTRIDGE) {
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
