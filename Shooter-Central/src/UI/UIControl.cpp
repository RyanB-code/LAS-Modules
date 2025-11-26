#include "UI/UIControl.h"

namespace ShooterCentral::UI {

void UIController::draw(const Database& database, const UnsavedChanges& unsavedChanges) {
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

    if(popup){
        ImGui::OpenPopup(popup->getTitle());

        if(ImGui::BeginPopupModal(popup->getTitle(), NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            popup->show();
            ImGui::EndPopup();
        }
    }

    switch(currentScreen){
        case Screen::HOME:
            Home::main(database, homeData, unsavedChanges);
            break;
        /*
        case Screen::VIEW:
            View::main(containers, viewData);
            break;
        case Screen::ADD:
            Add::main(containers, addData);
            break;
        case Screen::EDIT:

            break;
        */
        default:
            LAS::log_warn("SC Screen case not handled");
            break;
    }

}
void UIController::setScreen(const Screen& screen){
    currentScreen = screen;
}
bool UIController::setPopup(std::shared_ptr<Popup> set){
    if(!set)
        return false;

    popup = set;
    return true;
}
void UIController::closePopup() {
    popup = nullptr;
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


namespace UIEvents {
    SetScreen::SetScreen(const Screen& setScreen) : screen { setScreen } {

    }
    Status SetScreen::execute (UIController& controller){
        controller.setScreen(screen);
        return Status{true};
    }

    SetScreenData_Home::SetScreenData_Home(ScreenData::Home set) : homeData { set } {
            
    }
    Status SetScreenData_Home::execute (UIController& controller){
        controller.setScreenData(homeData);
        return Status{true};
    }
    SetScreenData_View::SetScreenData_View(ScreenData::View set) : viewData { set } {
            
    }
    Status SetScreenData_View::execute (UIController& controller){
        controller.setScreenData(viewData);
        return Status{true};
    }

    SetScreenData_Add::SetScreenData_Add(ScreenData::Add set) : addData { set } {
            
    }
    Status SetScreenData_Add::execute (UIController& controller){
        controller.setScreenData(addData);
        return Status{true};
    }

    SetScreenData_Edit::SetScreenData_Edit(ScreenData::Edit set) : editData { set } {
            
    }
    Status SetScreenData_Edit::execute (UIController& controller){
        controller.setScreenData(editData);
        return Status{true};
    }

}   // End UIEvents namespace

}   // End SC::UI namespace
