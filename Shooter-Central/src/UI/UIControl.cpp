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

    if(popup){
        ImGui::OpenPopup(popup->getTitle());

        if(ImGui::BeginPopupModal(popup->getTitle(), NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            popup->show();
            ImGui::EndPopup();
        }
    }

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
bool UIController::setPopup(std::shared_ptr<Popup> set){
    if(!set)
        return false;

    popup = set;
    return true;
}
void UIController::closePopup() {
    popup = nullptr;
}


namespace UIEvents {

    SetScreen::SetScreen(const Screen& setScreen) : screen { setScreen } {
            
    }
    Status SetScreen::execute (UIController& controller){
        controller.setScreen(screen);
        return Status{true};
    }

    ShowPopup::ShowPopup(std::shared_ptr<Popup> set) : popup { set } {

    }
    Status ShowPopup::execute(UIController& controller) {
        if(controller.setPopup(popup))
            return Status { true };

        return Status { false, "UIController::setPopup() failed" };
    }

    Status ClosePopup::execute(UIController& controller) {
        controller.closePopup();
        return Status { true };
    }


    }   // End UIEvents namespace

}   // End SC::UI namespace
