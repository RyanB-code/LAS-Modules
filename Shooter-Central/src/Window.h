#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"

#include <LAS/Window.h>

#include <iostream>

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

        static constexpr int MAX_LIST_NUM           { 10 };
        static constexpr int MAX_TEXT_INPUT_CHARS   { 32 };

        void drawHome       (ImVec2 windowSize)  const;

        void drawArmoryQuickView    () const;
        void drawStockpileQuickView () const;
        void drawEventsQuickView    () const;

        // Supplementary functions
        void drawAddGun         (bool& unsavedChanges) const;
        void drawAddWeaponType  (bool& unsavedChanges) const;

        AmmoTrackerPtr  ammoTracker;
        GunTrackerPtr   gunTracker;
        EventTrackerPtr eventTracker;

    };

    using SCWindowPtr = std::shared_ptr<ShooterCentralWindow>;

    namespace WindowHelper{
        void centerText(std::string text);
        bool centerButton(std::string text, ImVec2 buttonSize);
    }
}

