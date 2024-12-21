#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"

#include <LAS/Window.h>

#include <iostream>     // FOR TESTING

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
        void                            home            (GunTrackerPtr gunTracker, bool& unsavedChanges,  const StringVector& cartridgeNames);
        std::pair<bool, Gun>            addGun          (bool& unsavedChanges, const StringVector& cartridgeNames, const StringVector& wpnTypeNames);
        std::pair<bool, std::string>    addWeaponType   (bool& unsavedChanges);
    }

    namespace StockpileUI{
        void                            home                    (AmmoTrackerPtr ammoTracker, bool& unsavedChanges);
        std::pair<bool, std::string>    addCartridge            (bool& unsavedChanges);
        std::pair<bool, TrackedAmmo>    addAmmoType             (bool& unsavedChanges, const StringVector& cartridgeNames, const StringVector& manufacturerNames);
        std::pair<bool, std::string>    addManufacturer         (bool& unsavedChanges);
        std::pair<bool, TrackedAmmo>    addToExistingAmmoType   (bool& unsavedChanges, const std::vector<ConstTrackedAmmoPtr>& ammoList);
    }

    namespace EventsUI{
        void    home                (EventTrackerPtr eventTracker,  bool& unsavedChanges);
        void    addEventType        (bool& unsavedChanges);
        void    addLocation         (bool& unsavedChanges);
        void    addEvent            (bool& unsavedChanges);
        void    viewEvent           (EventPtr selectedEvent);
        void    eventGunTable       (std::vector<std::pair<Gun, TrackedAmmo>>& list, bool showAmmoUsed);
    }


        void    drawSelectGunTable      (const std::vector<ConstGunPtr>& gunList, Gun& selectedGun);
        void    drawSelectAmmoTable     (const std::vector<TrackedAmmoPtr>& ammoList, TrackedAmmo& selectedAmmo);
}

