#include "View.h"

namespace ShooterCentral::View{

GUI::GUI() {

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
            drawHome(homeData, unsavedChanges);
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


void drawHome (ScreenData_Home& data, const UnsavedChanges& changes) {
    
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


    if(data.showGuns) {
        ImGui::Text("showing guns");
    }
}

} // End namespace
