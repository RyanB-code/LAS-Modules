#include "View.h"

namespace ShooterCentral::View{

GUI::GUI(const ContainerItrs& itrs){

    homeData.selectedEvent = itrs.events_cend;
    homeData.selectedAmmo = itrs.ammoStockpile_cend;
    homeData.selectedGun = itrs.gunsInArmory_cend;

    viewData.selectedEvent = itrs.events_cend;
    viewData.selectedAmmo = itrs.ammoStockpile_cend;
    viewData.selectedGun = itrs.gunsInArmory_cend;

    editData.selectedEvent = itrs.events_cend;
    editData.selectedAmmo = itrs.ammoStockpile_cend;
    editData.selectedGun = itrs.gunsInArmory_cend;

}
GUI::~GUI() {

}
void GUI::draw(const ContainerItrs& itrs, const UnsavedChanges& unsavedChanges) {

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
            draw_Home(itrs, homeData, unsavedChanges);
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

void draw_Home (const ContainerItrs& itrs, ScreenData_Home& data, const UnsavedChanges& changes) {
    
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
        if(ImGui::BeginChild("Home_Guns", childWindowSizes, 0) ){
            draw_HomeGuns(itrs.gunsInArmory_cbegin, itrs.gunsInArmory_cend, data.selectedGun);
        }
        ImGui::EndChild();
    }
}
void draw_HomeGuns  (   std::map<GunMetadata, AssociatedGun>::const_iterator begin, 
                        std::map<GunMetadata, AssociatedGun>::const_iterator end, 
                        std::map<GunMetadata, AssociatedGun>::const_iterator& selected
                    )
{
    ImVec2 tableSize { ImGui::GetContentRegionAvail().x, 200};

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

        for(auto itr { begin }; itr != end; ++itr){
            if(!itr->second) // bool check to see if gun is valid
                continue;

            const GunMetadata& gun {itr->second.getGun()};
            bool isGunSelected { false };
            
            if(gun == selected->second.getGun())
                isGunSelected = true;

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column){
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Selectable(gun.weaponType.getName().c_str(), &isGunSelected, ImGuiSelectableFlags_SpanAllColumns);

                        if(isGunSelected)
                            selected = itr;

                        break;
                    case 1:
                        ImGui::Text("%s", gun.cartridge.getName().c_str());
                        break;
                    case 2:
                        ImGui::Text("%s", gun.name.c_str());
                        break;
                    case 3:
                        ImGui::Text("%d", itr->second.getRoundCount());
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

    ImGui::Indent(20);
    ImGui::Text("Guns In Armory:       %d", row); 
    ImGui::Unindent(20);

    // Next is selected gun information

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SeparatorText( "Selected Gun" );

    if(selected == end) {
        centerText("Select A Gun For More Information");
        return;
    }

    const AssociatedGun& selectedGun    { selected->second };
    const GunMetadata& selectedGunInfo  { selectedGun.getGun() };
    int roundCount { selectedGun.getRoundCount() };
    int eventsUsed { selectedGun.totalEventsUsed() };

    // Gun Details
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
        ImGui::Indent(20);
        ImGui::Text("Round Count:       %d", roundCount);
        ImGui::Text("Events Used In:    %d", eventsUsed); 
        //ImGui::Text("Last Event:        %s", 
        ImGui::Unindent(20);
    }
    ImGui::EndChild();
}


} // End namespace
