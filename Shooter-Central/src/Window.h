#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"

#include <LAS/Window.h>

#include <iostream> // FOR TESTING ONLY

namespace ShooterCentral{
    struct UISettings{
        const int MAX_LIST_NUM               { 32 };
        const int MAX_TEXT_INPUT_CHARS       { 32 };
        const int MAX_TEXT_INPUT_CHARS_NOTES { 256 };
    };
    struct EmptyObjects {
        const WeaponType    WEAPON_TYPE     { };
        const Cartridge     CARTRIDGE       { };
        const EventType     EVENT_TYPE      { };
        const Manufacturer  MANUFACTURER    { };
        const Location      LOCATION        { };
        const TrackedAmmo   TRACKED_AMMO    { };
        const Gun           GUN             { };
    };

    static  constexpr   UISettings     UI_SETTINGS     { };
    static  const       EmptyObjects   EMPTY_OBJECTS   { };

    class ShooterCentralWindow : public LAS::Windowing::Window {
    public:
        ShooterCentralWindow();
        ~ShooterCentralWindow();

        void draw() override;

        bool setAmmoTracker     (AmmoTrackerPtr setAmmoTracker);
        bool setGunTracker      (GunTrackerPtr setGunTracker);
        bool setEventTracker    (EventTrackerPtr setEventTracker);

        struct UnsavedChanges{
            bool armory     { false };
            bool stockpile  { false };
            bool events     { false };
        };

    private:
        AmmoTrackerPtr  ammoTracker;
        GunTrackerPtr   gunTracker;
        EventTrackerPtr eventTracker;

        bool showArmory { true }, showStockpile { true }, showEvents{ true };

        UnsavedChanges unsavedChanges { };
    };

    using SCWindowPtr = std::shared_ptr<ShooterCentralWindow>;

    namespace UIHelper{
        void    centerText              (std::string text);
        void    centerTextDisabled      (std::string text);
        bool    centerButton            (std::string text, ImVec2 buttonSize);
    }

    namespace ArmoryUI{
        void        home            (GunTrackerPtr gunTracker, bool& unsavedChanges,  const CartridgeList& cartridgeNames);
        void        viewGun         (std::shared_ptr<const Gun> gun);
        Gun         addGun          (bool& unsavedChanges, const CartridgeList& cartridges, const WeaponTypeList& wpnTypes);
        WeaponType  addWeaponType   (bool& unsavedChanges);

        int     verifyGun               (const std::string& name, const WeaponType& wt, const Cartridge& cartridge);    // Return is like Linux chmod numbering 4,2,1. 0 = passed
        bool    verifyWeaponTypeName    (const std::string& name);
    }

    namespace StockpileUI{
        void            home                    (AmmoTrackerPtr ammoTracker, bool& unsavedChanges);
        Cartridge       addCartridge            (bool& unsavedChanges);
        TrackedAmmo     addAmmoType             (bool& unsavedChanges, const CartridgeList& cartridgeNames, const ManufacturerList& manufacturerNames);
        Manufacturer    addManufacturer         (bool& unsavedChanges);
        TrackedAmmo     addToExistingAmmoType   (bool& unsavedChanges, const std::vector<ConstTrackedAmmoPtr>& ammoList);

        bool            verifyCartridgeName     (const std::string& name);
        bool            verifyManufacturerName  (const std::string& name);
        int             verifyAmmoType          (const std::string& name, const Manufacturer& manufacturer, const Cartridge& cartridge, int grainWeight);   // 8,4,2,1

        void            errorMakingAmmoTypePopupText(int verifyAmmoReturnVal);
    }

    namespace EventsUI{
        void    home                (EventTrackerPtr eventTracker,  AmmoTrackerPtr ammoTracker, GunTrackerPtr gunTracker, ShooterCentralWindow::UnsavedChanges& unsavedChanges);
        void    viewEvent           (std::shared_ptr<const Event> event);

        EventType                   addEventType    (bool& unsavedChanges);
        Location                    addLocation     (bool& unsavedChanges);

        // Apply to stockpile, apply to guns, event
        // Need to add separate verification function for this
        std::tuple<bool, bool, EventPtr>   addEvent        (GunTrackerPtr gunTracker,                  const EventTypeList& eventTypes,    const LocationList& locations, 
                                                            const ConstTrackedAmmoPtrList& ammoList,    bool& unsavedChanges);

        bool    verifyEventType     (const std::string& name);
        bool    verifyLocation      (const std::string& name);
        
        void    eventGunTable       (const std::vector<std::pair<ConstGunPtr, ConstTrackedAmmoPtr>>& list, bool showAmmoUsed);
        void    selectGunTable      (const ConstGunPtrList&         list,   Gun&         selectedGun);
        void    selectAmmoTable     (const ConstTrackedAmmoPtrList& list,   TrackedAmmo& selectedAmmo);
    }
}

