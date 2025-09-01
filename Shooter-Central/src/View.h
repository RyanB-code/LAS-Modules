#pragma once

#include "AssociatedItems.h"
#include "CommonItems.h"
#include "Containers.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <map>
#include <functional>

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

        AMMO_AMMO,
        AMMO_MANUFACTURER,

        GUN_AMMO_CARTRIDGE,

        GUN_GUN,
        GUN_WEAPON_TYPE,

        SIZE
    };


    static const Cartridge      EMPTY_CARTRIDGE     { };
    static const WeaponType     EMPTY_WEAPON_TYPE   { };
    static const GunMetadata    EMPTY_GUN_METADATA = {
        .cartridge      = EMPTY_CARTRIDGE,
        .weaponType     = EMPTY_WEAPON_TYPE
    };
    static const GunAndAmmo     EMPTY_GUN_AND_AMMO { EMPTY_GUN_METADATA };



    // To iterate over categories
    static constexpr std::array<Category, static_cast<size_t>(Category::SIZE)> CATEGORY_LIST {
        Category::NONE,
        Category::GUNS,
        Category::EVENTS,
        Category::STOCKPILE
    };
    static constexpr std::array<SubItem, static_cast<size_t>(SubItem::SIZE)> SUB_ITEM_LIST {
        SubItem::NONE,

        SubItem::EVENT_EVENT,
        SubItem::EVENT_TYPE,
        SubItem::EVENT_LOCATION,

        SubItem::AMMO_AMMO,
        SubItem::AMMO_MANUFACTURER,

        SubItem::GUN_AMMO_CARTRIDGE,

        SubItem::GUN_GUN,
        SubItem::GUN_WEAPON_TYPE,
    };



    // Holds data for what is shown that remains between frames
    namespace ScreenData{
        struct Home{
            bool showGuns       { true };
            bool showEvents     { true };
            bool showStockpile  { true };

            std::weak_ptr<Event>            selectedEvent;
            std::weak_ptr<AssociatedGun>    selectedGun;
            Cartridge                       selectedCartridge { };
        };

        struct View {
            struct EventTab {
                std::weak_ptr<Event>                        selectedEvent;
                std::reference_wrapper<const GunAndAmmo>    selectedGun     { std::cref(EMPTY_GUN_AND_AMMO) };
            };

            struct GunTab{
                std::weak_ptr<AssociatedGun>    selectedGun;
                std::weak_ptr<Event>            selectedEvent;
            };

            struct StockpileTab{
                Cartridge                       selectedCartridge { };
                std::weak_ptr<AssociatedAmmo>   selectedAmmo;
            };


            Category        category                { Category::NONE }; 
            std::string     categoryComboBoxText    { };

            EventTab        eventTab                { };
            GunTab          gunTab                  { };
            StockpileTab    stockpileTab            { };

        };

        struct Add {
            static constexpr int MAX_CHAR_INPUT { 64 };
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
        void gunWindow_selectedGunInformation   (const AssociatedGun& gun);
        void eventsWindow                       (const std::map<EventMetadata, std::shared_ptr<Event>>& events, std::weak_ptr<Event>& selected );
        void stockpileWindow                    (const std::map<Cartridge, int>& cartridgeList, Cartridge& selected);
    }

    namespace View {
        void main                               (const Containers& containers, ScreenData::View& data);

        void gunTab                             (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, ScreenData::View::GunTab& data );
        void gunTab_eventsWindow                (const AssociatedGun& gun, std::weak_ptr<Event>& selectedEvent);
        void gunTab_selectedGunInformation      (const AssociatedGun& gun);
        void gunTab_ammoUsedWindow              (const AssociatedGun& gun);

        void eventsTab                          (const std::map<EventMetadata, std::shared_ptr<Event>>& events, ScreenData::View::EventTab& data);
        void eventsTab_selectedEventInformation (const Event& event);
        void eventsTab_gunsUsed                 (const Event& event, std::reference_wrapper<const GunAndAmmo>& selectedGun );

        void stockpileTab(
                const std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>>& ammoList,
                const std::map<Cartridge, int>& cartridgeList,
                ScreenData::View::StockpileTab& selected
            );

    }
    namespace Add {
        void    main                    (const Containers& containers, ScreenData::Add& data);
        void    showExistingItemsWindow (const Containers& containers, const SubItem& selected, ImVec2 size); 
        void    addItemWindow           (const SubItem& selected);

        void    add_WeaponType          ();
    }


    // Helper functions
    void centerNextItemX            (float x);
    void centerNextItemY            (float y);
    void centerNextComboBoxX    (const std::string& text, float comboBoxWidth);

    void centerText         (const std::string& text);
    void centerTextDisabled (const std::string& text);
    bool centerButton       (const std::string& text, ImVec2 size);

    std::string categoryToString    (const Category& category,  const std::string& noneText=""); // noneText - Choose what to display when none is selected
    std::string subItemToString     (const SubItem& item,       const std::string& noneText=""); // noneText - Choose what to display when none is selected
                                                                                               


    // Table drawing
    namespace Tables{
        void selectable_Cartridges(
                const std::map<Cartridge, int>& cartridges, 
                Cartridge& selected,
                ImVec2 size
            );
        void selectable_SingleCartridgeAmmo(
                const std::map<AmmoMetadata, std::shared_ptr<AssociatedAmmo>>& list, 
                std::weak_ptr<AssociatedAmmo>& selected,
                ImVec2 size 
                );
        void selectable_KnownAmmo(
                const std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>& list,       
                std::weak_ptr<AmmoMetadata>& selected,
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
                std::reference_wrapper<const GunAndAmmo>& gun,
                ImVec2 size     
            );
        
        void amountOfAmmo   (const std::vector<AmountOfAmmo>& ammoUsed,                         ImVec2 size);
        void amountOfAmmo   (const std::map<AmmoMetadata, AmountOfAmmo>& ammoUsed,              ImVec2 size);

        void ammoGunsUsed   (const std::map<GunMetadata, std::shared_ptr<GunMetadata>>& list,   ImVec2 size);
    }

    namespace ListBoxes{
        void cartridges     (const std::map<Cartridge, std::shared_ptr<Cartridge>>& list,       ImVec2 size);
        void manufacturers  (const std::map<Manufacturer, std::shared_ptr<Manufacturer>>& list, ImVec2 size);
        void eventLocations (const std::map<Location, std::shared_ptr<Location>>& list,         ImVec2 size);
        void eventTypes     (const std::map<EventType, std::shared_ptr<EventType>>& list,       ImVec2 size); 
        void weaponTypes    (const std::map<WeaponType, std::shared_ptr<WeaponType>>& list,     ImVec2 size);
    }
    namespace ComboBoxes{
        void  category    (Category& selected);
        void  subItem     (SubItem& selected);
    }

}
