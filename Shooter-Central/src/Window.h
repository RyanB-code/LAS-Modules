#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"

#include <LAS/Window.h>

namespace ShooterCentral{
    struct UISettings{
        const int MAX_LIST_NUM               { 10 };
        const int MAX_TEXT_INPUT_CHARS       { 32 };
        const int MAX_TEXT_INPUT_CHARS_NOTES { 256 };
    };

    static constexpr UISettings UI_SETTINGS { };

    class ShooterCentralWindow : public LAS::Windowing::Window {
    public:
        ShooterCentralWindow();
        ~ShooterCentralWindow();

        void draw() override;

        bool setAmmoTracker     (AmmoTrackerPtr setAmmoTracker);
        bool setGunTracker      (GunTrackerPtr setGunTracker);
        bool setEventTracker    (EventTrackerPtr setEventTracker);

    private:
        AmmoTrackerPtr  ammoTracker;
        GunTrackerPtr   gunTracker;
        EventTrackerPtr eventTracker;

        bool showArmory { true }, showStockpile { true }, showEvents{ true };

        struct UnsavedChanges{
            bool armory     { false };
            bool stockpile  { false };
            bool events     { false };
        };

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
        GunPtr      addGun          (bool& unsavedChanges, const CartridgeList& cartridges, const WeaponTypeList& wpnTypes);
        WeaponType  addWeaponType   (bool& unsavedChanges);
    }

    namespace StockpileUI{
        void            home                    (AmmoTrackerPtr ammoTracker, bool& unsavedChanges);
        Cartridge       addCartridge            (bool& unsavedChanges);
        TrackedAmmo     addAmmoType             (bool& unsavedChanges, const CartridgeList& cartridgeNames, const ManufacturerList& manufacturerNames);
        Manufacturer    addManufacturer         (bool& unsavedChanges);
        TrackedAmmo     addToExistingAmmoType   (bool& unsavedChanges, const std::vector<ConstTrackedAmmoPtr>& ammoList);
    }

    namespace EventsUI{
        void    home                (EventTrackerPtr eventTracker,  AmmoTrackerPtr ammoTracker, std::shared_ptr<const GunTracker> gunTracker, bool& unsavedChangesEvents, bool& unsavedChangesStockpile);
        void    viewEvent           (std::shared_ptr<const Event> event);

        EventType                   addEventType    (bool& unsavedChanges);
        Location                    addLocation     (bool& unsavedChanges);
        std::pair<bool, EventPtr>   addEvent        (   bool& unsavedChanges,           const EventTypeList& eventTypes,    const LocationList& locations, 
                                                        const ConstGunPtrList& guns,    const ConstTrackedAmmoPtrList& ammoList );
        
        void    eventGunTable       (const std::vector<std::pair<Gun, TrackedAmmo>>& list, bool showAmmoUsed);
        void    selectGunTable      (const ConstGunPtrList&         gunList,    Gun& selectedGun);
        void    selectAmmoTable     (const ConstTrackedAmmoPtrList& ammoList,   TrackedAmmo& selectedAmmo);
    }
}

