#include "UIEvents.h"

namespace ShooterCentral::UI::UIEvents {

SetScreen::SetScreen(const Screen& setScreen) : screen { setScreen } {

}
Status SetScreen::execute (UIController& controller){
    controller.setScreen(screen);
    return Status{true};
}

namespace SetScreenData {

    Home::Home(ScreenData::Home set) : homeData { set } {
            
    }
    Status Home::execute (UIController& controller){
        controller.setScreenData(homeData);
        return Status{true};
    }
    View::View(ScreenData::View set) : viewData { set } {
            
    }
    Status View::execute (UIController& controller){
        controller.setScreenData(viewData);
        return Status{true};
    }

    Add::Add(ScreenData::Add set) : addData { set } {
            
    }
    Status Add::execute (UIController& controller){
        controller.setScreenData(addData);
        return Status{true};
    }

    Edit::Edit(ScreenData::Edit set) : editData { set } {
            
    }
    Status Edit::execute (UIController& controller){
        controller.setScreenData(editData);
        return Status{true};
    }
}   // SetScreenData namespace

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

    SetScreenData::Add amend {addWindow};
    pushEvent(&amend);
    return Status{true, "test status resetAddEventWindow"};
}

}   // SC namespace
