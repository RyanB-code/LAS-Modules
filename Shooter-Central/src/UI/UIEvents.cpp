#include "UIEvents.h"

namespace ShooterCentral::UI::UIEvents {

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

ResetAddEventWindow::ResetAddEventWindow(const ScreenData::Add::EventWindow& data) : screenData { data }
{

}
Status ResetAddEventWindow::execute(UIController& controller) {
    auto addWindow { controller.getScreenData_Add() };
    addWindow.eventWindow = screenData;

    SetScreenData_Add amend {addWindow};
    pushEvent(&amend);
    return Status{true, "test status resetAddEventWindow"};
}

}   // SC namespace
