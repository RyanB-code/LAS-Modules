#pragma once

#include "Events.h"
#include "UIControl.h"
#include "UIData.h"

namespace ShooterCentral::UI{
    class Popup;
}

namespace ShooterCentral::UI::UIEvents{



class SetScreen : public UIEvent {
public:
    SetScreen(const Screen& setScreen);
    ~SetScreen() = default;

    UI_EVENT_FUNCTIONS(SetScreen)
private:
    Screen screen;
};

class SetScreenData_Home : public UIEvent {
public:
    SetScreenData_Home(ScreenData::Home set);
    ~SetScreenData_Home() = default;

    UI_EVENT_FUNCTIONS(SetScreenData_Home)
private:
    ScreenData::Home homeData;

};

class SetScreenData_View : public UIEvent {
public:
    SetScreenData_View(ScreenData::View set);
    ~SetScreenData_View() = default;

    UI_EVENT_FUNCTIONS(SetScreenData_View)
private:
    ScreenData::View viewData;

};

class SetScreenData_Add : public UIEvent {
public:
    SetScreenData_Add(ScreenData::Add set);
    ~SetScreenData_Add() = default;

    UI_EVENT_FUNCTIONS(SetScreenData_Add)
private:
    ScreenData::Add addData;

};

class SetScreenData_Edit : public UIEvent {
public:
    SetScreenData_Edit(ScreenData::Edit set);
    ~SetScreenData_Edit() = default;

    UI_EVENT_FUNCTIONS(SetScreenData_Edit)
private:
    ScreenData::Edit editData;

}; 

class ShowPopup : public UIEvent {
public: 
    ShowPopup(std::shared_ptr<Popup>);
    ~ShowPopup() = default;

    UI_EVENT_FUNCTIONS(ShowPopup)
private:
    std::shared_ptr<Popup> popup; 
};

class ClosePopup : public UIEvent {
public:
    ClosePopup() = default;
    ~ClosePopup() = default;


    UI_EVENT_FUNCTIONS(ClosePopup)
};



class ResetAddEventWindow : public UIEvent {
public: 
    ResetAddEventWindow(const ScreenData::Add::EventWindow& data=UI::ScreenData::Add::EventWindow { } );
    ~ResetAddEventWindow() = default;

    UI_EVENT_FUNCTIONS(ResetAddEventWindow)

private:
    UI::ScreenData::Add::EventWindow screenData;
};

}
