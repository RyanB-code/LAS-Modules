#include "UI/UIControl.h"

namespace ShooterCentral::UI {

void UIController::draw(const Database& database, const UnsavedChanges& unsavedChanges) {
    // Stores last screen, checks if equal to current screen (set after ImGui does its thing)
    // If not equal, it means an Event changed the screen, so force ImGui to show which tab we want
    // from the Event

    if(displayedPopup){
        if(displayedPopup->wasCloseCalled())
            displayedPopup = nullptr;
    }

    if(!popupQueue.empty() && !displayedPopup ){
        displayedPopup = std::move(popupQueue.front());
        popupQueue.pop();
    }

    if(displayedPopup){
        ImGui::OpenPopup(displayedPopup->getTitle());

        if(ImGui::BeginPopupModal(displayedPopup->getTitle(), NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            displayedPopup->show();
            ImGui::EndPopup();
        }
   }

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


    switch(currentScreen){
        case Screen::HOME:
            Home::main(database, homeData, unsavedChanges);
            break;
        case Screen::VIEW:
            View::main(database, viewData);
            break;
        case Screen::ADD:
            Add::main(database, addData);
            break;
        case Screen::EDIT:
            Edit::main(database, editData);
            break;
        default:
            LAS::log_warn("SC Screen case not handled");
            break;
    }

}
void UIController::setScreen(const Screen& screen){
    currentScreen = screen;
}
bool UIController::pushPopup(Popup* popup){
    if(!popup)
        return false;

    std::unique_ptr<Popup> ptr {popup->clone()};

    popupQueue.push(std::move(ptr));
    return true;
}

ScreenData::Home UIController::getScreenData_Home() const{
    return homeData;
}
ScreenData::View UIController::getScreenData_View() const{
    return viewData;
}
ScreenData::Add  UIController::getScreenData_Add() const{
    return addData;
}
ScreenData::Edit UIController::getScreenData_Edit() const{
    return editData;
}

void UIController::setScreenData(ScreenData::Home set) {
    homeData = set;
}
void UIController::setScreenData(ScreenData::View set) {
    viewData = set;
}
void UIController::setScreenData(ScreenData::Add set) {
    addData = set;
}
void UIController::setScreenData(ScreenData::Edit set) {
    editData = set;
}


}   // End SC::UI namespace
