#include "Framework.h"

using namespace LAS;

namespace ShooterCentral {

Framework::Framework() {

}
Framework::~Framework() {

}
bool Framework::setup(const std::string& directory){
    paths.parentDir = directory;

    if(!setupFilesystem(paths)){
        log_fatal("Failed to setup filesystem");
        return false;
    }

    if(!readGuns(database, std::filesystem::path(paths.gunsDir)))
        log_error("Failed reading Guns");
    if(!readAmmo(database, std::filesystem::path(paths.ammoDir)))
        log_error("Failed reading Ammo");
    if(!readEvents(database, std::filesystem::path(paths.eventsDir)))
        log_error("Failed reading Events");
     if(!readMetadataItems(database, std::filesystem::path(paths.metadataItems)))
        log_error("Failed reading Metadata Items");


    addAllMetadataInfo(database);
    associateEvents(database);

    log_info("SC Setup sucessful");
    // END REAL SETUP DATA


    // Below is for testing display
    /*
    std::cout << "\nEvents: \n";
    for(const auto& [info, e] : database.getEvents())
        printEvent(e);

    std::cout << "\nStockpile: \n";
    for(const auto& [key, cartMap] : database.getStockpile()){
        for(const auto& [info, stockpileAmmo] : cartMap)
            printStockpileAmmo(stockpileAmmo);
    }

    std::cout << "\nStockpile by cartridge: \n";
    for(const auto& c : database.getCartridges() ) {
        try {
            for(const auto& [info, stockpileAmmo] : database.getStockpile(c) )
                printAmountOfAmmo(stockpileAmmo.getAmountOfAmmo());
        }
        catch(...){
            continue;
        }  
    }

    std::cout << "\nArmory: \n";
    for(const auto& [key, cartMap] : database.getArmory()){
        for(const auto& [info, armoryGun] : cartMap)
            printArmoryGun(armoryGun);
    }

    std::cout << "\nArmory by cartridge: \n";
    for(const auto& c : database.getCartridges() ) {
        try { 
            for(const auto& [info, gun] : database.getArmory(c) )
                printGunMetadata(gun.getGunInfo());
        }
        catch(...){
            continue;
        }
    }


    std::cout << "\n\nManufacturers:\n";
    for(const auto& manufacturer : database.getManufacturers() ){
        std::cout << "  " << manufacturer.getName() << "\n";
    }
    std::cout << "\n\nLocations:\n";
    for(const auto& location : database.getLocations() ){
        std::cout << "  " << location.getName() << "\n";
    }
    std::cout << "\n\nEventTypes:\n";
    for(const auto& et : database.getEventTypes() ){
        std::cout << "  " << et.getName() << "\n";
    }
    std::cout << "\n\nCartridges:\n";
    for(const auto& c : database.getCartridges() ){
        std::cout << "  " << c.getName() << "\n";
    }
    std::cout << "\n\nWeaponTypes:\n";
    for(const auto& wt : database.getWeaponTypes() ){
        std::cout << "  " << wt.getName() << "\n";
    }
    */

    return true;
}
void Framework::update() {
    std::unique_ptr<DatabaseEvent> databaseEvent;
    std::unique_ptr<UIEvent> uiEvent;
    pollEvent(databaseEvent);
    pollEvent(uiEvent);

    // Must check bc if no event is queued, pollEvent returns nullptr
    if(databaseEvent){
        Status s {databaseEvent->execute(database)};

        if (!s.didSucceed)
            log_error(std::format("Database Event failed. What: {}", s.msg));
        else
            unsavedChanges = true;
    }
    if(uiEvent){
        Status s {uiEvent->execute(view)};

        // In future have it open a pop-up msg in the UI too
        if (!s.didSucceed)
            log_error(std::format("UI Event failed. What: {}", s.msg));
    }


    {
        using namespace UI;
        if(attemptSave){

            attemptSave = false;
            if(save(database, paths)){
                unsavedChanges = false;
            }
        }
    }

}
void Framework::draw() {
    view.draw(database, unsavedChanges, attemptSave); 
}
bool setupFilesystem(Framework::Filepaths& paths){
    if(paths.parentDir.empty())
        return false;

    paths.parentDir = LAS::TextManip::ensureSlash(paths.parentDir);

    paths.ammoDir         = paths.parentDir + "Ammo/";
    paths.eventsDir       = paths.parentDir + "Events/";
    paths.gunsDir         = paths.parentDir + "Guns/";
    paths.miscDir         = paths.parentDir + "Misc/";
    paths.metadataItems   = paths.miscDir + "MetadataItems.json";

    // Check paths are good
    if(!LAS::ensureDirectory(paths.ammoDir)){
        log_error(std::format("Error finding or creating directory [{}]", paths.ammoDir));
        return false;
    }
    if(!LAS::ensureDirectory(paths.eventsDir)){
        log_error(std::format("Error finding or creating directory [{}]", paths.eventsDir));
        return false;
    }
    if(!LAS::ensureDirectory(paths.gunsDir)){
        log_error(std::format("Error finding or creating directory [{}]", paths.gunsDir));
        return false;
    }
    if(!LAS::ensureDirectory(paths.miscDir)){
        log_error(std::format("Error finding or creating directory [{}]", paths.miscDir));
        return false;
    }
    return true;
}
bool save(const Database& db, const Framework::Filepaths& paths){
    std::vector<std::string> itemsNotSaved { };
    std::vector<std::string> itemsNotDeleted{ };

    if(!write(
            paths.metadataItems,
            db.getManufacturers(),
            db.getCartridges(),
            db.getWeaponTypes(),
            db.getLocations(),
            db.getEventTypes()
        ))
    {
        itemsNotSaved.emplace_back("All Metatadata Items");
    }

    // Iterate over directories and delete old json files since items could have been edited
    for(auto const& file : std::filesystem::directory_iterator(paths.ammoDir)){
        if(!std::string_view{ file.path().string() }.ends_with(".json"))
            continue;

        std::error_code ec { };
        if(!std::filesystem::remove(file.path(), ec))
            itemsNotDeleted.emplace_back( 
                    std::format("File: [{}] What: {}",
                        file.path().string(),
                        ec.message()
                    )
                );
    }
    for(auto const& file : std::filesystem::directory_iterator(paths.gunsDir)){
        if(!std::string_view{ file.path().string() }.ends_with(".json"))
            continue;

        std::error_code ec { };
        if(!std::filesystem::remove(file.path(), ec))
            itemsNotDeleted.emplace_back( 
                    std::format("File: [{}] What: {}",
                        file.path().string(),
                        ec.message()
                    )
                );
    }
    for(auto const& file : std::filesystem::directory_iterator(paths.eventsDir)){
        if(!std::string_view{ file.path().string() }.ends_with(".json"))
            continue;

        std::error_code ec { };
        if(!std::filesystem::remove(file.path(), ec))
            itemsNotDeleted.emplace_back( 
                    std::format("File: [{}] What: {}",
                        file.path().string(),
                        ec.message()
                    )
                );
    }



    // Write new files
    for(const auto& [cartridge, map] : db.getStockpile()){
        for(const auto& [key, item] : map){

            if(!write(paths.ammoDir, item)){
                itemsNotSaved.emplace_back(item.getAmmoInfo().name);
            }
        }
    }
    for(const auto& [cartridge, map] : db.getArmory()){
        for(const auto& [key, item] : map){

            if(!write(paths.gunsDir, item)){
                itemsNotSaved.emplace_back(item.getGunInfo().name);
            }
        }
    }
    for(const auto& [key, event] : db.getEvents()){
        if(!write(paths.eventsDir, event)){
            itemsNotSaved.emplace_back(eventName(event.getInfo()));
        }
    }
 

    // Early exit if everything succeeded
    if(itemsNotSaved.empty() && itemsNotDeleted.empty())
        return true;

    // Popup showing what failed to save
    auto bodyFunction = [itemsNotSaved, itemsNotDeleted]() {
        if(!itemsNotSaved.empty()){
            UI::centerText("Failed to Save the Following Items");

            ImGui::Separator();
            ImGui::Dummy( ImVec2{400, 0} );
            ImGui::Spacing();
            ImGui::Spacing();

            for(const auto& s : itemsNotSaved)
                ImGui::BulletText("%s", s.c_str());

            if(!itemsNotDeleted.empty()){
                ImGui::Spacing();
                ImGui::Spacing();
            }
        }
        if(!itemsNotDeleted.empty()){
            UI::centerText("Failed to Delete the Following Items");

            ImGui::Separator();
            ImGui::Dummy( ImVec2{400, 0} );
            ImGui::Spacing();
            ImGui::Spacing();

            for(const auto& s : itemsNotDeleted)
                ImGui::BulletText("%s", s.c_str());
        }

    };

    UI::CustomClosePopup popup { "Save Items Failed", bodyFunction };
    UI::UIEvents::PushPopup pushPopup { &popup };
    pushEvent(&pushPopup);

    return false;
}

}   // End SC namespace
