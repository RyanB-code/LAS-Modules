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

    addAllMetadataInfo(database);
    associateEvents(database);

    log_info("SC Setup sucessful");
    // END REAL SETUP DATA


    // Below is for testing display
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

}
void Framework::draw() {
    using namespace UI;

    bool attemptSave { false };
    view.draw(database, unsavedChanges, attemptSave);

    if(attemptSave){
        if(!save(database, paths)){
            SimpleClosePopup popup {"Saving Failed", "Failed saving database items"};
            UIEvents::PushPopup pushPopup { &popup };

            pushEvent(&pushPopup);
        }
        else
            unsavedChanges = false;
    }
}
bool setupFilesystem(Framework::Filepaths& paths){
    if(paths.parentDir.empty())
        return false;

    paths.parentDir = LAS::TextManip::ensureSlash(paths.parentDir);

    paths.ammoDir         = paths.parentDir + "Ammo/";
    paths.eventsDir       = paths.parentDir + "Events/";
    paths.gunsDir         = paths.parentDir + "Guns/";
    paths.miscDir         = paths.parentDir + "Misc/";

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
    return false;
}

}   // End SC namespace
