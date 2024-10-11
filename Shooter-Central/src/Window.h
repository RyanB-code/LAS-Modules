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
        static constexpr int NUM_CARTRIDGE_NAMES_SHOWN { 10 };

        void drawHome       (ImVec2 windowSize)  const;
        void drawStockpile  ()  const;
        void drawEvents     ()  const;

        void drawArmoryQuickView(ImVec2 windowSize) const;
        void drawAddGun     () const;

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

