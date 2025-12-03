#pragma once

#include "CommonItems.h"
#include "Backend/Database.h"

#include "Events.h"
#include "UI/UIData.h"
#include "Screens/HomeScreen.h"
#include "Screens/ViewScreen.h"
#include "Screens/AddScreen.h"
#include "Screens/EditScreen.h"


#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <memory>

namespace ShooterCentral::UI {

class Popup;

class UIController {
public:
    UIController()  = default;
    ~UIController() = default;

    void draw(const Database& database, const UnsavedChanges& unsavedChanges);

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


}   // End ShooterCentral::UI namespace
