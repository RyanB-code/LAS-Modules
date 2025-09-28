#include "UI/UIControl.h"

namespace ShooterCentral::UI {

UIController::UIController(){

}
UIController::~UIController() {

}
void UIController::draw(const Containers& containers, const UnsavedChanges& unsavedChanges) {
    // Stores last screen, checks if equal to current screen (set after ImGui does its thing)
    // If not equal, it means an Event changed the screen, so force ImGui to show which tab we want
    // from the Event

    static Screen lastScreen;
    ImGuiTabItemFlags homeFlags { 0 }, viewFlags { 0 }, addFlags { 0 }, editFlags { 0 };
    bool applyForce { false };

    if(lastScreen != currentScreen)
        applyForce = true;

    if(applyForce){
        switch(currentScreen){
            case Screen::HOME:
                homeFlags |= ImGuiTabItemFlags_SetSelected;
                break;
            case Screen::VIEW:
                viewFlags |= ImGuiTabItemFlags_SetSelected;
                break;
            case Screen::ADD:
                addFlags |= ImGuiTabItemFlags_SetSelected;
                break;
            case Screen::EDIT:
                editFlags |= ImGuiTabItemFlags_SetSelected;
                break;
            default:
                LAS::log_warn("SC Screen case not handled");
                break;
        }
    }

    if(ImGui::BeginTabBar("Tabs")){
        if(ImGui::BeginTabItem("Home", nullptr, homeFlags)){
            currentScreen = Screen::HOME;
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("View", nullptr, viewFlags)){
            currentScreen = Screen::VIEW;
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Add", nullptr, addFlags)){
            currentScreen = Screen::ADD;
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Edit", nullptr, editFlags)){
            currentScreen = Screen::EDIT;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    lastScreen = currentScreen;

    if(popUp.shown)
        displayPopUp(popUp.text);

    switch(currentScreen){
        case Screen::HOME:
            Home::main(containers, homeData, unsavedChanges);
            break;
        case Screen::VIEW:
            View::main(containers, viewData);
            break;
        case Screen::ADD:
            Add::main(containers, addData);
            break;
        case Screen::EDIT:

            break;
        default:
            LAS::log_warn("SC Screen case not handled");
            break;
    }

}
void UIController::setScreen(const Screen& screen){
    currentScreen = screen;
}


SetScreen::SetScreen(const Screen& setScreen) : screen { setScreen } {
        
}
SetScreen::~SetScreen(){

}
Status SetScreen::execute (UIController& controller){
    controller.setScreen(screen);
    return Status{true};
}
ShowPopup::ShowPopup(const char* msg) {

}
ShowPopup::~ShowPopup(){

}
Status ShowPopup::execute(UIController& controller) {
    return Status { false };
}


} // End UI namespace
