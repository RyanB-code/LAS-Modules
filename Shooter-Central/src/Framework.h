#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Drill.h"
#include "Event.h"
#include "Window.h"

#include <LAS/Logging.h>

#include <string>
#include <stdint.h>
#include <map> 
#include <vector>

namespace ShooterCentral{
    class Framework final {
    public:
        Framework();
        ~Framework();

        struct Filesystem{
            std::string parentDir;
            std::string ammoDir;
            std::string drillsDir;
            std::string eventsDir;
            std::string gunsDir;
        };

        bool setup(LAS::Logging::LoggerPtr setLoggerPtr, const std::string& directory);   // Call inside LASM_init
        
        SCWindowPtr getWindow() const;
        std::string getCommandGroupName() const;

        bool                    setLogger(LAS::Logging::LoggerPtr setLogger);
        LAS::Logging::LoggerPtr getLogger() const;

    private:
        AmmoTrackerPtr  ammoTracker;
        GunTrackerPtr   gunTracker;
        EventTrackerPtr eventTracker;

        SCWindowPtr             window;
        LAS::Logging::LoggerPtr logger;

        static constexpr  std::string commandGroupName { "sc" };

        bool setupFilesystem    (Filesystem& filesystem);
        bool setupAmmoTracker   (std::string directory);
        bool setupGunTracker    (std::string directory);
        bool setupEventTracker  (std::string directory);

        bool setupWindow        ();
    };
}
