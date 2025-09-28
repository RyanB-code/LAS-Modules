#pragma once

#include "UI/UIControl.h"
#include "Container Items/AssociatedItems.h"
#include "CommonItems.h"
#include "Containers.h"
#include "Events.h"

#include <LAS/Logging.h>
#include <LAS/json.h>

#include <string>
#include <stdint.h>
#include <map> 
#include <vector>
#include <filesystem>
#include <memory>
#include <chrono>


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
        Containers          containers      { };
        UI::UIController    view            { };
        UnsavedChanges      unsavedChanges  { };

        std::shared_ptr<bool> shown;

        static constexpr char FILENAME_DESCRIPTORS[] = "ItemDescriptors.json";


        bool readDir_Guns            (const std::string& dir);
        bool readDir_Ammo            (const std::string& dir);
        bool readDir_Events          (const std::string& dir);
        bool readFile_Descriptors    (const std::string& dir);

        bool writeFile_Descriptors   (std::string dirrectory);

        void buildAssociations();
    };

    namespace Setup {
        bool    setupFilesystem(Framework::Filepaths& paths); // Needs parentDir set first
    }

    namespace FileIO { 

        std::string makeFileName    (std::string directory, const GunMetadata& data);
        std::string makeFileName    (std::string directory, const AmmoMetadata& data);
        std::string makeFileName    (std::string directory, const ShootingEventMetadata& data);

        bool write (std::string directory, const GunMetadata& data);
        bool write (std::string directory, const AmountOfAmmo& data);
        bool write (std::string directory, const ShootingEvent& data);

        void read_GunMetadata   (const LAS::json& j, ObjectBuffers::GunMetadata& buffer);
        void read_AmmoMetadata  (const LAS::json& j, ObjectBuffers::AmmoMetadata& buffer);
        void read_EventMetadata (const LAS::json& j, ObjectBuffers::ShootingEventMetadata& buffer);
    }
}
