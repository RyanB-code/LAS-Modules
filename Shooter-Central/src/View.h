#pragma once

#include "AssociatedItems.h"
#include "CommonItems.h"
#include "Containers.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <map>

#include <iostream>


namespace ShooterCentral::View {

    enum class Screen {
        HOME,
        VIEW,
        ADD,
        EDIT,
        SETTINGS
    };
    enum class Category {
        NONE,
        GUNS,
        EVENTS,
        STOCKPILE
    };
    enum class SubItem {
        NONE,

        EVENT_EVENT,
        EVENT_TYPE,
        EVENT_LOCATION,

        AMMO_EXISTING,
        AMMO_MANUFACTURER,
        AMMO_CARTRIDGE,

        GUN_GUN,
        GUN_TYPE
    };

    struct ScreenData_Home {
        bool showGuns       { true };
        bool showEvents     { true };
        bool showStockpile  { true };

        std::weak_ptr<Event>            selectedEvent;
        std::weak_ptr<AssociatedAmmo>   selectedAmmo;
        std::weak_ptr<AssociatedGun>    selectedGun;
    };
    struct ScreenData_View {
        Category category { Category::NONE }; 

        std::weak_ptr<Event>            selectedEvent;
        std::weak_ptr<AssociatedAmmo>   selectedAmmo;
        std::weak_ptr<AssociatedGun>    selectedGun;
    };
    struct ScreenData_Add {
        Category category   { Category::NONE }; 
        SubItem  subItem    { SubItem::NONE };
    };
    struct ScreenData_Edit {
        Category category { Category::NONE }; 

        std::weak_ptr<Event>            selectedEvent;
        std::weak_ptr<AssociatedAmmo>   selectedAmmo;
        std::weak_ptr<AssociatedGun>    selectedGun;
    };


    class GUI {
    public:
        GUI();
        ~GUI();

        void draw(const Containers& containers, const UnsavedChanges& unsavedChanges);

    private:
        Screen currentScreen { Screen::HOME };

        ScreenData_Home homeData    { };
        ScreenData_View viewData    { };
        ScreenData_Add  addData     { };
        ScreenData_Edit editData    { };


        void resetAllScreens();
        void resetScreen(Screen screen);

        void resetGuns();
        void resetAmmo();
        void resetEvents();

    };

    void centerText(const std::string& text);
    void centerTextDisabled(const std::string& text);
    bool centerButton(const std::string& text, ImVec2 size);

    void draw_Home      (const Containers& containers, ScreenData_Home& data, const UnsavedChanges& changes);
    void draw_HomeGuns  (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, std::weak_ptr<AssociatedGun>& selected );
    void draw_HomeEvents(const std::map<EventMetadata, std::shared_ptr<Event>>& events, std::weak_ptr<Event>& selected );
    void draw_HomeStockpile(const std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>>& ammoList, std::weak_ptr<AssociatedAmmo>& selected);
}
