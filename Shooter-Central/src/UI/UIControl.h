#pragma once

#include "CommonItems.h"
#include "Containers.h"

#include "Events.h"
#include "UI/UIData.h"
#include "UI/UIDrawing.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <map>

#include <iostream> // For testing


namespace ShooterCentral::UI {

class UIController {
public:
    UIController();
    ~UIController();

    void draw(const Containers& containers, const UnsavedChanges& unsavedChanges);

    void setScreen(const Screen& screen);

private:
    Screen currentScreen { Screen::HOME };

    ScreenData::PopUpInfo popUp { };
    ScreenData::Home homeData   { };
    ScreenData::View viewData   { };
    ScreenData::Add  addData    { };
    ScreenData::Edit editData   { };


    void resetAllScreens();
    void resetScreen(Screen screen);

    void resetGuns();
    void resetAmmo();
    void resetEvents();
};

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


}   // End UI namespace
