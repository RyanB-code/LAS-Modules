#pragma once

#include "UI/UIControl.h"
#include "UI/UIHelperElements.h"
#include "CommonItems.h"
#include "Events.h"

#include "Backend/DatabaseItems.h"
#include "Backend/Database.h"
#include "Backend/DatabaseFileIO.h"

#include <LAS/Logging.h>
#include <LAS/json.h>

#include <string>
#include <stdint.h>
#include <map> 
#include <vector>
#include <filesystem>
#include <memory>
#include <chrono>

#include <iostream> // For testing


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

        std::string miscDir;
    };

    bool setup(const std::string& directory, std::shared_ptr<bool> shown);   // Called inside LASM_init
    
    void update();
    void draw();

    static constexpr char TITLE[] = "Shooter Central";
    static constexpr char COMMAND_GROUP_NAME[] = "sc"; 

private:
    std::shared_ptr<bool>   shown;
    Database                database        { };
    UI::UIController        view            { };
    UnsavedChanges          unsavedChanges  { };
};

namespace Setup {
    bool    setupFilesystem(Framework::Filepaths& paths); // Needs parentDir set first
}
    
}   // End SC namespace
