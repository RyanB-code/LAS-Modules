#pragma once

#include "CommonItems.h"
#include "Containers.h"

#include "Events.h"
#include "UI/UIData.h"
#include "UI/UIDrawing.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <map>
#include <memory>

#include <iostream> // For testing


namespace ShooterCentral::UI {

class Popup;

class UIController {
public:
    UIController();
    ~UIController();

    void draw(const Containers& containers, const UnsavedChanges& unsavedChanges);

    void setScreen(const Screen& screen);
    bool setPopup(std::shared_ptr<Popup> popup);
    void closePopup();

private:
    Screen currentScreen { Screen::HOME };

    std::shared_ptr<Popup>  popup;

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
    ShowPopup(std::shared_ptr<Popup>);
    ~ShowPopup() = default;

    virtual Status execute(UIController& controller) override;

    UI_EVENT_CLONE(ShowPopup)
private:
    std::shared_ptr<Popup> popup; 
};

class ClosePopup : public UIEvent {
public:
    ClosePopup() = default;
    ~ClosePopup() = default;

    virtual Status execute(UIController& controller) override;

    UI_EVENT_CLONE(ClosePopup)
};


}   // End UI namespace
