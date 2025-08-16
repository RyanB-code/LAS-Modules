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

    static const Cartridge EMPTY_CARTRIDGE { "NULL" };

    struct ScreenData_Home {
        bool showGuns       { true };
        bool showEvents     { true };
        bool showStockpile  { true };

        std::weak_ptr<Event>            selectedEvent;
        std::weak_ptr<AssociatedGun>    selectedGun;
        Cartridge                       selectedCartridge { EMPTY_CARTRIDGE };
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

    void centerNextItemX(float x);
    void centerNextItemY(float y);

    void centerText(const std::string& text);
    void centerTextDisabled(const std::string& text);
    bool centerButton(const std::string& text, ImVec2 size);

    void draw_Home      (const Containers& containers, ScreenData_Home& data, const UnsavedChanges& changes);
    void draw_HomeGuns  (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, std::weak_ptr<AssociatedGun>& selected );
    void draw_HomeGuns_GunInformation(std::shared_ptr<AssociatedGun> gun);
    void draw_HomeEvents(const std::map<EventMetadata, std::shared_ptr<Event>>& events, std::weak_ptr<Event>& selected );
    void draw_HomeStockpile(const std::map<Cartridge, int>& amountPerCartridgeList, Cartridge& selectedCartridge);

    void draw_View      (const Containers& containers, ScreenData_View& data);
    void draw_ViewGuns  (const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& guns, std::weak_ptr<AssociatedGun>& selected );
    void draw_ViewGuns_Details      (std::shared_ptr<AssociatedGun> selected);
    void draw_ViewGuns_Events       (std::shared_ptr<AssociatedGun> selected);
    void draw_ViewGuns_AmmoUsed     (std::shared_ptr<AssociatedGun> selected);

    void draw_ViewEvents            (const std::map<EventMetadata, std::shared_ptr<Event>>& events, std::weak_ptr<Event>& selected );
    void draw_ViewEvents_Details    (std::shared_ptr<Event> selected);
    void draw_ViewEvents_GunsUsed   (std::shared_ptr<Event> selected);


    void draw_SelectableGunTable(   const std::map<Cartridge, std::map<GunMetadata, std::shared_ptr<AssociatedGun>>>& list, 
                                    std::weak_ptr<AssociatedGun>& selected,
                                    ImVec2 size 
                                );
    void draw_SelectableEventGunTable(   const std::array<GunAndAmmo, Event::MAX_NUM_GUNS>& list, 
                                    GunAndAmmo& selected,
                                    ImVec2 size 
                                );
    void draw_EventAmmoUsedTable(const GunAndAmmo& selected, ImVec2 size);
   
    void draw_SelectableEventTable( const std::map<EventMetadata, std::shared_ptr<Event>>& events, 
                                    std::weak_ptr<Event>& selected,
                                    ImVec2 size
                                );
    void draw_AmountOfAmmoTable(  const std::map<AmmoMetadata, AmountOfAmmo>& ammoUsed, ImVec2 size);


}
