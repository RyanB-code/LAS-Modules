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

        static constexpr char FILENAME_DESCRIPTORS[] = "ItemDescriptors.json";


        bool readGuns           (const std::string& dir);
        bool readAmmo           (const std::string& dir);
        bool readEvents         (const std::string& dir);
        bool readDescriptors    (const std::string& dir);

        bool writeDescriptors   (std::string dirrectory);

        void buildAssociations();
        void addItemDescriptors(); // Things like Cartridge, Manufacuter, Location, etc
    };

    namespace Setup {
        bool    setupFilesystem(Framework::Filepaths& paths); // Needs parentDir set first
    }

    namespace FileIO {
        std::string makeFileName    (std::string directory, const GunMetadata& data);
        std::string makeFileName    (std::string directory, const AmmoMetadata& data);
        std::string makeFileName    (std::string directory, const Event& data);

        bool write (std::string directory, const GunMetadata& data);
        bool write (std::string directory, const AmountOfAmmo& data);
        bool write (std::string directory, const Event& data);
    }
}
