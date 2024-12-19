#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"

#include <LAS/Window.h>

#include <iostream>     // FOR TESTING

namespace ShooterCentral{
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

        static constexpr int MAX_LIST_NUM               { 10 };
        static constexpr int MAX_TEXT_INPUT_CHARS       { 32 };
        static constexpr int MAX_TEXT_INPUT_CHARS_NOTES { 256 };

        bool showArmory { true }, showStockpile { true }, showEvents{ true };

        bool unsavedChanges_Armory { false }, unsavedChanges_Stockpile { false }, unsavedChanges_Events { false };

        void drawHome       (ImVec2 windowSize);

        void drawArmoryUI   (const std::vector<ConstGunPtr>& gunList, const std::unordered_map<std::string, uint64_t>& roundsPerCartridge);
        void drawStockpile () const;
        void drawEvents    () const;

        // Supplementary functions 

        // Armory
        void drawAddGun         (bool& unsavedChanges) const;
        void drawAddWeaponType  (bool& unsavedChanges) const;

        // Stockpile
        void drawAddNewCartridge        (bool& unsavedChanges) const;
        void drawAddNewAmmoType         (bool& unsavedChanges) const;
        void drawAddNewManufacturer     (bool& unsavedChanges) const;
        void drawAddToExistingAmmoType  (bool& unsavedChanges) const;

        // Event Tracker
        void drawAddEventType       (bool& unsavedChanges) const;
        void drawAddLocation        (bool& unsavedChanges) const;
        void drawAddEvent           (bool& unsavedChanges) const;
        void drawViewEvent          (EventPtr selectedEvent) const;
        void drawEventGunTable      (std::vector<std::pair<Gun, TrackedAmmo>>& list, bool showAmmoUsed) const;

    };

    using SCWindowPtr = std::shared_ptr<ShooterCentralWindow>;

    namespace WindowHelper{
        void    centerText              (std::string text);
        void    centerTextDisabled      (std::string text);

        bool    centerButton            (std::string text, ImVec2 buttonSize);
        void    drawSelectGunTable      (const std::vector<ConstGunPtr>& gunList, Gun& selectedGun);
        void    drawSelectAmmoTable     (const std::vector<TrackedAmmoPtr>& ammoList, TrackedAmmo& selectedAmmo);

        // Drawing UI

    }
}

