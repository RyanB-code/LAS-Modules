#pragma once

#include "AssociatedItems.h"
#include "CommonItems.h"

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

        std::map<Event, std::shared_ptr<Event>>::const_iterator     selectedEvent;
        std::map<AmmoMetadata, AssociatedAmmo>::const_iterator      selectedAmmo;
        std::map<GunMetadata, AssociatedGun>::const_iterator        selectedGun;
    };
    struct ScreenData_View {
        Category category { Category::NONE }; 

        std::map<Event, std::shared_ptr<Event>>::const_iterator     selectedEvent;
        std::map<AmmoMetadata, AssociatedAmmo>::const_iterator      selectedAmmo;
        std::map<GunMetadata, AssociatedGun>::const_iterator        selectedGun;
    };
    struct ScreenData_Add {
        Category category   { Category::NONE }; 
        SubItem  subItem    { SubItem::NONE };
    };
    struct ScreenData_Edit {
        Category category { Category::NONE }; 

        std::map<Event, std::shared_ptr<Event>>::const_iterator     selectedEvent;
        std::map<AmmoMetadata, AssociatedAmmo>::const_iterator      selectedAmmo;
        std::map<GunMetadata, AssociatedGun>::const_iterator        selectedGun;
    };


    class GUI {
    public:
        GUI(const ContainerItrs& itrs);
        ~GUI();

        void draw(const ContainerItrs& itrs, const UnsavedChanges& unsavedChanges);

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

    void draw_Home      (const ContainerItrs& itrs, ScreenData_Home& data, const UnsavedChanges& changes);
    void draw_HomeGuns  (   std::map<GunMetadata, AssociatedGun>::const_iterator begin, 
                            std::map<GunMetadata, AssociatedGun>::const_iterator end, 
                            std::map<GunMetadata, AssociatedGun>::const_iterator& selected
                        );
    void draw_HomeEvents (  std::map<Event, std::shared_ptr<Event>>::const_iterator begin,
                            std::map<Event, std::shared_ptr<Event>>::const_iterator end,
                            std::map<Event, std::shared_ptr<Event>>::const_iterator& selected
                        );
    void draw_HomeStockpile (   std::map<AmmoMetadata,  AssociatedAmmo>::const_iterator begin,
                                std::map<AmmoMetadata,  AssociatedAmmo>::const_iterator end,
                                std::map<AmmoMetadata,  AssociatedAmmo>::const_iterator& selected
                            );





}
