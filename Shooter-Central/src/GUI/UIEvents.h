#pragma once

#include "Events.h"

#include "GUI/UIData.h"
#include "GUI/UIController.h"

namespace ShooterCentral::UI {

    // Events
    class SetScreen : public UIEvent {
    public:
        SetScreen(const Screen& setScreen);
        ~SetScreen();

        virtual Status execute (UIController& controller) override;

        UI_EVENT_CLONE(SetScreen)
    private:
        Screen screen;
    };

    class ShowPopup : public UIEvent {
    public: 
        ShowPopup(const char* msg);
        ~ShowPopup();

        virtual Status execute(UIController& controller) override;

        UI_EVENT_CLONE(ShowPopup)
    private:
        char text[512];
    };

} // End UI namespace
