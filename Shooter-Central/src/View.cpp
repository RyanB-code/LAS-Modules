#include "View.h"

namespace ShooterCentral::View{

GUI::GUI(){

}
GUI::~GUI() {

}
void GUI::draw(const Containers& containers, const UnsavedChanges& unsavedChanges) {

     if(ImGui::BeginTabBar("Tabs")){
        if(ImGui::BeginTabItem("Home")){
            currentScreen = Screen::HOME;
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("View")){
            currentScreen = Screen::VIEW;
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add")){
            currentScreen = Screen::ADD;
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Edit")){
            currentScreen = Screen::EDIT;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    switch(currentScreen){
        case Screen::HOME:
            draw_Home(containers, homeData, unsavedChanges);
            break;
        case Screen::VIEW:

            break;
        case Screen::ADD:

            break;
        case Screen::EDIT:

            break;
        default:
            LAS::log_warn("SC Screen case not handled");
            break;
    }
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
void draw_Home (const Containers& containers, ScreenData_Home& data, const UnsavedChanges& changes) {
    
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
        if(ImGui::BeginChild("Home Guns", childWindowSizes, 0) ){
            draw_HomeGuns(containers.getGunsInArmory(), data.selectedGun);
        }
        ImGui::EndChild();
    }

    if(horizontalLayout)
        ImGui::SameLine();

    if(data.showEvents){
        if(ImGui::BeginChild("Home Events", childWindowSizes, 0) ){
            draw_HomeEvents(containers.getEvents(), data.selectedEvent);
        }
        ImGui::EndChild();
    }

    if(horizontalLayout)
        ImGui::SameLine();

    if(data.showStockpile){
        if(ImGui::BeginChild("Home Stockpile", childWindowSizes, 0) ){
            draw_HomeStockpile(containers.getAmmoStockpile(), data.selectedAmmo); 
        }
        ImGui::EndChild();
    }
}
void draw_HomeGuns  (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, std::weak_ptr<AssociatedGun>& weakSelected ){

    // Obtain lock on selected
    std::shared_ptr<AssociatedGun> selected { weakSelected.lock() };

    if(!selected)
        weakSelected.reset();


    // Size table correctly
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};

    if(tableSize.x < 400)
        tableSize.x = 400;
    if(tableSize.x > 800)
        tableSize.x = 800;

    ImGui::SeparatorText( "Guns In Armory" );
    ImGui::Spacing();

    // Center the table
    float windowWidth { ImGui::GetContentRegionAvail().x };
    if(tableSize.x < windowWidth)
        ImGui::SetCursorPosX((windowWidth - tableSize.x) * 0.5f);

    int row { 0 };
    if(ImGui::BeginTable("Guns Table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, tableSize)) {
        ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Round Count",  ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& [cartridge, map] : guns){
            // Do not sort by Cartridge so just ignore it
            for(const auto& [gunMetadata, assocGunPtr] : map){

                if(!*assocGunPtr) // bool check to see if gun is valid
                    continue;

                const GunMetadata& gun {assocGunPtr->getGun()};
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
   
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent(20);
    ImGui::Text("Guns In Armory:       %d", row); 
    ImGui::Unindent(20);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText( "Selected Gun" );

    if(!selected) {
        centerText("Select A Gun For More Information");
        return;
    }

    // Show selected gun information
    const AssociatedGun& selectedGun    { *selected };
    const GunMetadata& selectedGunInfo  { selectedGun.getGun() };
    int roundCount { selectedGun.getRoundCount() };
    int eventsUsed { selectedGun.totalEventsUsed() };

    if(ImGui::BeginChild("Selected Gun Details Left", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::Text("Name:          %s", selectedGunInfo.name.c_str());
        ImGui::Text("Weapon Type:   %s", selectedGunInfo.weaponType.getName().c_str()); 
        ImGui::Text("Cartridge:     %s", selectedGunInfo.cartridge.getName().c_str());
        ImGui::Unindent(20);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Selected Gun Details Right", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Text("Round Count:       %d", roundCount);
        ImGui::Text("Events Used In:    %d", eventsUsed); 
        //ImGui::Text("Last Event:        %s", 
    }
    ImGui::EndChild();
}

void draw_HomeEvents(const std::map<EventMetadata, std::shared_ptr<Event>>& events, std::weak_ptr<Event>& weakSelected ){

    // Obtain lock on selected
    std::shared_ptr<Event> selected { weakSelected.lock() };

    if(!selected)
        weakSelected.reset();

    // Size the table
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};

    if(tableSize.x < 400)
        tableSize.x = 400;
    if(tableSize.x > 800)
        tableSize.x = 800;

    ImGui::SeparatorText( "Events" );
    ImGui::Spacing();

    // Center the table
    float windowWidth { ImGui::GetContentRegionAvail().x };
    if(tableSize.x < windowWidth)
        ImGui::SetCursorPosX((windowWidth - tableSize.x) * 0.5f);

    int row { 0 };
    if(ImGui::BeginTable("Event Table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, tableSize )){
        ImGui::TableSetupColumn("Date",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Event Type",   ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Location",     ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Guns Used",    ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& [eventMetadata, eventPtr] : events){
            const Event& event      { *eventPtr };
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
                        ImGui::Text("%s", event.getEventType().getName().c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", event.getLocation().getName().c_str());
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

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Indent(20);
    ImGui::Text("Events:       %d", row); 
    ImGui::Unindent(20);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText( "Selected Event" );

    if(!selected) {
        centerText("Select An Event For More Information");
        return;
    }

    
    // Show selected event information
    const Event& selectedEvent          { *selected };
    const EventMetadata& selectedInfo   { selectedEvent.getInfo() };

    if(ImGui::BeginChild("Selected Event Details", ImVec2{ImGui::GetContentRegionAvail().x/2, 75}, 0)){
        ImGui::Indent(20);
        ImGui::TextDisabled("Date: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", selectedEvent.printDate().c_str());

        ImGui::TextDisabled("Location: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", selectedEvent.getLocation().getName().c_str());

        ImGui::TextDisabled("Event Type: ");
        ImGui::SameLine(100);
        ImGui::Text("%s", selectedEvent.getEventType().getName().c_str());
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if(ImGui::BeginChild("Selected Event Notes", ImVec2{ImGui::GetContentRegionAvail().x, 75}, 0)){
        centerTextDisabled("Notes");
        ImGui::TextWrapped("%s", selectedEvent.getNotes().c_str());
    }
    ImGui::EndChild();
}
void draw_HomeStockpile(const std::map<Cartridge, std::map<AmmoMetadata, std::shared_ptr<AssociatedAmmo>>>& ammoList, std::weak_ptr<AssociatedAmmo>& weakSelected){
    // Obtain lock on selected
    std::shared_ptr<AssociatedAmmo> selected { weakSelected.lock() };

    if(!selected)
        weakSelected.reset();

    // Size the table
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x-2, 200};

    if(tableSize.x < 400)
        tableSize.x = 400;
    if(tableSize.x > 800)
        tableSize.x = 800;

    ImGui::SeparatorText( "Ammo Stockpile" );
    ImGui::Spacing();

    // Center the table
    float windowWidth { ImGui::GetContentRegionAvail().x };
    if(tableSize.x < windowWidth)
        ImGui::SetCursorPosX((windowWidth - tableSize.x) * 0.5f);

    int row { 0 };

    // NEED TO SHOW BREAKDOWN OF STOCKPILE
    /*

    if(ImGui::BeginTable("Stockpile", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, tableSize )) {
        ImGui::TableSetupColumn("Cartridge",    0);
        ImGui::TableSetupColumn("Amount",       0);
        ImGui::TableHeadersRow();

        if(itr == selected)
            isEventSelected = true;
            
        ImGui::PushID(std::to_string(row).c_str());
        ImGui::TableNextRow();

        for (int column{0}; column < 2; ++column){        
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
    */
}




} // End view namespace
