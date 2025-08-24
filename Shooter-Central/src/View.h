#pragma once

#include "AssociatedItems.h"
#include "CommonItems.h"
#include "Containers.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <map>

#include <iostream>


namespace ShooterCentral::UI {

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
        STOCKPILE,

        SIZE
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

    // To iterate over categories
    static constexpr std::array<Category, static_cast<size_t>(Category::SIZE)> CATEGORY_LIST {
        Category::NONE,
        Category::GUNS,
        Category::EVENTS,
        Category::STOCKPILE
    };


    static const Cartridge  EMPTY_CARTRIDGE { "NULL" };

    // Holds data for what is shown that remains between frames
    namespace ScreenData{
        struct Home{
            bool showGuns       { true };
            bool showEvents     { true };
            bool showStockpile  { true };

            std::weak_ptr<Event>            selectedEvent;
            std::weak_ptr<AssociatedGun>    selectedGun;
            std::weak_ptr<Cartridge>        selectedCartridge;
        };
        struct View {
            struct EventTab {
                std::weak_ptr<Event>            selectedEvent;
                std::weak_ptr<GunMetadata>      selectedGun;
            };
            struct GunTab{
                std::weak_ptr<AssociatedGun>    selectedGun;
                std::weak_ptr<Event>            selectedEvent;
            };

            Category category { Category::NONE }; 

            EventTab eventTab { };
            GunTab   gunTab { };

            std::weak_ptr<AssociatedAmmo>       selectedAmmo;

            std::string categoryComboBoxText { };
        };
        struct Add {
            Category category   { Category::NONE }; 
            SubItem  subItem    { SubItem::NONE };
        };
        struct Edit {
            Category category { Category::NONE }; 

            std::weak_ptr<Event>            selectedEvent;
            std::weak_ptr<AssociatedAmmo>   selectedAmmo;
            std::weak_ptr<AssociatedGun>    selectedGun;
        };
    }



    class UIController {
    public:
        UIController();
        ~UIController();

        void draw(const Containers& containers, const UnsavedChanges& unsavedChanges);

    private:
        Screen currentScreen { Screen::HOME };

        ScreenData::Home homeData    { };
        ScreenData::View viewData    { };
        ScreenData::Add  addData     { };
        ScreenData::Edit editData    { };


        void resetAllScreens();
        void resetScreen(Screen screen);

        void resetGuns();
        void resetAmmo();
        void resetEvents();
    };


    namespace Home {
        void main                               (const Containers& containers, ScreenData::Home& data, const UnsavedChanges& changes);

        void gunWindow                          (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, std::weak_ptr<AssociatedGun>& selected );
        void gunWindow_selectedGunInformation   (std::shared_ptr<AssociatedGun> selected);
        void eventsWindow                       (const std::map<EventMetadata, std::shared_ptr<Event>>& events, std::weak_ptr<Event>& selected );
        void stockpileWindow                    (const std::map<Cartridge, int>& cartridgeList, std::weak_ptr<Cartridge> selected);
    }

    namespace View {
        void main                           (const Containers& containers, ScreenData::View& data);

        void gunTab                         (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, ScreenData::View::GunTab& data );
        void gunTab_eventsWindow            (std::shared_ptr<AssociatedGun> selectedGun, std::weak_ptr<Event>& selectedEvent);
        void gunTab_selectedGunInformation  (std::shared_ptr<AssociatedGun> selected);
        void gunTab_ammoUsedWindow          (std::shared_ptr<AssociatedGun> selected);

        void eventsTab                          (const std::map<EventMetadata, std::shared_ptr<Event>>& events, ScreenData::View::EventTab& data);
        void eventsTab_selectedEventInformation (std::shared_ptr<Event> selectedEvent, std::weak_ptr<GunAndAmmo> selectedGun);
        void eventsTab_gunsUsed                 (std::shared_ptr<Event> selected);

        void stockpileTab   (const std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>>& ammoList,
                             const std::set<Cartridge>& cartridgeList,
                             std::weak_ptr<AssociatedAmmo>& selected
                            );
    }

    // Helper functions
    void centerNextItemX(float x);
    void centerNextItemY(float y);

    void centerText         (const std::string& text);
    void centerTextDisabled (const std::string& text);
    bool centerButton       (const std::string& text, ImVec2 size);


    // Table drawing
    namespace Tables{
        void selectable_Cartridges(
                const std::map<Cartridge, int>& cartridges, 
                std::weak_ptr<Cartridge>& selected,
                ImVec2 size
            );
        void selectable_Guns(
                const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& list, 
                std::weak_ptr<AssociatedGun>& selected,
                ImVec2 size 
            );
        void selectable_Events( 
                const std::map<EventMetadata, std::shared_ptr<Event>>& events, 
                std::weak_ptr<Event>& selected,
                ImVec2 size
            );
        void selectable_EventGunsUsed(
                const std::vector<GunAndAmmo>& list, 
                GunAndAmmo& selected,
                ImVec2 size     
            );


        void eventAmmoUsed  (const GunAndAmmo& selected, ImVec2 size);
        void amountOfAmmo   (const std::map<AmmoMetadata, AmountOfAmmo>& ammoUsed, ImVec2 size);
    }


}
