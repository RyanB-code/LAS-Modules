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

    Add_EventWindow::Add_EventWindow(const ScreenData::Add::EventWindow& data) : screenData { data }
    {

    }
    Status Add_EventWindow::execute(UIController& controller) {
        auto addWindow { controller.getScreenData_Add() };
        addWindow.eventWindow = screenData;

        SetScreenData::Add amend {addWindow};
        pushEvent(&amend);
        return Status{true};
    }
    Add_GunWindow::Add_GunWindow(const ScreenData::Add::GunWindow& data) : screenData { data }
    {

    }
    Status Add_GunWindow::execute(UIController& controller) {
        auto addWindow { controller.getScreenData_Add() };
        addWindow.gunWindow = screenData;

        SetScreenData::Add amend {addWindow};
        pushEvent(&amend);
        return Status{true};
    }

    Edit::Edit(ScreenData::Edit set) : editData { set } {
            
    }
    Status Edit::execute (UIController& controller){
        controller.setScreenData(editData);
        return Status{true};
    }
}   // SetScreenData namespace

PushPopup::PushPopup(Popup* set) {
    popupPtr = set->clone();
}
Status PushPopup::execute(UIController& controller) {
    if(controller.pushPopup(popupPtr.get()))
        return Status { true };

    return Status { false, "UIController::pushPopup(UI::Popup*) failed" };
}
std::unique_ptr<UIEvent> PushPopup::clone() const{ 
    auto buffer { std::make_unique<PushPopup>( popupPtr.get()  ) };
    return buffer;
}




}   // SC namespace
