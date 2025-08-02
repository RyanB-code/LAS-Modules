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

    class SelectAmmo {
    public:
        SelectAmmo();
        ~SelectAmmo();

        void setCartridgeMap (std::map<Cartridge, std::map<AmmoMetadata, AssociatedAmmo>>::const_iterator set);
        std::map<Cartridge, std::map<AmmoMetadata, AssociatedAmmo>>::const_iterator getCartridgeMap();

        bool select(const AmmoMetadata& key);
        std::map<AmmoMetadata, AssociatedAmmo>::const_iterator getSelected();
    private:
        std::map<Cartridge, std::map<AmmoMetadata, AssociatedAmmo>>::const_iterator selectedMap;
        std::map<AmmoMetadata, AssociatedAmmo>::const_iterator selectedAmmo;
    };

    class SelectGun {
    public:
        SelectGun();
        ~SelectGun();

        void setCartridgeMap (std::map<Cartridge, std::map<GunMetadata, AssociatedGun>>::const_iterator set);
        std::map<Cartridge, std::map<GunMetadata, AssociatedGun>>::const_iterator getCartridgeMap();

        bool select(const GunMetadata& key);
        std::map<GunMetadata, AssociatedGun>::const_iterator getSelected();
    private:
        std::map<Cartridge, std::map<GunMetadata, AssociatedGun>>::const_iterator selectedMap;
        std::map<GunMetadata, AssociatedGun>::const_iterator selectedGun;
    };

    struct ScreenData_Home {
        bool showGuns       { true };
        bool showEvents     { true };
        bool showStockpile  { true };

        std::map<Event, std::shared_ptr<Event>>::const_iterator     selectedEvent;
        SelectAmmo  selectedAmmo;
        SelectGun   selectedGun;
    };
    struct ScreenData_View {
        Category category { Category::NONE }; 

        std::map<Event, std::shared_ptr<Event>>::const_iterator     selectedEvent;
        std::map<Cartridge, std::map<AmmoMetadata, AssociatedAmmo>>::const_iterator      selectedAmmo;
        std::map<Cartridge, std::map<GunMetadata, AssociatedGun>>::const_iterator        selectedGun;
    };
    struct ScreenData_Add {
        Category category   { Category::NONE }; 
        SubItem  subItem    { SubItem::NONE };
    };
    struct ScreenData_Edit {
        Category category { Category::NONE }; 

        std::map<Event, std::shared_ptr<Event>>::const_iterator     selectedEvent;
        std::map<Cartridge, std::map<AmmoMetadata, AssociatedAmmo>>::const_iterator      selectedAmmo;
        std::map<Cartridge, std::map<GunMetadata, AssociatedGun>>::const_iterator        selectedGun;
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

    void draw_Home      (const Containers& containers, ScreenData_Home& data, const UnsavedChanges& changes);
    void draw_HomeGuns  (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, SelectGun& selected );
    void draw_HomeEvents(const std::map<Event, std::shared_ptr<Event>>& events, std::map<Event, std::shared_ptr<Event>>::const_iterator& selected );
    void draw_HomeStockpile (   std::map<Cartridge, std::map<AmmoMetadata,  AssociatedAmmo>>::const_iterator begin,
                                std::map<Cartridge, std::map<AmmoMetadata,  AssociatedAmmo>>::const_iterator end,
                                SelectAmmo& selected
                            );





}
