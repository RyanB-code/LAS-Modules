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

namespace SetScreenData {

    class Home : public UIEvent {
    public:
        Home(ScreenData::Home set);
        ~Home() = default;

        UI_EVENT_FUNCTIONS(Home)
    private:
        ScreenData::Home homeData;

    };

    class View : public UIEvent {
    public:
        View(ScreenData::View set);
        ~View() = default;

        UI_EVENT_FUNCTIONS(View)
    private:
        ScreenData::View viewData;

    };

    class Add : public UIEvent {
    public:
        Add(ScreenData::Add set);
        ~Add() = default;

        UI_EVENT_FUNCTIONS(Add)
    private:
        ScreenData::Add addData;

    };

    class Add_EventWindow : public UIEvent {
        public: 
            Add_EventWindow(const ScreenData::Add::EventWindow& data=UI::ScreenData::Add::EventWindow { } );
            ~Add_EventWindow() = default;

            UI_EVENT_FUNCTIONS(Add_EventWindow)

        private:
            UI::ScreenData::Add::EventWindow screenData;
    };

    class Edit : public UIEvent {
    public:
        Edit(ScreenData::Edit set);
        ~Edit() = default;

        UI_EVENT_FUNCTIONS(Edit)
    private:
        ScreenData::Edit editData;

    }; 

}   // SetScreenData namespace


class PushPopup : public UIEvent {
public: 
    PushPopup(Popup* popup);
    ~PushPopup() = default;

    virtual Status execute (UIController& ) override;
    virtual std::unique_ptr<UIEvent> clone() const override;

private:
    std::unique_ptr<Popup> popupPtr { }; 
};




}
