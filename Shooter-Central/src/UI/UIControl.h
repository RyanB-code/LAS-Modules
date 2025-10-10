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

    ScreenData::Home    getScreenData_Home()    const;
    ScreenData::View    getScreenData_View()    const;
    ScreenData::Add     getScreenData_Add()     const;
    ScreenData::Edit    getScreenData_Edit()    const;

    void setScreenData(ScreenData::Home set);
    void setScreenData(ScreenData::View set);
    void setScreenData(ScreenData::Add set);
    void setScreenData(ScreenData::Edit set);


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


namespace UIEvents {

    class SetScreen : public UIEvent {
    public:
        SetScreen(const Screen& setScreen);
        ~SetScreen() = default;

        UI_EVENT_FUNCTIONS(SetScreen)
    private:
        Screen screen;
    };

    class SetScreenData_View : public UIEvent {
    public:
        SetScreenData_View(ScreenData::View set);
        ~SetScreenData_View() = default;

        UI_EVENT_FUNCTIONS(SetScreenData_View)
    private:
        ScreenData::View viewData;

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

}   // End UIEvents namespace


}   // End ShooterCentral::UI namespace
