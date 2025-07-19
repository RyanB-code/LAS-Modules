#pragma once

#include "AssociatedItems.h"
#include "CommonItems.h"
#include "View.h"
#include "Model.h"

#include <LAS/Logging.h>
#include <LAS/json.h>

#include <string>
#include <stdint.h>
#include <map> 
#include <vector>
#include <filesystem>
#include <memory>

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

        bool setup(const std::string& directory, std::shared_ptr<bool> shown);   // Call inside LASM_init
        
        std::string getCommandGroupName() const;

        void update();
        void draw();

        static constexpr char TITLE[] = "Shooter Central";
        static constexpr char COMMAND_GROUP_NAME[] = "sc"; 
    private:
        Model           model   { };
        View::GUI       view;

        UnsavedChanges  unsavedChanges { };

        std::shared_ptr<bool> shown;

        static constexpr char FILENAME_WEAPON_TYPES[] = "WeaponTypes.json";


        bool readGuns(const std::string& dir);
        bool readAmmo(const std::string& dir);
    };

    namespace Setup {
        bool    setupFilesystem(Framework::Filepaths& paths); // Needs parentDir set first
    }

    namespace FileIO {
        std::string makeFileName    (std::string directory, const GunMetadata& data);
        std::string makeFileName    (std::string directory, const AmmoMetadata& data);

        bool write (std::string directory, const GunMetadata& data);
        bool write (std::string directory, const AmountOfAmmo& data);
    }
}
