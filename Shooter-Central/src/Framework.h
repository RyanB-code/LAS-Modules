#pragma once

#include "Events.h"
#include "UI/UIControl.h"
#include "UI/UIHelperElements.h"
#include "UI/UIEvents.h"

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
#include "TestingItems.h"


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

    bool setup(const std::string& directory);   // Called inside LASM_init
    
    void update();
    void draw();

    static constexpr char TITLE[] = "Shooter Central";
    static constexpr char SHORT_TAG[] = "sc"; 

private:
    Database                database        { };
    UI::UIController        view            { };
    bool                    unsavedChanges  { false };
    Filepaths               paths           { };
};

bool    setupFilesystem (Framework::Filepaths& paths); // Needs parentDir set first
bool    save            (const Database& db, const Framework::Filepaths& paths);

    
}   // End SC namespace
