#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Event.h"
#include "Window.h"
#include "Model.h"

#include <LAS/Logging.h>

#include <string>
#include <stdint.h>
#include <map> 
#include <vector>
#include <filesystem>

namespace ShooterCentral{

    class Framework final {
    public:
        Framework();
        ~Framework();

        struct Filepaths{
            std::string parentDir;
            std::string ammoDir;
            std::string eventsDir;
            std::string gunsDir;
        };

        bool setup(const std::string& directory);   // Call inside LASM_init
        
        SCWindowPtr getWindow() const;
        std::string getCommandGroupName() const;

    private:
        static constexpr  std::string commandGroupName { "sc" };

        SCWindowPtr window;
        Model       model;
    };

    namespace Setup {
        bool    setupFilesystem(Framework::Filepaths& paths); // Needs parentDir set first
        bool    setupWindow(SCWindowPtr window);
    }
}
