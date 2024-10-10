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

        void drawStockpile  ()  const;
        void drawArmory     ()  const;
        void drawEvents     ()  const;

        AmmoTrackerPtr  ammoTracker;
        GunTrackerPtr   gunTracker;
        EventTrackerPtr eventTracker;

    };

    using SCWindowPtr = std::shared_ptr<ShooterCentralWindow>;
}